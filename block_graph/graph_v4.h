#ifndef SEA_GRAPH_H
#define SEA_GRAPH_H

#include "../include/public.h"

/**
 * 方案4： 二级存储，
 * 一级存储node_id, block_id, record id
 * 二级存储 node,edge, attr
 * update的时候，如果block_id+record_id有空间就修改它，没有就创建一个新的block_id, record_id 把老数据copy过去,
 * 数据写进新的record, 然后,返回新的block_id, record_id, 更新一级存储.
 * 一条新的边会在两个node上都要增加
 *
 * 方案优点：
 *  1. 0(1)访问到node, edge, attr所有的信息
 *  2. 不用跨block 访问node, edge, attr, 特别是在分布式环境，跨block意味在不同网络。延时太高。
 *  3. 可以通过attr 快速找到node,有利于全文搜索
 *  4. 支持出度,入度查询
 *  5. 支持更新
 *  6. 预留空间支持动态规则，开始可以是0，之后是128字节，256，512，1024等也可以变小,减少需要移动node的情况.
 *  7. 支持配置一个节点最大大小。
 *
 * 方案缺点:
 *  1. update copy数据的过程可能比较暴力，太大了，特别是一个node拥有128MB数据的时候.
 *  2. node 拥有上百万的边的时候，性能很差
 *
 * 缺点解决方案:
 *  1. 大型节点预留更多的空间,按一个算法算出一个值，尽量减少大型节点的移动.
 *  2. node 边太多的时候，边不考虑顺序存储，而是二分, 边少的时候无所谓，有个limit值.
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
