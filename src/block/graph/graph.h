#ifndef SEA_GRAPH_H
#define SEA_GRAPH_H

#include "../../include/public.h"

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

struct graph_db_conn;
int graph_db_create(char *dir, char *db_name);
int graph_db_destroy(char *dir, char *db_name);

struct graph_db_conn *graph_db_open(char *dir, char *db_name, int *error);
void graph_db_close(struct graph_db_conn *close);

int graph_db_insert(struct graph_db_conn *conn, struct node_info *node,
                struct graph_attr_info *attr_info, uint16_t attr_count,
                struct graph_edge_info *edge_info, uint16_t edge_count,
                uint32_t edge_free_size, uint32_t attr_free_size,
                uint64_t *node_block_id, uint32_t *node_record_id);

int graph_db_update(struct graph_db_conn *conn, uint64_t node_block_id, uint32_t node_record_id,
                    struct node_info *node,
                    struct graph_attr_info *attr_info, uint16_t attr_count,
                    struct graph_edge_info *edge_info, uint16_t edge_count);

int graph_db_remove(struct graph_db_conn *conn, uint64_t node_block_id, uint32_t node_record_id);

struct node_info *graph_db_find(struct graph_db_conn *conn, uint64_t node_block_id, uint32_t node_record_id);

#endif
