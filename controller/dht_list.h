#ifndef SEA_DHT_LIST_H
#define SEA_DHT_LIST_H

#include "../include/public.h"

/**
 * 需求：
 * 	将block数据发送到远程节点，或者从远程节点读出来
 *  todo
 * 	1. 通过,block_id, record_id 获取数据
 *  2. 将数据存储起来，返回block_id, record_id
 *  3. 将block 进行raft 备份
 */ 

int dht_list_get_data(uint64_t block_id, uint32_t record_id, char *buf, int *buf_len, uint32_t *ret_buf_len);
int dht_list_put_data(char *buf, int buf_len, uint64_t *block_id, uint32_t *record_id);
int dht_list_raft_block(uint64_t block_id, uint8_t count);

int dht_list_init(int port, int node_list_len, char **node_list);
void dht_list_fini();

#endif //SEA_DHT_LIST_H
