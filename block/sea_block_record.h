#ifndef __SEA_BLOCK_RECORD_H
#define __SEA_BLOCK_RECORD_H

#include "public.h"


struct sea_block_record;

struct sea_block_record *sea_block_record_malloc();
void sea_block_record_free(struct sea_block_record *header);

int sea_block_record_append(struct sea_block_record *header, void *buf, uint32_t length);

typedef int (*sea_block_record_foreach_func)(void *record, int length, void *args);
int sea_block_record_foreach(struct sea_block_record *header, sea_block_record_foreach_func func, void *args);

int sea_block_record_get_first(struct sea_block_record *header, void *buf, uint32_t buf_len, uint32_t *ret_buf_len);
	
void *sea_block_record_get_first(struct sea_block_record *record_list, uint32_t *length);




#endif

