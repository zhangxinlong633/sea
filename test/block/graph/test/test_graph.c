
#include <graph_data.h>
#include "../../include/public.h"
#include "../include/"

char dir[256] = "/Users/zhangxinlong/tmp/data";
char db_name[256] = "graph";
uint64_t node_block_ids[1000] = {0};
uint32_t node_record_ids[1000] = {0};

int insert_test()
{
    int ret = EINVAL;
    struct node_info node;

    memset(&node, 0, sizeof(struct node_info));

    struct graph_attr_info attr_info;
    memset(&attr_info, 0, sizeof(struct graph_attr_info));
    snprintf(attr_info.attr_name, 16, "name");
    snprintf(attr_info.attr_value, 16, "hello");

    struct graph_edge_info edge_info;
    memset(&edge_info, 0, sizeof(struct graph_edge_info));
    edge_info.direction = 0;
    edge_info.node_id = 0;

    struct graph_db_conn *graph = graph_db_open(dir, db_name, &ret);
    if (graph == NULL || ret != 0) {
        goto exit;
    }

    for (int i = 0; i < 1000; i ++) {
        ret = graph_db_insert(graph, &node, &attr_info, 1, &edge_info, 1, 1024, 1024,
                &node_block_ids[i], &node_record_ids[i]);
        if (ret != 0) {
            goto exit;
        }
    }

    ret = 0;

exit:
    if (graph != NULL) {
        graph_db_close(graph);
    }

    return ret;
}

int query_by_record_id_test()
{
    int ret = EINVAL;
    struct graph_db_conn *graph = graph_db_open(dir, db_name, &ret);
    if (graph == NULL || ret != 0) {
        goto exit;
    }

    for (int i = 0; i < 1000; i ++) {
        struct node_info *node = graph_db_find(graph, node_block_ids[i], i);
        if (node == NULL) {
            goto exit;
        }
        if (node->edge_count != 1 || node->attr_count != 1) {
            goto exit;
        }
    }

    ret = 0;

exit:

    if (graph != NULL) {
        graph_db_close(graph);
    }

    return ret;
}

int main(int argc, char **argv)
{
    int ret = EINVAL;
    graph_db_create(dir, db_name);
    memset(dir, 0,256);
    memset(db_name, 0, 256);

    snprintf(dir, 256, "/Users/zhangxinlong/tmp/data");
    snprintf(db_name, 256, "graph");

    ret = insert_test();
    if (ret != 0) {
        goto exit;
    }

    ret = query_by_record_id_test();
    if (ret != 0) {
        goto exit;
    }

exit:
    graph_db_destroy(dir, db_name);
    return ret;
}

