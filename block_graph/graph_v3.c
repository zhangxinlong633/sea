#include "graph_v3.h"

int create_node(struct sea_block *block, struct node_info *node,
                struct graph_attr_info *attr_info, uint16_t attr_count,
                struct graph_edge_info *edge_info, uint16_t edge_count,
                uint32_t *node_id)
{
    int ret = EINVAL;
    int node_size = sizeof(struct node_info);
    uint32_t edge_size = edge_count * sizeof(struct graph_edge_info);
    uint32_t attr_size = attr_count * sizeof(struct graph_attr_info);
    uint32_t buf_size = node_size + edge_size + attr_size;

    char *buf = malloc(buf_size);
    if (buf == NULL) {
        ret = ENOMEM;
        goto exit;
    }

    char *p = buf;

    // node data
    node->attr_count = attr_count;
    node->edge_count = edge_count;
    memcpy(p, (char *)node, node_size);

    // edge data
    p += node_size;
    memcpy(p, (char *)edge_info, edge_size);

    // attr data
    p += edge_size;
    memcpy(p, (char *)attr_info, attr_size);

    // write to db
    uint32_t record_id = 0;
    int over_limit = 0;
    ret = sea_block_append(block, buf, buf_size, &record_id, &over_limit);
    if (ret != 0) {
        goto exit;
    }

    *node_id = record_id;

    ret = 0;

exit:
    return ret;
}

struct node_info *find_node(struct sea_block *block, uint32_t node_id, uint32_t *ret_buf_len)
{
    int ret = EINVAL;
    struct node_info *info = malloc(1024*1024);
    if (info == NULL) {
        goto exit;
    }

    ret = sea_block_query_by_record_id(block, node_id, (char *)info, 1024*1024, ret_buf_len);
    if (ret != 0) {
        goto exit;
    }

    ret = 0;

exit:
    if (ret != 0 && info != NULL) {
        free(info);
        info = NULL;
    }

    return info;
}
