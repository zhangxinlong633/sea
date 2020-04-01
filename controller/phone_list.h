#ifndef __PHONE_LIST_H
#define __PHONE_LIST_H

#include "../include/public.h"

struct phone_list;
int phone_list_init();
int phone_list_fini();

struct phone_list *phone_list_create(uint32_t node_cap);
void phone_list_destroy(struct phone_list *phone_list);

int phone_list_put(struct phone_list *phone_list, uint64_t block_id, uint32_t *node, uint32_t node_len);
int phone_list_get(struct phone_list *phone_list, uint64_t block_id, uint32_t *node, uint32_t node_len, uint32_t *ret_node_len);
int phone_list_save(char *path, int path_len);
int phone_list_load(char *path, int path_len);

#endif

