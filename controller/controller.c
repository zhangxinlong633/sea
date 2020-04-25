#include <stdlib.h>
#include "phone_list.h"
#include "../include/public.h"

#define SEA_CONTROLLER_LOCAL_NODE 0
#define DATA_DIR "/Users/zhangxinlong/tmp/controller"

struct sea_controller {
	uint64_t block_id;
	struct sea_block *block;
	struct phone_list *phone_list;
};

struct sea_controller *sea_controller_create(uint64_t block_id)
{
	struct sea_controller *controller = malloc(sizeof(struct sea_controller));
	if (controller == NULL) {
		goto exit;
	}

	if (block_id == 0) {
        controller->block_id = time(NULL);
    } else {
        controller->block_id = block_id;
	}

	controller->block = NULL;
	controller->phone_list = phone_list_create(3);

exit:
	return controller;
}

void sea_controller_destroy(struct sea_controller *controller)
{
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

	// todo: 本地没有block读远程的blockid
	//dht_find_node_data(controller, block_id, record_id, buf, buf_len, ret_buf_len);

exit:
	return ret;
}

int sea_controller_write_local(struct sea_controller *controller, char *buf, int buf_len, uint64_t *block_id, uint32_t *record_id)
{
	int ret = EINVAL;

	*block_id = controller->block_id;

	if (controller->block == NULL) {
		controller->block = sea_block_open(DATA_DIR, controller->block_id);
		if (controller->block == NULL) {
			sea_block_create(DATA_DIR, controller->block_id, SEA_BLOCK_MAX_SIZE_DEFAULT, SEA_BLOCK_MAX_COUNT_DEFAULT);
		}

		controller->block = sea_block_open(DATA_DIR, controller->block_id);
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
	phone_list_put(controller->phone_list, controller->block_id, &node, 1);

	if (overlimit) {
		controller->block_id++;
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

    //dht_forward_data(controller, buf, buf_len, block_id, record_id);

//exit:
	return ret;
}

int sea_controller_init(void)
{
    //todo 建立dht nng socket
    // 初始化dht
    //dht_list_init();
	return 0;
}

void sea_controller_fini(void)
{
	return;
}

