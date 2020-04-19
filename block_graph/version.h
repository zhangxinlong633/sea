#ifndef SEA_VERSION_H
#define SEA_VERSION_H

/**
 * 方案4： 二级存储，
 * 一级存储node_id, block_id, record id
 * 二级存储 node,edge, attr
 * update的时候，如果block_id+record_id有空间就修改它，没有就创建一个新的block_id, record_id 把老数据copy过去,
 * 数据写进新的record, 然后,返回新的block_id, record_id, 更新一级存储.
 * 一条新的边会在两个node上都要增加
 *
 * 方案优点：
 *  1. 0(1)访问到node, edge, attr所有的信息 -> ok
 *  2. 不用跨block 访问node, edge, attr, 特别是在分布式环境，跨block意味在不同网络。延时太高。 -> ok
 *  3. 可以通过attr 快速找到node,有利于全文搜索
 *  4. 支持出度,入度查询
 *  5. 支持更新 -> ok
 *  6. 预留空间支持动态规则，开始可以是0，之后是128字节，256，512，1024等也可以变小,减少需要移动node的情况. -> ok
 *  7. 支持配置一个节点最大大小。
 *
 * 方案缺点:
 *  1. update copy数据的过程可能比较暴力，太大了，特别是一个node拥有128MB数据的时候.
 *  2. node 拥有上百万的边的时候，查找边, 性能很差
 *
 * 缺点解决方案:
 *  1. 大型节点预留更多的空间,按一个算法算出一个值，尽量减少大型节点的移动. -> ok
 *  2. node 边太多的时候，边不考虑顺序存储，而是二分, 边少的时候无所谓，有个limit值.
 */


#endif //SEA_VERSION_H
