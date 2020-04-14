#include "public.h"

#if 0
#define DEBUGP 
#else
#define DEBUGP(format, args...)
#endif

#define SEA_BLOCK_INDEX_ENTRY_SIZE  (sizeof(struct sea_block_index_entry))
#define SEA_BLOCK_INDEX_HEADER_SIZE  (sizeof(struct sea_block_index_header))

//   index file format:    1 hdr + n entry(s)
typedef struct sea_block_index_header
{
    uint64_t block_id;
    uint64_t max_count;
    uint64_t start_timestamp;   /** 对应的数据文件开始时间戳 */ 
    uint64_t stop_timestamp;    /** 对应的数据文件结束时间戳 */
    uint32_t count;             /** 该索引文件中index entry的数量 */
    uint32_t unused;            /** 保留字段 */
} __attribute__ ((packed)) sea_block_index_header_t;

typedef struct sea_block_index_entry
{
    uint64_t timestamp;    
    uint64_t offset;  
} __attribute__ ((packed)) sea_block_index_entry_t;

struct sea_block_index {
    int fd;
    struct sea_block_index_header header;
};


void sea_block_index_filename(char *dir, uint64_t block_id, char *filename, int filename_length)
{
    snprintf(filename, filename_length, "%s/%llu/index", dir, block_id);
}

int sea_block_index_create(char *dir, uint64_t block_id, uint64_t max_count)
{
    int fd;
    uint64_t now;
    int ret = EINVAL;
    char filename[PATH_MAX];
    struct sea_block_index_header header;

    DEBUGP("%s: begin\n", __func__);

    /* dir/block_id */
    make_sub_dir(dir, block_id);
    sea_block_index_filename(dir, block_id, filename, PATH_MAX);
    
    fd = open(filename, O_RDWR | O_CREAT, 00755);
    if (fd == -1) {
        ret = errno;
        DEBUGP("%s: open fail, filename: %s, fd: %u\n", __func__, index_filename, fd);
        goto exit;
    }

    memset(&header, 0, sizeof(struct sea_block_index_header));
    header.block_id = block_id; 
    header.max_count = max_count; 
    header.count = 0;
    now = get_current_mills();
    header.start_timestamp = now;
    header.stop_timestamp = now;
    
    ret = write(fd, &header, sizeof(struct sea_block_index_header));
    if(ret == -1) {
        DEBUGP("%s: write header fail, errno: %d\n", __func__, errno);
        ret = errno;
        goto exit;
    }
    
    ret = 0;

exit:
    close(fd);
    return ret;
}

int sea_block_index_destroy(char *dir, uint64_t block_id)
{
    int ret = EINVAL;
    char filename[PATH_MAX];
    sea_block_index_filename(dir, block_id, filename, PATH_MAX);

    if (access(filename, F_OK) == -1) {
        ret = 0;
    } else {
        ret = unlink(filename);
        if(ret != 0) {
            ret = errno;
        } else {
        }
    }

    return ret;
}

struct sea_block_index *sea_block_index_open(char *dir, uint64_t block_id, int *error)
{
    struct sea_block_index *index = NULL;
    int fd = 0;
    int ret = EINVAL;
    char filename[PATH_MAX];
    struct sea_block_index_header header;
    
    sea_block_index_filename(dir, block_id, filename, PATH_MAX);

    fd = open(filename, O_RDWR);
    if(fd == -1) {
        ret = errno;
        goto exit;
    }
    int size = read(fd, (char *)&header, sizeof(struct sea_block_index_header));
    if (size != sizeof(struct sea_block_index_header)) {
        goto exit;
    }

    index = malloc(sizeof(struct sea_block_index));
    if(index == NULL) {
        ret = ENOMEM;
        goto exit;
    }

    memcpy(&index->header, &header, sizeof(struct sea_block_index_header));
    index->fd = fd;
    ret = 0;

   
exit:
    if(ret != 0) {
       if (index != NULL) {
            free(index);
       }
       if (fd > 0) {
            close(fd);
       }
    }

    *error = ret;

    return index;
}

void sea_block_index_close(struct sea_block_index *index)
{
    close(index->fd);
    free(index);
}


