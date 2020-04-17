#include "../../include/public.h"
#include "../graph_v3.h"

uint64_t block_id = 1;

int insert_test()
{
    int ret = EINVAL;
    char *dir = "/Users/zhangxinlong/tmp/data";
    uint32_t max_size = 128 * 1024 * 1024;
    uint32_t max_count = 128 * 1024;
    sea_block_create(dir, block_id, max_size, max_count);
    struct sea_block *block = sea_block_open(dir, block_id);
    char buf[1024] = {0};
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

    uint32_t node_id = 0;

    for (int i = 0; i < 1000; i ++) {
        ret = create_node(block, &node, &attr_info, 1, &edge_info, 1, &node_id);
        if (ret != 0) {
            goto exit;
        }
    }

exit:

    sea_block_close(block);
	return ret;
}

int query_by_record_id_test()
{
    char *dir = "/Users/zhangxinlong/tmp/data";
    struct sea_block *block = sea_block_open(dir, block_id);

    uint32_t ret_buf_len = 0;

    for (int i = 0; i < 1000; i ++) {
        struct node_info *node = find_node(block, i, &ret_buf_len);
        if (node->edge_count != 1 || node->attr_count != 1) {
            goto exit;
        }
    }

exit:
    sea_block_close(block);
    sea_block_destroy(dir, block_id);
	return 0;
}

int main(int argc, char **argv)
{
    insert_test();
    query_by_record_id_test();
    //clone test ??
	return 0;
}

