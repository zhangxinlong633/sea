#include "public.h"

#if 0
#define DUMP_DATA   1
#define DEBUGP sea_log_error 
#else
#define DEBUGP(format, args...)
#endif

typedef struct sea_block_data_header
{
    uint8_t     magic;  
    uint8_t     disable; 
    uint16_t    length;
} __attribute__ ((packed)) sea_block_data_header;

typedef struct sea_block_data_tail
{
    uint16_t    length;
    uint8_t     pad;
    uint8_t     magic;
} __attribute__ ((packed)) sea_block_data_tail;

struct sea_block_data {
    int fd;
};

void sea_block_data_filename(char *dir, uint64_t block_id, char *filename, int filename_len)
{
    snprintf(filename, filename_len, "%s/%llu/%llu", dir, block_id, block_id);
}


int sea_block_data_create(char *dir, uint64_t block_id, uint64_t max_size)
{
    int ret = 0;
    int fd = 0;
    char filename[PATH_MAX];

    sea_block_data_filename(dir, block_id, filename, PATH_MAX);

    fd = open(filename, O_CREAT|O_RDWR/*|O_DIRECT*/, 00755);
    if (fd == -1) {
        ret = errno;
    }
    else {
        close(fd);
    }

    return ret;
}

int sea_block_data_destroy(char *dir, uint64_t block_id)
{
    int ret = EINVAL;
    char filename[PATH_MAX];

    sea_block_data_filename(dir, block_id, filename, PATH_MAX);
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

struct sea_block_data *sea_block_data_open(char *dir, uint64_t block_id)
{
    int ret = EINVAL;
    int fd = 0;
    char filename[PATH_MAX];
    struct sea_block_data *data = NULL;

    sea_block_data_filename(dir, block_id, filename, PATH_MAX);

    fd = open(filename, O_RDWR /*| O_DIRECT*/);
    if(fd == -1) {
        ret = errno;
        goto exit;
    }

    data = (struct sea_block_data *)malloc(sizeof(struct sea_block_data));
    if (data == NULL) {
        ret = ENOMEM;
        goto exit;
    }

    data->fd = fd;

    ret = 0;

exit:
    DEBUGP("%s: finish, ret: %d\n", __func__, ret);
    return data;
}

int sea_block_data_close(struct sea_block_data *data)
{
    if (data != NULL) {
        if (data->fd > 0) {
            close(data->fd);
        }
    }
	return 0;
}

#define SEA_BLOCK_DATA_MAGIC_HEADER '^'
#define SEA_BLOCK_DATA_MAGIC_TAIL 	'$'
int sea_block_data_append(struct sea_block_data *data, void *buffer, uint32_t length, int *overlimit, uint64_t *offset)
{
    int ret = EINVAL;
    struct sea_block_data_header header;
    struct sea_block_data_tail tail;

    header.magic = SEA_BLOCK_DATA_MAGIC_HEADER;
    header.length = length;
    header.disable = 0;

    tail.length = length;
    tail.magic = SEA_BLOCK_DATA_MAGIC_TAIL;
    tail.pad = 0;

    int buf_len = sizeof(struct sea_block_data_header) + length + sizeof(struct sea_block_data_tail);
    char *buf = malloc(buf_len);
    if (buf == NULL) {
        goto exit;
    }
    memcpy(buf, &header, sizeof(struct sea_block_data_header));
    memcpy(buf + sizeof(struct sea_block_data_header), buffer, length);
    memcpy(buf + sizeof(struct sea_block_data_header) + length, &tail, sizeof(struct sea_block_data_tail));

    *offset = lseek(data->fd, 0, SEEK_END);
    uint32_t size = write(data->fd, buf, buf_len);
    if (size != length) {
        ret = ENOENT;
        goto exit;
    }

    ret = 0;

exit:
    return ret;
}

struct sea_block_record *sea_block_data_query_by_offsets(struct sea_block_data *data, struct sea_block_offset *offsets)
{
    struct sea_block_data_header header;
    struct sea_block_record *record_list = sea_block_record_malloc();

    int size = 0;
    for (int i = 0; i < sea_block_offset_count(offsets); i ++) {
        uint64_t offset = sea_block_offset_get(offsets, i);
        lseek(data->fd, offset, SEEK_CUR);
        size = read(data->fd, &header, sizeof(struct sea_block_data_header));
        if (size != sizeof(struct sea_block_data_header)) {
            goto exit;
        }
        
        lseek(data->fd, offset + sizeof(struct sea_block_data_header), SEEK_SET);
        char *buf = malloc(header.length);
        if (buf == NULL) {
            goto exit;
        }

        size = read(data->fd, &header, header.length);
        if (size != header.length) {
            goto exit;
        }

        sea_block_record_append(record_list, buf, header.length);
    }

exit:
    return record_list;
}