int sea_block_index_append(struct sea_block_index *index, uint64_t offset, uint32_t *record_id, int *overlimit)
{
    int ret = EINVAL;
    int size = 0;
    struct sea_block_index_entry entry;
    
    entry.timestamp = get_current_mills();    
    entry.offset = offset;

    *record_id = index->header.count;
    index->header.count ++;
    index->header.stop_timestamp = entry.timestamp;
    if (index->header.count >= index->header.max_count) {
        *overlimit = 1;
    } else {
        *overlimit = 0;
    }

    lseek(index->fd, 0, SEEK_END);
    size = write(index->fd, &entry, SEA_BLOCK_INDEX_ENTRY_SIZE);
    if (size != SEA_BLOCK_INDEX_ENTRY_SIZE) {
        goto exit;
    }

    lseek(index->fd, 0, SEEK_SET);  
    size = write(index->fd, &index->header, SEA_BLOCK_INDEX_HEADER_SIZE);
    if (size != SEA_BLOCK_INDEX_HEADER_SIZE) {
        ret = 0xff;
        goto exit;
    }

    ret = 0;

exit:
    if (ret == 0xff) {
        index->header.count --;
        ftruncate(index->fd, SEA_BLOCK_INDEX_HEADER_SIZE + SEA_BLOCK_INDEX_ENTRY_SIZE * index->header.count);   
    }
    return ret;
}

struct sea_block_offset *sea_block_index_query_by_count(struct sea_block_index *index, uint32_t first_record_id, uint32_t count, uint32_t *real_count)
{
    uint32_t read_count = 0;
    struct sea_block_offset *offset = NULL;

    lseek(index->fd, SEA_BLOCK_INDEX_HEADER_SIZE + SEA_BLOCK_INDEX_ENTRY_SIZE * first_record_id, SEEK_SET);     
    if (index->header.count <= first_record_id) {
        errno = EOF;
        goto exit;
    }

    if (index->header.count > first_record_id + count) {
        read_count = count;
    } else {
        read_count = index->header.count - first_record_id;
    }

    offset = sea_block_offset_malloc(); 
    
    struct sea_block_index_entry *entry_list = malloc(SEA_BLOCK_INDEX_ENTRY_SIZE * read_count);
    if (entry_list == NULL) {
        goto exit;
    }

    uint32_t size = read(index->fd, entry_list, SEA_BLOCK_INDEX_ENTRY_SIZE * read_count); 
    if (size != SEA_BLOCK_INDEX_ENTRY_SIZE * read_count) {
        goto exit;
    }

    for (int i = 0; i < read_count; i ++) {
        sea_block_offset_append(offset, entry_list[i].offset);
    }

    *real_count = read_count;

exit:
    if (entry_list != NULL) {
        free(entry_list);
    }

    return offset;
}

struct sea_block_offset *sea_block_index_query_by_time(struct sea_block_index *index, uint64_t start, uint64_t stop, uint32_t first_record_id, uint32_t count, uint32_t *real_count)
{
    uint32_t read_count = 0;
    uint32_t start_record_id = 0;
    struct sea_block_offset *offset = NULL;

    if (first_record_id == 0) {
		// todo fixme
        // start_record_id = sea_block_get_first_record_id(start);
    } else {
        start_record_id = first_record_id;
    }

    if (index->header.count <= start_record_id) {
        errno = EOF;
        goto exit;
    }


    lseek(index->fd, SEA_BLOCK_INDEX_HEADER_SIZE + SEA_BLOCK_INDEX_ENTRY_SIZE * start_record_id, SEEK_SET);     

    offset = sea_block_offset_malloc(); 
    struct sea_block_index_entry entry;
    while(start_record_id < index->header.count) {
        int size = read(index->fd, &entry, SEA_BLOCK_INDEX_ENTRY_SIZE);
        if (size != SEA_BLOCK_INDEX_ENTRY_SIZE) {
            goto exit;
        }
        if (entry.timestamp < stop) {
            sea_block_offset_append(offset, entry.offset);
        } else {
            break;
        }

        start_record_id ++;
        (*real_count) ++;
    }

exit:
    return offset;
}

struct sea_block_offset *sea_block_index_query_by_record_id(struct sea_block_index *index, uint32_t record_id)
{
    int size = 0;
    uint32_t read_count = 0;
    struct sea_block_index_entry entry;
    struct sea_block_offset *offset = NULL;

    if (index->header.count <= record_id) {
        errno = EOF;
        goto exit;
    }

    lseek(index->fd, SEA_BLOCK_INDEX_HEADER_SIZE + SEA_BLOCK_INDEX_ENTRY_SIZE * record_id, SEEK_SET);     

    offset = sea_block_offset_malloc(); 
    size = read(index->fd, &entry, SEA_BLOCK_INDEX_ENTRY_SIZE);
    if (size != SEA_BLOCK_INDEX_ENTRY_SIZE) {
        goto exit;
    }

    sea_block_offset_append(offset, entry.offset);

exit:
    return offset;
}

