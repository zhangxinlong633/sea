#ifndef SEA_GRAPH_RECORD_H
#define SEA_GRAPH_RECORD_H

#include "../../include/public.h"

struct graph_record;

struct graph_node *graph_create_node(struct graph_record *record, uint32_t node_type);
uint32_t graph_remove_node(struct graph_record *record, struct graph_node *node);

uint32_t graph_create_node_property(struct graph_record *record, struct graph_node *node, char *key, uint32_t key_len, char *value, uint32_t value_len);
uint32_t graph_remove_node_property(struct graph_record *record, struct graph_node *node, char *key, uint32_t key_len);

struct graph_edge *graph_create_edge(struct graph_record *record, struct graph_node *src_node, struct graph_node *dst_node, uint32_t edge_type);
uint32_t graph_remove_edge(struct graph_record *record, struct graph_edge *edge);

uint32_t graph_create_edge_property(struct graph_record *record, struct graph_edge *edge, char *key, uint32_t key_len, char *value, uint32_t value_len);
uint32_t graph_remove_edge_property(struct graph_record *record, struct graph_edge *edge, char *key, uint32_t key_len);

#endif
