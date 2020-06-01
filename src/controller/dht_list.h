#ifndef SEA_DHT_LIST_H
#define SEA_DHT_LIST_H

#include "../include/public.h"

int dht_list_get_free_node(uint32_t *ip);
int dht_list_get_node(uint64_t block_id, uint32_t record_id, uint32_t *ip);

int dht_list_init(int port, int node_list_len, char **node_list);
void dht_list_fini();

#endif //SEA_DHT_LIST_H
