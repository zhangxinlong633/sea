#include <stdio.h>
#include <stdlib.h>

int get_ip_list(uint64_t block_id, uint32_t *ip_list, int ip_list_len, int *ret_ip_len)
{
	int ret = EINVAL;

	/*1. find phone_list */
	ret = phone_list_get(block_id, ip_list, ip_list_len, ret_ip_len);
	if (ret == 0 && ip[0] != 0) {
		goto exit;
	}

	/*3. find gossip */
	ret = gossip_list_get(block_id, ip_list, ip_list_len, ret_ip_len);
	if (ret == 0 && ip[0] != 0) {
		goto exit;
	}

exit:
	return ret;
}

int read(uint64_t block_id, uint32_t record_id, char *buf, int buf_len, int *ret_buf_len)
{
	int ret = EINVAL;
	uint32_t ip_list[16] = {0};
	int ret_ip_len = 0;

	ret = get_ip_list(block_id, ip_list, 16, &ret_ip_len);
	if (ret != 0 || ip_list[0] == 0) {
		goto exit;
	}

	uint8_t local_flag = pick_ip_local(ip_list, ip_list_len);
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
	}

exit:
	return ret;
}

struct alloc_block_entry {
	uint64_t block_id;
	uint64_t size;
	uint64_t count;
}

int write_local(char *buf, int buf_len, uint64_t *block_id, uint32_t *record_id)
{
	int ret = EINVAL;

	struct alloc_block_entry *block = pick_block();
	if (block == NULL) {
		ret = ENOENT;
		goto exit;
	}

	uint8_t overlimit = 0;
	ret = block_append(block_id, buf, buf_len, &record_id, &overlimit);
	if (ret == 0) {
		block->size += buf_len;
		block->count ++;
		if (overlimit) {
			pop_block_id(block_id);
			enqueue_block_id_in_replication_list(block_id);
		}
	}

exit:
	return ret;
}

int write(char *buf, int buf_len, uint64_t *block_id, uint32_t *record_id)
{
	int ret = EINVAL;

	ret = write_local(buf, buf_len, block_id, record_id);
	if (ret != 0) {
		goto exit;
	}

	int remote_count = 0;

	uint32_t rmote_ip_list[16] = {0};

	// 挑选符合条件的remote，条件标准, 1. 剩余空间越大，越靠前, 2. 响应时间越短越靠前.
	ret = get_remote(rmote_ip_list, 16, &ret_remote_count);
	for (int i = 0; i < ret_remote_count; i ++) {
		ret = write_remote(remote_ip_list[i], buf, buf_len, block_id, record_id);
		if (ret == 0) {
			break;
		}
	}

exit:
	return ret;
}

int init(void)
{

}


