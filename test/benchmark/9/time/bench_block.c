#include "../../src/include/public.h"
#include "../../src/block/time/sea_block.h"


#define COUNT 10000 * 10000
char *dir = "/Users/zhangxinlong/tmp/block";
uint64_t block_id = 0;

int insert_test()
{
    uint32_t max_size = 128 * 1024 * 1024;
    uint32_t max_count = 128 * 1024;

    sea_block_create(dir, block_id, max_size, max_count);
    struct sea_block *block = sea_block_open(dir, block_id);

    char buf[102400] = {0};
    uint32_t start = time(NULL);
    for (int i = 0; i < COUNT; i += 100) {
        uint32_t record_id = 0;
        int over_limit = 0;
        memset(buf, i % 0xff, 102400);
        sea_block_append(block, buf, 102400, &record_id, &over_limit);
        if (over_limit) {
            block_id ++;
            sea_block_create(dir, block_id, max_size, max_count);
            block = sea_block_open(dir, block_id);
        }

        if (i % 1000000 == 0) {
            uint32_t stop = time(NULL);
            printf("write %u from %u to %u, speed: %u\n", i, start, stop, i / ((stop - start) > 0 ? (stop - start) : 1));
        }
    }

    sea_block_close(block);

    return 0;
}

int foreach_func_test(void *record, int length, void *args)
{
    return 0;
}

int query_by_record_id_test()
{
    // by count
    char *dir = "/Users/zhangxinlong/tmp/data";

    char buf[2048] = {0};
    uint32_t buf_len = 2048;
    uint32_t ret_buf_len = 0;
    uint32_t start = time(NULL);

    for (int i = block_id; i < 100; i ++) {
        struct sea_block *block = sea_block_open(dir, 0);
        for (int j = 0;  j < 8096; j ++) {
            sea_block_query_by_record_id(block, j, buf, buf_len, &ret_buf_len);
        }
        sea_block_close(block);

        if (i % 1000000 == 0) {
            uint32_t stop = time(NULL);
            printf("read %u from %u to %u, speed: %u\n", i, start, stop, i / ((stop - start) > 0 ? (stop - start) : 1));
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    insert_test();
    query_by_record_id_test();
    return 0;
}

