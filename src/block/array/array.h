#ifndef SEA_COL_DATA_H
#define SEA_COL_DATA_H

#include "../../include/public.h"

#define VALUE_TYPE_ARR_MIN       1
#define VALUE_TYPE_ARR_UINT8     1
#define VALUE_TYPE_ARR_INT8      2
#define VALUE_TYPE_ARR_UINT16    3
#define VALUE_TYPE_ARR_INT16     4
#define VALUE_TYPE_ARR_UINT32    5
#define VALUE_TYPE_ARR_INT32     6
#define VALUE_TYPE_ARR_UINT64    7
#define VALUE_TYPE_ARR_INT64     8
#define VALUE_TYPE_ARR_FLOAT     9
#define VALUE_TYPE_ARR_DOUBLE    10
#define VALUE_TYPE_ARR_UINT128   11
#define VALUE_TYPE_ARR_MAX       12


#define VALUE_TYPE_SIZE_MIN       1
#define VALUE_TYPE_SIZE_UINT8     1
#define VALUE_TYPE_SIZE_INT8      1
#define VALUE_TYPE_SIZE_UINT16    2
#define VALUE_TYPE_SIZE_INT16     2
#define VALUE_TYPE_SIZE_UINT32    4
#define VALUE_TYPE_SIZE_INT32     4
#define VALUE_TYPE_SIZE_UINT64    8
#define VALUE_TYPE_SIZE_INT64     8
#define VALUE_TYPE_SIZE_FLOAT     8
#define VALUE_TYPE_SIZE_DOUBLE    8
#define VALUE_TYPE_SIZE_UINT128   16
#define VALUE_TYPE_SIZE_MAX       16

struct col_array_info {
    uint8_t record_type;
    char col_name[32];
};


struct col_array;

int col_array_create(char *path, struct col_array_info *info);
int col_array_destroy(char *path);

struct col_array *col_array_open(char *path, int *error);
int col_array_append(struct col_array *array, void *record, uint32_t record_size, uint8_t *over_limit);
int col_array_read(struct col_array *array, uint32_t index_start, uint32_t index_stop, char *record, uint32_t record_size, uint32_t *real_record_size);
void col_array_close(struct col_array *array);

uint32_t col_array_get_type_size(uint8_t type) {
    uint32_t size = 0;

    switch (type) {
        case VALUE_TYPE_ARR_UINT8:
            size = VALUE_TYPE_SIZE_UINT8;
            break;
        case VALUE_TYPE_ARR_INT8:
            size = VALUE_TYPE_SIZE_INT8;
            break;
        case VALUE_TYPE_ARR_UINT16:
            size = VALUE_TYPE_SIZE_UINT16;
            break;
        case VALUE_TYPE_ARR_INT16:
            size = VALUE_TYPE_SIZE_INT16;
            break;
        case VALUE_TYPE_ARR_UINT32:
            size = VALUE_TYPE_SIZE_UINT32;
            break;
        case VALUE_TYPE_ARR_INT32:
            size = VALUE_TYPE_SIZE_INT32;
            break;
        case VALUE_TYPE_ARR_UINT64:
            size = VALUE_TYPE_SIZE_UINT64;
            break;
        case VALUE_TYPE_ARR_INT64:
            size = VALUE_TYPE_SIZE_INT64;
            break;
        case VALUE_TYPE_ARR_FLOAT:
            size = VALUE_TYPE_SIZE_FLOAT;
            break;
        case VALUE_TYPE_ARR_DOUBLE:
            size = VALUE_TYPE_SIZE_DOUBLE;
            break;
        case VALUE_TYPE_ARR_UINT128:
            size = VALUE_TYPE_SIZE_UINT128;
            break;
        default:
            size = VALUE_TYPE_SIZE_UINT32;
            break;
    }

    return size;
}

#endif
