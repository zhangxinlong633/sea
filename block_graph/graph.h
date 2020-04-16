#ifndef SEA_GRAPH_H
#define SEA_GRAPH_H

#include "../include/public.h"

struct node_info {

};

struct edge_info {
    int edge_len;
    uint64_t next_node;
    char data[256];
};

/**
 * 创建node block 的时候，默认会创建一个边block, 两个连续一起创建，分配一个node, edge block对应的位置分配一个edge reocrd.
 * 这个edge record做为 node的起始边.
 * tips: edge是由node基于一定的规则生成的
 * @param node
 * @param block_id
 * @param record_id
 * @return
 */
int create_node(struct node_info *node, uint64_t *block_id, uint32_t *record_id);
int destroy_node(uint64_t block_id, uint32_t record_id);
struct node_info *find_node(uint64_t block_id, uint32_t record_id);
//int update_node(uint64_t block_id, uint32_t record_id, struct node_info *node);

/**
 *
 * 边block的生成，每个src_node 有一个唯一的边record,  edge_src_node_id，边是成对增加的，如果A->B,这样的一个边，会加成两个边A节点的A->B,B结点同样要加个.
 * @param edge
 * @param edge_count
 * @return
 */
int get_edge_record_id(uint64_t src_node_id);
int append_edge(uint64_t src_node_id, struct edge_info *edge, int edge_count);
int destroy_edge(uint64_t src_node_id, struct edge_info *edge);

#endif
