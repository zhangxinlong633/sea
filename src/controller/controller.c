#include <stdlib.h>
#include "phone_list.h"
#include "../include/public.h"
#include "super_block.h"
#include "controller.h"

#define SEA_CONTROLLER_LOCAL_NODE 0
#define DATA_DIR "/Users/zhangxinlong/tmp/controller"

struct sea_controller {
	struct controller_super_block *super_block;
	struct sea_block *block;
	struct phone_list *phone_list;
};

struct sea_controller *sea_controller_create(char *dir)
{
	int ret = EINVAL;
	struct sea_controller *controller = malloc(sizeof(struct sea_controller));
	if (controller == NULL) {
		goto exit;
	}

	controller->block = NULL;
	controller->phone_list = phone_list_create(3);

	controller->super_block = controller_super_block_open_or_create(dir, "controller", &ret);
	if (controller->super_block == NULL) {
		goto exit;
	}
	ret = 0;

exit:
	if (ret != 0) {
		free(controller);
		controller = NULL;
	}
	return controller;
}

void sea_controller_destroy(struct sea_controller *controller)
{
	controller_super_block_close(controller->super_block);
	free(controller);
}

int sea_controller_read_from_local(uint64_t block_id, uint32_t record_id, char *buf, int buf_len, uint32_t *ret_buf_len)
{
	int ret = EINVAL;
	struct sea_block *block = sea_block_open(DATA_DIR, block_id);
	if (block == NULL) {
		goto exit;
	}

	ret = sea_block_query_by_record_id(block, record_id, buf, buf_len, ret_buf_len);
	if (ret == 0 && ret_buf_len != 0) {
		goto exit;	
	}

exit:

	if (block != NULL) {
		sea_block_close(block);
	}
	return ret;
}

//策略：读的时候，优先找自己，找到就读自己的数据，找不到就寻找节点
int sea_controller_read(struct sea_controller *controller, uint64_t block_id, uint32_t record_id, char *buf, uint32_t buf_len, uint32_t *ret_buf_len)
{
	int ret = EINVAL;
	uint32_t ip_list[16] = {0};
	uint32_t ret_ip_len = 0;

    /*ret = phone_list_get(controller->phone_list, block_id, ip_list, 16, &ret_ip_len);
	if (ret != 0 || ip_list[0] == 0) {
		goto exit;
	} */

	ret = sea_controller_read_from_local(block_id, record_id, buf, buf_len, ret_buf_len);
	if (ret != 0) {
		uint32_t ip = 0;
		ret = dht_list_get_node(block_id, record_id, &ip);
		if (ret != 0) {
			goto exit;
		}

		ret = controller_data_get(ip, block_id, record_id, buf, buf_len, ret_buf_len);
	}


exit:
	return ret;
}

int sea_controller_write_local(struct sea_controller *controller, char *buf, int buf_len, uint64_t *block_id, uint32_t *record_id)
{
	int ret = EINVAL;

	ret = controller_super_block_get_block_id(controller->super_block, block_id);
	if (ret != 0) {
		goto exit;
	}

	if (controller->block == NULL) {
		controller->block = sea_block_open(DATA_DIR, *block_id);
		if (controller->block == NULL) {
			sea_block_create(DATA_DIR, *block_id, SEA_BLOCK_MAX_SIZE_DEFAULT, SEA_BLOCK_MAX_COUNT_DEFAULT);
		}

		controller->block = sea_block_open(DATA_DIR, *block_id);
	}
	if (controller->block == NULL) {
		ret = ENOENT;
		goto exit;
	}

	int overlimit = 0;
	ret = sea_block_append(controller->block, buf, buf_len, record_id, &overlimit);
	if (ret != 0) {
		goto exit;
	}

	uint32_t node = SEA_CONTROLLER_LOCAL_NODE;
	phone_list_put(controller->phone_list, *block_id, &node, 1);

	if (overlimit) {
		controller_super_block_expire_block_id(controller->super_block, *block_id);
		sea_block_close(controller->block);
		controller->block = NULL;
	}

exit:
	return ret;
}

int load_local_blocks(void)
{
   // 找到所有block,将他们插入到phone_list.
   // 将没有插入满的几个block单独返回，供后续插入使用.
   return 0;
}

//策略：写的时候优先写自己，然后找响应最快的节点
int sea_controller_write(struct sea_controller *controller, char *buf, int buf_len, uint64_t *block_id, uint32_t *record_id)
{
	int ret = EINVAL;

    //if ((has_free_block_id || block_id = generate_local_block_id()) && space > LIMIT) {
        ret = sea_controller_write_local(controller, buf, buf_len, block_id, record_id);
     //   goto exit;
    //}
	uint32_t ip;

	ret = dht_list_get_free_node(&ip);
	if (ret != 0) {
		goto exit;
	}

	ret = controller_data_send(buf, buf_len, ip, block_id, record_id);

exit:
	return ret;
}

int sea_controller_init(void)
{
	int port = 8888;
	int node_list_len = 0;
	char node_list[12][20] = {{"192.168.0.11"}};
	dht_list_init(port, node_list_len, node_list);
	return 0;
}

void sea_controller_fini(void)
{
	dht_list_fini();
}

