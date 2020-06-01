#ifndef SEA_GRAPH_DATA_H
#define SEA_GRAPH_DATA_H

#include "../include/public.h"

int create_node_data(struct sea_block *block, struct node_info *node,
                     struct graph_attr_info *attr_info, uint16_t attr_count,
                     struct graph_edge_info *edge_info, uint16_t edge_count,
                     uint32_t *node_id, uint32_t edge_free_size, uint32_t attr_free_size);
struct node_info *find_node_data(struct sea_block *block, uint32_t node_id, uint32_t *ret_buf_len);

#endif //SEA_GRAPH_DATA_H
