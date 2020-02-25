#include "gossip_list.h"

struct gossip_list {
	uint32_t node_cap;
};

int gossip_list_init()
{
	return 0;
}

int gossip_list_fini()
{
	return;
}

struct gossip_list *gossip_list_create(uint32_t node_cap)
{
	return NULL;
}

void gossip_list_destroy(struct gossip_list *gossip_list)
{
	return;
}

int gossip_list_put(struct gossip_list *gossip_list, uint64_t block_id, uint32_t *node_id, uint32_t node_len)
{
	return 0;
}

int gossip_list_get(struct gossip_list *gossip_list, uint64_t block_id, uint32_t *node_id, uint32_t node_len, uint32_t ret_node_len)
{
	return 0;
}


