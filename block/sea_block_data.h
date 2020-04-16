#ifndef __SEA_BLOCK_H__
#define __SEA_BLOCK_H__

#include "public.h"

struct sea_block_data;

int sea_block_data_create(char *dir, uint64_t block_id, uint64_t max_size);
int sea_block_data_destroy(char *dir, uint64_t data_block_id);

struct sea_block_data *sea_block_data_open(char *dir, uint64_t data_block_id);
int sea_block_data_close(struct sea_block_data *data);

int sea_block_data_append(struct sea_block_data *data, void *buffer, uint32_t length, int *overlimit, uint64_t *offset);
struct sea_block_record *sea_block_data_query_by_offsets(struct sea_block_data *data, struct sea_block_offset *offsets);

#endif


