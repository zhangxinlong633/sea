#include "../../../block_graph/graph.h"

char dir[256] = "/Users/zhangxinlong/tmp/data";
char db_name[256] = "graph";

#define COUNT 10000 * 10000

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

    uint64_t node_block_id;
    uint32_t node_record_id;
    uint32_t start = time(NULL);
    for (int i = 0; i < COUNT; i ++) {
        ret = graph_db_insert(graph, &node, &attr_info, 1, &edge_info, 1, 1024, 1024,
                              &node_block_id, &node_record_id);
        if (ret != 0) {
            goto exit;
        }
        if (i % 10000 == 0) {
            uint32_t stop = time(NULL);
            printf("graph test count: %d, from %u to %u , speed: %u\n", i, start, stop, i / ((stop - start) > 0 ? (stop - start): 1) );
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

exit:
    graph_db_destroy(dir, db_name);
    return ret;
}

