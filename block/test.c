#include "public.h"

int insert_test()
{
    char *dir = "~/tmp/data";
    uint64_t block_id;
    uint32_t max_size = 128 * 1024 * 1024;
    uint32_t max_count = 128 * 1024;
    sea_block_create(dir, block_id, max_size, max_count);
    struct sea_block *block = sea_block_open(dir, block_id);
    char buf[1024] = {0};
    for (int i = 0; i < 1000; i ++) {
        memset(buf, i, 1024);
		uint32_t record_id = 0;
		int overlimit = 0;
        sea_block_append(block, buf, 1024, &record_id, &overlimit);
    }
    sea_block_close(block);
	return 0;
}

int foreach_func_test(void *record, int length, void *args)
{
	return 0;
}

int query_by_count_test()
{
    // by count
    char *dir = "~/tmp/data";
    struct sea_block *block = sea_block_open(dir, 0);
    int count = 1024;
    uint32_t real_count = 0;
    struct sea_block_record *record_list = sea_block_query_by_count(block, 0, count, &real_count);

	sea_block_record_foreach(record_list, foreach_func_test, NULL);
    sea_block_record_free(record_list);
	return 0;
}

int query_by_time_test()
{
    // by time
    char *dir = "~/tmp/data";
    struct sea_block *block = sea_block_open(dir, 0);
    uint32_t start = 0;
    uint32_t stop = 0xffffffff;
    uint32_t real_count = 0;
    struct sea_block_record *record_list = sea_block_query_by_time(block, start, stop, 0, 100, &real_count);
	sea_block_record_foreach(record_list, foreach_func_test, NULL);
    sea_block_record_free(record_list);

    sea_block_close(block);
	return 0;
}

int query_by_record_id_test()
{
    // by count
    char *dir = "~/tmp/data";
    uint64_t record_id = 0;
    struct sea_block *block = sea_block_open(dir, 0);
    struct sea_block_record *record_list = sea_block_query_by_record_id(block, record_id);
	sea_block_record_foreach(record_list, foreach_func_test, NULL);
    sea_block_record_free(record_list);

    sea_block_close(block);
	return 0;
}

int main(int argc, char **argv)
{
    insert_test();
    query_by_count_test();
    query_by_time_test();
    query_by_record_id_test();
    // clone test ??
	return 0;
}

