#include "../../src/include/public.h"
#include "./

#define COUNT 10000 * 10000 * 10

int main(int argc, char **argv)
{
    int ret = EINVAL;
    struct col_array_info info;

    memset(&info, 0, sizeof(info));

    info.record_type = VALUE_TYPE_ARR_UINT32;
    snprintf(info.col_name, 32,  "col_name1");
    char *dir = "/Users/zhangxinlong/tmp/first";

    col_array_create(dir, &info);

    int error  = 0;
    struct col_array *array = col_array_open(dir, &error);
    if (array == NULL || error != 0) {
        goto exit;
    }

    uint32_t record_count = 1024*100;
    uint32_t record_size = record_count * 4;
    uint32_t *record = malloc(record_size);

    for (int i = 0; i < record_size /4; i ++) {
        record[i] = i;
    }

    uint32_t start = time(NULL);
    uint8_t over_limit = 0;

    for (int i = 0; i < COUNT; i += record_count) {
        ret = col_array_append(array, (char *)record, record_size, &over_limit);
        if (ret != 0) {
            goto exit;
        }
    }
    uint32_t stop = time(NULL);
    printf("array bench count: %u, from %u to %u, speed: %u\n", COUNT, start, stop, COUNT / ((stop - start) > 0 ? (stop - start) : COUNT));

    ret = 0;

exit:
    col_array_destroy(dir);
    return ret;
}
