#ifndef SEA_GRAPH_H
#define SEA_GRAPH_H

#include "../include/public.h"

/**
 * 方案优点：
 *  1. 0(1)访问到node, edge, property
 *  2. 不用跨block 访问node, edge, property, 特别是在分布式环境，跨block意味在不同网络。延时太高。
 * 方案缺点:
 *  1. 一个结点最多会浪费1024字节
 *  2. 后期加property超过1024字节，比较麻烦, 要么拆文件，要么跨网络
 *  3. 如果node id, 没有属性，也没有边，会很占空间，会产生sum(node id)个小文件
 */
struct node_info {
    /**
     * 创建的时候返回
     */
    uint64_t node_id;

    /**
     * 形成一个block 链, 这样就没有128MB限制了
     */
    uint64_t next_block_id;
    uint64_t prev_block_id;

    /**
     * 初始化从第十六个property之后开始是edge offset, 如果property用了16个了，就分32个property,以此类推.
     * 因为相比于边，属性变化更加小,
     * 另一个角底说，property是它自己的东西，而边是外面的，越向内，越接近node info
     */
    uint32_t edge_offset;

    /**
     * 初始化之后是否还要加属性，如果不加属性，edge会紧跟在属性之后，如果加的话，会有一定的空间留出来.
     */
    uint8_t add_property_flag;
    uint32_t edge_count;
    uint32_t property_count;
    struct graph_property_info property_info[0];
};

struct graph_property_info {
    char property_name[16];
    char property_value[16];
};

struct graph_edge_info {
    uint8_t  direction; // 0: none, 1: src->dst, 2: dst->src
    uint8_t  pad[3];
    uint64_t node_id;
};

/**
 * node 相当于block
 * @param node
 * @param block_id
 * @param record_id
 * @return
 */
int create_node(struct node_info *node);
int destroy_node(uint64_t node_id);
struct node_info *find_node(uint64_t node_id);

/**
 * edge 相当于block中的record, 这样是比较快速, property也是record,只不过从一个固定的地方开始
 * @param edge
 * @param edge_count
 * @return
 */
int create_edge(uint64_t src_node_id, struct graph_edge_info *info);
struct graph_edge_info *find_edge(uint_64_t src_node_id, uint32_t edge_id);
int destroy_edge(uint64_t src_node_id, uint32_t edge_id);

int create_property(uint64_t src_node_id, struct graph_property_info *info);
struct graph_property_info *find_property(uint_64_t src_node_id, uint32_t property_id);
int destroy_property(uint64_t src_node_id, uint32_t property_id);

#endif
