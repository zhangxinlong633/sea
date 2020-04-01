#ifndef __GOSSIP_LIST_H
#define __GOSSIP_LIST_H

#include "../include/public.h"

struct gossip_list;
int gossip_list_init();
void gossip_list_fini();

struct gossip_list *gossip_list_create(uint32_t node_cap);
void gossip_list_destroy(struct gossip_list *gossip_list);

int gossip_list_put(struct gossip_list *gossip_list, uint64_t block_id, uint32_t *node_id, uint32_t node_len);
int gossip_list_get(struct gossip_list *gossip_list, uint64_t block_id, uint32_t *node_id, uint32_t node_len, uint32_t ret_node_len);

#endif

