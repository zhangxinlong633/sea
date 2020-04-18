#include "../include/public.h"
#include "graph.h"
#include "../block_word/md5.h"
#include "graph_data.h"

struct graph_db_conn {
    char dir[32];
    uint64_t index_block_id;
    struct sea_block *index_block;
    struct sea_block *data_block;
};

uint64_t graph_db_get_block_id(char *db_name)
{
    char output[256] = {0};

    md5((unsigned char *)db_name, output, 256);

    return (uint64_t)output;
}
int graph_db_create(char *dir, char *db_name)
{
    uint32_t max_size = 128 * 1024 * 1024;
    uint32_t max_count = 128 * 1024;

    uint64_t block_id = graph_db_get_block_id(db_name);
    return sea_block_create(dir, block_id, max_size, max_count);
}

int graph_db_destroy(char *dir, char *db_name)
{
    return sea_block_data_destroy(dir, graph_db_get_block_id(db_name));
}

struct graph_db_conn *graph_db_open(char *dir, char *db_name, int *error)
{
    struct graph_db_conn *conn = malloc(sizeof(struct graph_db_conn));
    if (conn == NULL) {
        goto exit;
    }

    uint64_t block_id = graph_db_get_block_id(db_name);

    struct sea_block *block = sea_block_open(dir, block_id);
    conn->index_block = block;
    conn->data_block = NULL;
    conn->index_block_id = block_id;
    memcpy(conn->dir, dir, strlen(dir));

exit:
    return conn;
}

void graph_db_close(struct graph_db_conn *conn)
{
    if (conn->index_block) {
        sea_block_close(conn->index_block);
    }
    if (conn->data_block) {
        sea_block_close(conn->data_block);
    }
}

struct graph_db_index_node_data {
    uint64_t block_id;
    uint32_t record_id;
    uint32_t pad;
};


static int __graph_db_insert_internal(struct graph_db_conn *conn, struct node_info *node,
                            struct graph_attr_info *attr_info, uint16_t attr_count,
                            struct graph_edge_info *edge_info, uint16_t edge_count,
                            uint32_t edge_free_size, uint32_t attr_free_size,
                            uint64_t *node_block_id, uint32_t *node_record_id,
                            uint8_t exist_node_flag)

{
    int ret = EINVAL;
    uint32_t max_size = 128 * 1024 * 1024;
    uint32_t max_count = 128 * 1024;
    uint64_t data_block_id = 2; // todo 随机生成

    if (exist_node_flag) {
        data_block_id = *node_block_id;
    }

    // todo 使用最新的block id
    if (conn->data_block == NULL) {
        sea_block_create(conn->dir, data_block_id, max_size, max_count);
        conn->data_block = sea_block_open(conn->dir, data_block_id);
    }
    if (conn->data_block == NULL) {
        goto exit;
    }

    // 先把node data 写进去
    uint32_t data_record_id = 0;
    ret = create_node_data(conn->data_block, node, attr_info, attr_count,edge_info, edge_count, &data_record_id, edge_free_size, attr_free_size);
    if (ret != 0) {
        goto exit;
    }

    // 再把index更新
    struct graph_db_index_node_data index_data;
    if (!exist_node_flag) {
        index_data.block_id = data_block_id;
        index_data.record_id = data_record_id;
        index_data.pad = 0;

        uint32_t index_record_id = 0;
        int overlimit = 0;
        ret = sea_block_append(conn->index_block, (char *) &index_data, sizeof(struct graph_db_index_node_data),
                               &index_record_id, &overlimit);
        if (ret != 0) {
            goto exit;
        }

        *node_block_id = conn->index_block_id;
        *node_record_id = index_record_id;
    } else {
        index_data.block_id = *node_block_id;
        index_data.record_id = *node_record_id;
        index_data.pad = 0;

        ret = sea_block_update(conn->index_block, *node_record_id, (char *) &index_data, sizeof(struct graph_db_index_node_data));
        if (ret != 0) {
            goto exit;
        }
    }
    ret = 0;

exit:
    return ret;
}

int graph_db_insert(struct graph_db_conn *conn, struct node_info *node,
                    struct graph_attr_info *attr_info, uint16_t attr_count,
                    struct graph_edge_info *edge_info, uint16_t edge_count,
                    uint32_t edge_free_size, uint32_t attr_free_size,
                    uint64_t *node_block_id, uint32_t *node_record_id)
{
    return __graph_db_insert_internal(conn, node, attr_info, attr_count, edge_info, edge_count,
                                      edge_free_size, attr_free_size, node_block_id, node_record_id, false);
}

/**
 * 检查是否可以update，可以的话就直接udpate,不可以的话就要remove再append一个新的
 * @param node
 * @param attr_count
 * @param edge_count
 * @return
 */
int graph_db_check_update(struct node_info *node, uint16_t attr_count, uint16_t edge_count)
{
    return false;
}

int graph_db_update(struct graph_db_conn *conn, uint64_t node_block_id, uint32_t node_record_id,
                    struct node_info *node,
                    struct graph_attr_info *attr_info, uint16_t attr_count,
                    struct graph_edge_info *edge_info, uint16_t edge_count)
{
    int ret = EINVAL;
    struct node_info *node_info = graph_db_find(conn, node_block_id, node_record_id);

    if (graph_db_check_update(node_info, attr_count, edge_count)) {
        // todo update data block
        ret = 0;
        goto exit;
    }

    ret = graph_db_remove(conn, node_block_id, node_record_id);
    if (ret != 0) {
        goto exit;
    }

    uint32_t attr_free_size = node->attr_count * sizeof(struct graph_attr_info) / 2;
    uint32_t edge_free_size = node->edge_count * sizeof(struct graph_edge_info) / 2;

    ret = __graph_db_insert_internal(conn, node, attr_info, attr_count, edge_info, edge_count,
                                      edge_free_size, attr_free_size, &node_block_id, &node_record_id, true);
    if (ret != 0) {
        goto exit;
    }

    ret = 0;

exit:
    return ret;
}

int graph_db_remove(struct graph_db_conn *conn, uint64_t node_block_id, uint32_t node_record_id)
{
    // todo
    //  先读取index block ,node,
    //  然后再根据里面的blcok_id, record id,
    //  删除data, block ,
    //  删除 index block
    return 0;
}

struct node_info *graph_db_find(struct graph_db_conn *conn, uint64_t node_block_id, uint32_t node_record_id)
{
    struct node_info *node = NULL;
    struct graph_db_index_node_data node_data;
    struct sea_block *block = NULL;

    // 查询index node
    uint32_t ret_buf_len = 0;
    int ret = sea_block_query_by_record_id(conn->index_block, node_record_id, &node_data, sizeof(struct graph_db_index_node_data), &ret_buf_len);
    if (ret != 0) {
        goto exit;
    }

    // 查询 data node
    block = sea_block_open(conn->dir, node_data.block_id);
    if (block == NULL) {
        goto exit;
    }

    node = malloc(1024 * 1024);
    if (node == NULL) {
        goto exit;
    }

    ret = sea_block_query_by_record_id(block, node_data.record_id, node, 1024 * 1024, &ret_buf_len);
    if (ret != 0) {
        goto exit;
    }

    ret = 0;

exit:
    if (ret != 0) {
        if (block != NULL) {
            sea_block_close(block);
        }
        if (node != NULL) {
            free(node);
            node = NULL;
        }
    }
    return node;
}

