#ifndef SEA_GRAPH_H
#define SEA_GRAPH_H

#include "../include/public.h"

/**
 * 方案2：一个block_record_id  存储node部分信息，并且包含它部分的属性和边, 应用记住第一个block_record_id做为node_id,
 * 它可以关联多个block_record来存储
 * 方案优点：
 *  1. 0(N)访问到node, edge, property, N 是存储次数
 *  3. 可以通过property 快速找到node,有利于全文搜索
 * 方案缺点:
 *  1. 点，边加属性比较麻烦
 *  2. 点加边，需要lseek到新的磁盘record地址
 *  3. 新加的边和属性有可能会跨越block, 会从磁盘io变成网络io.
 *  4.
 *
 */

struct graph_record_hdr {
    /**
     * 1: node
     * 2: property
     * 3: edge
     */

    uint8_t type;

};

struct graph_record {
    struct graph_record_hdr hdr;
    struct node_info node;
    char data[0];
    /**
     * node后面紧跟property, edge 相关信息
     */
    /* struct graph_edge_info edge_info[4];  */
};

struct node_info {
    uint8_t  inuse;
    /**
     * 创建的时候返回
     */
    uint64_t node_id;

    /**
     * 形成一个block 链, 这样就没有128MB限制了
     */
    uint64_t next_record_id;
    uint64_t prev_record_id;

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
    uint8_t  inuse;
    /**
     * property name和value是Tlv格式，支持变长
     */
    /*char property_name[16];
    char property_value[16]; */
};

struct graph_edge_info {
    uint8_t  inuse;
    uint8_t  direction; // 0: none, 1: src->dst, 2: dst->src
    uint64_t node_id;
    struct graph_property_info info;
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
