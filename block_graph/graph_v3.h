#ifndef SEA_GRAPH_H
#define SEA_GRAPH_H

#include "../include/public.h"

/**
 * 方案3：做一个假设，所有的node信息都是Append,不存在修改信息,修改就是删除之后新增.那么这个简单的模型之下，所有的node都在一个record里
 *
 * 方案优点：
 *  1. 0(1)访问到node, edge, attr所有的信息
 *  2. 不用跨block 访问node, edge, attr, 特别是在分布式环境，跨block意味在不同网络。延时太高。
 *  3. 可以通过attr 快速找到node,有利于全文搜索
 *
 * 方案缺点:
 *  1. 不支持update
 *  2. 只支持出度查询，不支持入度
 *
 * 优化空间：
 *  1. 支持update
 *  2. 支持入度
 */

/**
 * 文件结构: graph_record: node_info + n * attr + m * (edge + x * attr)
 */

struct node_info {
    /**
     * 形成一个block 链, 这样就没有128MB限制了
     */
    uint32_t next_record_id;
    uint32_t prev_record_id;

    uint32_t edge_count;
    uint32_t attr_count;
    // 后面跟着edge 和 attr
}__attribute__ ((packed));

struct graph_attr_info {
    char attr_name[16];
    char attr_value[16];
} __attribute__ ((packed)) ;

struct graph_edge_info {
    uint8_t  direction; // 0: none, 1: src->dst, 2: dst->src
    uint32_t node_id;
}__attribute__ ((packed));

int create_node(struct sea_block *block, struct node_info *node,
        struct graph_attr_info *attr_info, uint16_t attr_count,
        struct graph_edge_info *edge_info, uint16_t edge_count,
        uint32_t *node_id);
struct node_info *find_node(struct sea_block *block, uint32_t node_id, uint32_t *ret_buf_len);

#endif
