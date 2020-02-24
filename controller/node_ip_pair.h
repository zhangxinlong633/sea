#ifndef __NODE_IP_PAIR_H
#define __NODE_IP_PAIR_H

struct node_ip_pair *node_ip_pair_create();
void node_ip_pair_create(struct node_ip_pair *pair);

int node_ip_pair_insert(struct node_ip_pair *pair, struct addr *ip, uint8_t is_ipv6);

int node_ip_pair_get_ip_by_node(struct node_ip_pair *pair, uint32_t node_id, struct addr *ip, uint8_t *is_ipv6);
int node_ip_pair_get_node_by_ip(struct node_ip_pair *pair, struct addr *ip, uint8_t is_ipv6, uint32_t node_id);

int node_ip_pair_init()
int node_ip_pair_fini()

#endif
