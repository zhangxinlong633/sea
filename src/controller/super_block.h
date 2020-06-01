#ifndef __SUPER_BLOCK_H
#define __SUPER_BLOCK_H

#include "../include/public.h"

struct controller_super_block;

struct controller_super_block *controller_super_block_open_or_create(char *dir, char *db_name, int *error);
int controller_super_block_get_block_id(struct controller_super_block *super_block, uint64_t *block_id);
int controller_super_block_expire_block_id(struct controller_super_block *super_block, uint64_t block_id);
int controller_super_block_close(struct controller_super_block *super_block);

#endif