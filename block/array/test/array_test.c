#include "../array.h"

int main(int argc, char **argv)
{
    int ret = EINVAL;
    struct col_array_info info;

    memset(&info, 0, sizeof(info));

    info.record_type = VALUE_TYPE_ARR_UINT32;
    snprintf(info.col_name, 32,  "col_name1");

    col_array_create("first", &info);

    int error  = 0;
    struct col_array *array = col_array_open("first", &error);
    if (array == NULL || error != 0) {
        goto exit;
    }

    uint32_t record_size = 1024*4;
    uint32_t *record = malloc(record_size);
    uint32_t *read_record = malloc(record_size);

    for (int i = 0; i < record_size /4; i++) {
        record[i] = i;
    }

    uint8_t over_limit = 0;

    for (int i = 0; i < 10000; i ++) {
        ret = col_array_append(array, (char *)record, record_size, &over_limit);
        if (ret != 0) {
            goto exit;
        }
    }

    uint32_t real_size;
    ret = col_array_read(array, 0, 1024, (char *)read_record, record_size, &real_size);
    if (ret != 0) {
        goto exit;
    }

    for (int i = 0; i < 1024; i ++) {
        if (record[i] != read_record[i]) {
            ret = ENOENT;
            goto exit;
        }
    }

    ret = 0;

exit:
    col_array_destroy("first");
    return ret;
}
