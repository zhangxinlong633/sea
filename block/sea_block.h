#ifndef SEA_BLOCK_H
#define SEA_BLOCK_H

#include "public.h"

struct sea_block;
int sea_block_create(char *dir, uint64_t block_id, uint64_t max_size, uint64_t max_count);
void sea_block_destroy(char *dir, uint64_t block_id);
struct sea_block *sea_block_open(char *dir, uint64_t block_id);
void sea_block_close(struct sea_block *block);

#define SEA_BLOCK_MAX_SIZE_DEFAULT (128*1024*1024)
#define SEA_BLOCK_MAX_COUNT_DEFAULT (128*1024)

int sea_block_append(struct sea_block *block, void *data, int length, uint32_t *record_id, int *overlimit);
struct sea_block_record *sea_block_query_by_count(struct sea_block *block, uint32_t first_reocrd_id, uint32_t count, uint32_t *real_count);
struct sea_block_record *sea_block_query_by_time(struct sea_block *block, uint64_t start, uint64_t stop, uint32_t first_reocrd_id, uint32_t count, uint32_t *real_count);
int sea_block_query_by_record_id(struct sea_block *block, uint32_t record_id, void *buf, int buf_len, uint32_t *ret_buf_len);

int sea_block_update(struct sea_block *block, uint32_t record_id, void *data, int length);


#endif

