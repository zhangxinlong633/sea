#include "public.h"

struct sea_block {
    char *dir;
    uint64_t block_id; //文件ID
	struct sea_block_data *data;
	struct sea_block_index *index;
};

int sea_block_create(char *dir, uint64_t block_id, uint64_t max_size, uint64_t max_count)
{   
    int ret = EINVAL;    
    
    if (max_size == 0) {
        max_size = SEA_BLOCK_MAX_SIZE_DEFAULT;
    }

    if (max_count == 0) {
        max_count = SEA_BLOCK_MAX_COUNT_DEFAULT;
    }

    ret = sea_block_index_create(dir, block_id, max_count);
    if (ret != 0) {
        goto exit;
    }
    ret = sea_block_data_create(dir, block_id, max_size);
	if (ret != 0) {
		goto exit;
	}

exit:
	return ret;
}

void sea_block_destroy(char *dir, uint64_t block_id)
{
	sea_block_data_destroy(dir, block_id);
	sea_block_index_destroy(dir, block_id);
}

struct sea_block *sea_block_open(char *dir, uint64_t block_id)
{
	struct sea_block *block = malloc(sizeof(struct sea_block));
	if (block == NULL) {
		goto exit;
	}

	block->dir = malloc(strlen(dir)+1);
	if (block->dir == NULL) {
		goto exit;
	}

	memset(block->dir, 0, strlen(dir) + 1);
	memcpy(block->dir, dir, strlen(dir));
	block->block_id = block_id;

	int error = 0;
	block->data = sea_block_data_open(dir, block_id);
	if (block->data== NULL) {
		goto exit;
	}

	block->index = sea_block_index_open(dir, block_id, &error);
	if (block->index== NULL) {
		goto exit;
	}

exit:
	return block;		
}

void sea_block_close(struct sea_block *block)
{
	free(block->dir);
	if (block->data != NULL) {
		sea_block_data_close(block->data);
	}
	if (block->index != NULL) {
		sea_block_index_close(block->index);
	}
	free(block);
}

int sea_block_append(struct sea_block *block, void *data, int length, uint32_t *record_id, int *overlimit)
{
	int ret = EINVAL;
	if (block == NULL || block->index == NULL || data == NULL || length == 0) {
		goto exit;
	}

	uint64_t offset = 0;
	ret = sea_block_data_append(block->data, data, length, overlimit, &offset);
	if (ret != 0) {
		goto exit;
	}

	ret = sea_block_index_append(block->index, offset, record_id, overlimit);
	if (ret != 0) {
		goto exit;
	} 

	ret = 0;
	
exit:
	return ret;
}

struct sea_block_record *sea_block_query_by_count(struct sea_block *block, uint32_t current_record_id, uint32_t count, uint32_t *real_count)
{
	struct sea_block_record *record_list = NULL;

	struct sea_block_offset *offsets = sea_block_index_query_by_count(block->index, current_record_id, count, real_count);
	if (offsets == NULL) {
		goto exit;
	}

	record_list = sea_block_data_query_by_offsets(block->data, offsets);
	
exit:
	if (offsets != NULL) {
		sea_block_offset_free(offsets);
	}

	return record_list;
}

struct sea_block_record *sea_block_query_by_time(struct sea_block *block, uint64_t start, uint64_t stop, uint32_t first_record_id, uint32_t count, uint32_t *real_count)
{
	struct sea_block_record *record_list = NULL;

	struct sea_block_offset *offsets = sea_block_index_query_by_time(block->index, start, stop, first_record_id, count, real_count);
	if (offsets == NULL) {
		goto exit;
	}

	record_list = sea_block_data_query_by_offsets(block->data, offsets);
	
exit:
	if (offsets != NULL) {
		sea_block_offset_free(offsets);
	}

	return record_list;
}

struct sea_block_record *sea_block_query_by_record_id(struct sea_block *block, uint32_t record_id)
{
	struct sea_block_record *record_list = NULL;

	struct sea_block_offset *offsets = sea_block_index_query_by_record_id(block->index, record_id);
	if (offsets == NULL) {
		goto exit;
	}

	record_list = sea_block_data_query_by_offsets(block->data, offsets);
	
exit:
	if (offsets != NULL) {
		sea_block_offset_free(offsets);
	}

	return record_list;
}

