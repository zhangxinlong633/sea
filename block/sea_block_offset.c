#include "public.h"

struct sea_block_offset {
    uint32_t count;
    uint32_t write_pos;
    uint32_t size;
    uint32_t pad;
    uint64_t offset[0];
};

#define INCREMENT_SIZE 128
struct sea_block_offset *sea_block_offset_malloc() 
{
    int malloc_size = sizeof(struct sea_block_offset) + INCREMENT_SIZE * sizeof(uint64_t);
    struct sea_block_offset *offset = malloc(malloc_size);
    if (offset == NULL) {
        goto exit;
    }
    memset(offset, 0, malloc_size);

    offset->size = malloc_size;
    
exit:
    return offset;
}

void sea_block_offset_free(struct sea_block_offset *header)
{
    free(header);
}

int sea_block_offset_append(struct sea_block_offset *header, uint64_t offset)
{
    int ret = EINVAL;

    if (header->write_pos >= header->count) {
        int size = (header->size - sizeof(struct sea_block_offset)) * 2 + sizeof(struct sea_block_offset);
        header = realloc(header, size);
        if (header == NULL) {
            goto exit;
        }
        header->count *= 2;
    }

    header->offset[header->write_pos++] = offset;
    ret = 0;

exit:
    return ret;
}

uint32_t sea_block_offset_count(struct sea_block_offset *header)
{
    return header->count;
}

uint64_t sea_block_offset_get(struct sea_block_offset *header, uint32_t i)
{
    return header->offset[i];
}

