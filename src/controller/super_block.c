#include "super_block.h"
#include "../block/word/md5.h"
#include "controller.h"

struct controller_super_block {
	char controller_name[32];
	char dir[PATH_MAX];
	uint64_t block_id;
};

union xx {
    uint64_t block_id;
    char output[256];
};

uint64_t controller_get_block_id(char *db_name)
{
    char output[256] = {0};
    union xx f;

    md5((unsigned char *)db_name, f.output, 256);

    return f.block_id;
}

int sync_super_block_from_disk(struct controller_super_block *super_block)
{
    int ret = EINVAL;
    uint64_t controller_bock_id = controller_get_block_id(super_block->controller_name);

    struct sea_block *block = sea_block_open(super_block->dir, controller_bock_id);
    if (block == NULL) {
        sea_block_create(super_block->dir, controller_bock_id, SEA_BLOCK_MAX_SIZE_DEFAULT, SEA_BLOCK_MAX_COUNT_DEFAULT);
    }

    block = sea_block_open(super_block->dir, controller_bock_id);
    if (block == NULL) {
        goto exit;
    }

    uint64_t block_id = 0;

    uint32_t ret_buf_len = 0;

	ret = sea_block_query_by_record_id(block, 0, (char *)block_id, 8, &ret_buf_len);
	if (ret == 0 && ret_buf_len != 0) {
		goto exit;
	}

    if (block_id == 0) {
        super_block->block_id = controller_bock_id + 1;
    } else {
        super_block->block_id = block_id;
    }

    ret = 0;

exit:
    if (block) {
        sea_block_close(block);
    }
    return ret;
}

int sync_super_block_to_disk(struct controller_super_block *super_block) 
{
    int ret = EINVAL;
    uint64_t controller_bock_id = controller_get_block_id(super_block->controller_name);

    struct sea_block *block = sea_block_open(super_block->dir, controller_bock_id);
    if (block == NULL) {
        goto exit;
    }

	int overlimit = 0;
	ret = sea_block_update(block, 0, (char *)&super_block->block_id, 8);
	if (ret != 0) {
		goto exit;
	}

    ret = 0;
    
exit:
    if (block) {
        sea_block_close(block);
    }
    return ret; 
}

struct controller_super_block *controller_super_block_open_or_create(char *dir, char *db_name, int *error)
{
    struct controller_super_block *super_block = malloc(sizeof(struct controller_super_block));
    if (super_block == NULL) {
        goto exit;
    }

    memcpy(super_block->controller_name, db_name, strlen(db_name));
    memcpy(super_block->dir, dir, strlen(dir));

    sync_super_block_from_disk(super_block);

exit:
    return super_block; 
}

int controller_super_block_get_block_id(struct controller_super_block *super_block, uint64_t *block_id)
{
    *block_id = super_block->block_id;
    return 0;
}

int controller_super_block_expire_block_id(struct controller_super_block *super_block, uint64_t block_id)
{
    super_block->block_id ++;
    sync_super_block_to_disk(super_block);
    return 0;
}

int controller_super_block_close(struct controller_super_block *super_block)
{
    free(super_block);
    return 0;
}
