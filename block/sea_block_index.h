#ifndef __SEA_BLOCK_INDEX_H
#define __SEA_BLOCK_INDEX_H

#include "public.h"

struct sea_block_index;

int sea_block_index_create(char *dir, uint64_t block_id, uint64_t max_count);
int sea_block_index_destroy(char *dir, uint64_t block_id);

struct sea_block_index *sea_block_index_open(char *dir, uint64_t block_id, int *error);
void sea_block_index_close(struct sea_block_index *index);

int sea_block_index_append(struct sea_block_index *index, uint64_t offset, uint32_t *record_id, int *overlimit);

struct sea_block_offset *sea_block_index_query_by_count(struct sea_block_index *index, uint32_t first_record_id, uint32_t count, uint32_t *real_count);
struct sea_block_offset *sea_block_index_query_by_time(struct sea_block_index *index, uint64_t start, uint64_t stop, uint32_t first_record_id, uint32_t count, uint32_t *real_count);
struct sea_block_offset *sea_block_index_query_by_record_id(struct sea_block_index *index, uint32_t record_id);

void sea_block_offset_free(struct sea_block_offset *offsets);

#endif

