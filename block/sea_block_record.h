#ifndef __SEA_BLOCK_RECORD_H
#define __SEA_BLOCK_RECORD_H

#include "public.h"

#define VALUE_TYPE_MIN       1
#define VALUE_TYPE_UINT16    1
#define VALUE_TYPE_INT16     2
#define VALUE_TYPE_UINT32    3
#define VALUE_TYPE_INT32     4
#define VALUE_TYPE_UINT64    5
#define VALUE_TYPE_INT64     6
#define VALUE_TYPE_FLOAT     7
#define VALUE_TYPE_DOUBLE    8
#define VALUE_TYPE_UINT128   9
#define VALUE_TYPE_MAX       9

#define RECORD_COLS_MAX       32
#define RECORD_COLS_NAME_LEN  32

struct sea_record_desc {
    uint8_t key_type;
    uint8_t value_nums;
    uint8_t value_types[RECORD_COLS_MAX];
    uint8_t value_names[RECORD_COLS_MAX][RECORD_COLS_NAME_LEN];
};

struct sea_block_record;

struct sea_block_record *sea_block_record_malloc();
struct sea_block_record *sea_block_record_malloc_single_record(struct sea_record_desc *desc);
struct sea_block_record *sea_block_record_malloc_structure(struct sea_record_desc *desc, uint8_t is_in_single_record);
void sea_block_record_free(struct sea_block_record *header);

int sea_block_record_append(struct sea_block_record *header, void *buf, uint32_t length);

typedef int (*sea_block_record_foreach_func)(void *record, int length, void *args);
int sea_block_record_foreach(struct sea_block_record *header, sea_block_record_foreach_func func, void *args);

void *sea_block_record_get_first(struct sea_block_record *record_list, uint32_t *length);


#endif

