#ifndef __SEA_BLOCK_OFFSET_H
#define __SEA_BLOCK_OFFSET_H

#include "public.h"

struct sea_block_offset; 

struct sea_block_offset *sea_block_offset_malloc();
void sea_block_offset_free(struct sea_block_offset *offsets);

int sea_block_offset_append(struct sea_block_offset *offsets, uint64_t offset);

uint32_t sea_block_offset_count(struct sea_block_offset *offsets);
uint64_t sea_block_offset_get(struct sea_block_offset *offsets, uint32_t i);


#endif

