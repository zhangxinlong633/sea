#include "../../include/public.h"
#include "array.h"

struct col_array_hdr {
    uint32_t count;
    uint8_t record_type;
    char col_name[32];
    char data[0];
} __attribute__ ((packed));

struct col_array {
    int fd;
    struct col_array_hdr  hdr;
};

int col_array_create(char *path, struct col_array_info *info)
{
    int ret = EINVAL;
    int fd = 0;
    struct col_array_hdr hdr;

    if (info == NULL) {
        goto exit;
    }

    fd = open(path, O_CREAT|O_RDWR/*|O_DIRECT*/, 00755);
    if (fd == -1) {
        ret = errno;
        goto exit;
    }

    hdr.count = 0;
    hdr.record_type = info->record_type;
    memcpy(hdr.col_name, info->col_name, strlen(info->col_name));
    ret = write(fd, &hdr, sizeof(struct col_array_hdr));
    if (ret != sizeof(struct col_array_hdr)) {
        ret = errno;
        goto exit;
    }

exit:
    if (fd > 0) {
        close(fd);
    }
    return ret;
}

int col_array_destroy(char *path)
{
    int ret = EINVAL;
    char filename[PATH_MAX];

    if (access(path, F_OK) == -1) {
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


struct col_array *col_array_open(char *path, int *error)
{
    int ret = EINVAL;
    int fd = 0;
    struct col_array_hdr header;
    struct col_array *array = malloc(sizeof(struct col_array));
    if (array == NULL) {
        ret = ENOMEM;
        goto exit;
    }

    fd = open(path, O_RDWR);
    if(fd == -1) {
        ret = errno;
        goto exit;
    }
    int size = read(fd, (char *)&header, sizeof(struct col_array_hdr));
    if (size != sizeof(struct col_array_hdr)) {
        ret = errno;
        goto exit;
    }

    memcpy(&array->hdr, &header, sizeof(struct col_array_hdr));
    array->fd = fd;
    ret = 0;

exit:
    if(ret != 0) {
        if (array != NULL) {
            free(array);
        }
        if (fd > 0) {
            close(fd);
        }
    }

    *error = ret;

    return array;
}

void col_array_close(struct col_array *array)
{
    close(array->fd);
    free(array);
}

int col_array_append(struct col_array *array, void *record, uint32_t record_size, uint8_t *over_limit)
{
    int ret = EINVAL;
    uint32_t record_type_size = col_array_get_type_size(array->hdr.record_type);
    uint32_t record_count = (record_size / record_type_size);
    uint32_t real_record_size = record_count * record_type_size;

    uint64_t offset = lseek(array->fd, 0, SEEK_END);
    int write_size = write(array->fd, record, real_record_size);
    if (write_size != real_record_size) {
        ret = errno;

        ftruncate(array->fd, offset);
        goto exit;
    }

    // update hdr
    array->hdr.count += record_count;
    lseek(array->fd, 0, SEEK_SET);
    write(array->fd, &array->hdr, sizeof(struct col_array_hdr));


    ret = 0;

exit:
    return ret;
}

int col_array_read(struct col_array *array, uint32_t index_start, uint32_t index_stop, char *record, uint32_t record_size, uint32_t *real_record_count)
{
    int ret = EINVAL;
    uint32_t record_type_size = col_array_get_type_size(array->hdr.record_type);
    uint32_t real_buf_size = (record_size / record_type_size) * record_type_size;
    uint32_t index_count_size = (index_stop - index_start) * record_type_size;

    uint32_t real_limit_read_size = real_buf_size > index_count_size ? index_count_size : real_buf_size;


    uint64_t start_offset = index_start * record_type_size + sizeof(struct col_array_hdr);
    uint64_t offset = lseek(array->fd, start_offset, SEEK_SET);

    int read_size = read(array->fd, record, real_limit_read_size);
    if (read_size < 0) {
        ret = errno;
        goto exit;
    }

    *real_record_count = read_size / record_type_size;

    ret = 0;

exit:
    return ret;
}

