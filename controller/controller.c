#include <stdio.h>
#include <stdlib.h>
#include "phone_list.h"
#include "../include/public.h"

#define SEA_CONTROLLER_LOCAL_NODE 0
#define DATA_DIR "/tmp/controller"

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

static int get_ip_list(struct sea_controller *controller, uint64_t block_id, uint32_t *ip_list, int ip_list_len, uint32_t *ret_ip_len)
{
	int ret = EINVAL;

	/*1. find phone_list */
	ret = phone_list_get(controller->phone_list, block_id, ip_list, ip_list_len, ret_ip_len);
	if (ret == 0 && ip_list[0] != 0) {
		goto exit;
	}

	/*3. find gossip */
	/*ret = gossip_list_get(block_id, ip_list, ip_list_len, ret_ip_len);
	if (ret == 0 && ip[0] != 0) {
		goto exit;
	}*/

exit:
	return ret;
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

int sea_controller_read(struct sea_controller *controller, uint64_t block_id, uint32_t record_id, char *buf, uint32_t buf_len, uint32_t *ret_buf_len)
{
	int ret = EINVAL;
	uint32_t ip_list[16] = {0};
	uint32_t ret_ip_len = 0;

	/*ret = get_ip_list(controller, block_id, ip_list, 16, &ret_ip_len);
	if (ret != 0 || ip_list[0] == 0) {
		goto exit;
	} */

	ret = sea_controller_read_from_local(block_id, record_id, buf, buf_len, ret_buf_len);

	/*uint8_t local_flag = pick_ip_local(ip_list, ip_list_len);
	if (local_flag) {
		// 本地ip, 直接读数据返回，不需要再次建立链接	
		ret = read_from_local(block_id, record_id, buf, buf_len, ret_buf_len);
		if (ret == 0 && ret_buf_len != 0) {
			goto exit;	
		}
	}

	// 到远程主机get data
	for (int i = 0; i < ret_ip_len; i ++) {
		// 请求 ip
		ret = remote_ip_get_record(ip_list[i], block_id, record_id, buf, buf_len, ret_buf_len);
		if (ret == 0 || ret_buf_len != 0) {
			goto exit;	
		}
	} */

exit:
	return ret;
}

int sea_controller_write_local(struct sea_controller *controller, char *buf, int buf_len, uint64_t *block_id, uint32_t *record_id)
{
	int ret = EINVAL;

	*block_id = controller->block_id;

	if (controller->block == NULL) {
		sea_block_create(DATA_DIR, controller->block_id, SEA_BLOCK_MAX_SIZE_DEFAULT, SEA_BLOCK_MAX_COUNT_DEFAULT);
		controller->block = sea_block_open(DATA_DIR, controller->block_id);
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

int sea_controller_write(struct sea_controller *controller, char *buf, int buf_len, uint64_t *block_id, uint32_t *record_id)
{
	int ret = EINVAL;

	ret = sea_controller_write_local(controller, buf, buf_len, block_id, record_id);
	if (ret != 0) {
		goto exit;
	}

	/*int remote_count = 0;

	uint32_t rmote_ip_list[16] = {0};

	// 挑选符合条件的remote，条件标准, 1. 剩余空间越大，越靠前, 2. 响应时间越短越靠前.
	ret = get_remote(rmote_ip_list, 16, &ret_remote_count);
	for (int i = 0; i < ret_remote_count; i ++) {
		ret = write_remote(remote_ip_list[i], buf, buf_len, block_id, record_id);
		if (ret == 0) {
			break;
		}
	} */

exit:
	return ret;
}

int sea_controller_init(void)
{
	return 0;
}

void sea_controller_fini(void)
{
	return;
}

