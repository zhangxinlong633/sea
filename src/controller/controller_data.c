#include "controller_data.h"
#include "../include/protocol.h"
#include <nng/nng.h>
#include <nng/supplemental/http/http.h>
#include <nng/supplemental/util/platform.h>

#define IPADDR_FMT  "%u.%u.%u.%u"
#define NIPQUAD(addr) \
        ((unsigned char *)&addr)[0], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[3]

int client(uint32_t ip, struct data_server_hdr *hdr, struct data_server_hdr *recv_hdr, uint32_t *recv_hdr_len)
{
	char url[256];

	nng_socket sock;
	int        rv;
	size_t     sz;
	uint8_t    cmd[sizeof(uint64_t)];

	memset(url, 0, 256);
	snprintf(url, 256, "tcp://"IPADDR_FMT":%u", NIPQUAD(ip), 8889);

	if ((rv = nng_req0_open(&sock)) != 0) {
		printf("nng_socket, rv: %d", rv);
	}
	if ((rv = nng_dial(sock, url, NULL, 0)) != 0) {
		printf("nng_dial, rv: %d", rv);
	}
	if ((rv = nng_send(sock, hdr, hdr->len, 0)) != 0) {
		printf("nng_send, rv: %d", rv);
	}
	if ((rv = nng_recv(sock, recv_hdr, recv_hdr_len, NNG_FLAG_ALLOC)) != 0) {
		printf("nng_recv, rv: %d", rv);
	}

	nng_close(sock);
	return (0);
}

static uint32_t min(uint32_t a, uint32_t b) 
{
	return a > b ? b : a;
}

int controller_data_get(uint32_t ip, uint64_t block_id, uint32_t record_id, void *buf, uint32_t buf_len, uint32_t *ret_buf_len)
{
	int ret = EINVAL;
	struct data_server_hdr *hdr = malloc(sizeof(struct data_server_hdr));
	if (hdr == NULL) {
		goto exit;
	}
	memset(hdr, 0, sizeof(struct data_server_hdr));

	hdr->method = DATA_SERVER_METHOD_GET;
	hdr->len = sizeof(struct data_server_hdr);
	hdr->block_id = block_id;
	hdr->record_id = record_id;

	ret = 0;

	int recv_hdr_len = 1024 * 1024 * 10;
	struct data_server_hdr *recv_hdr = malloc(recv_hdr_len);
	if (recv_hdr == NULL) {
		goto exit;
	}

	ret = client(ip, hdr, recv_hdr, recv_hdr_len);
	if (ret != 0 || recv_hdr->error != 0) {
		goto exit;
	}

	*ret_buf_len = min(recv_hdr->len - sizeof(struct data_server_hdr), buf_len);
	memcpy(buf, recv_hdr->data, *ret_buf_len);
	ret = 0;

exit:	
	if (recv_hdr != NULL) {
		free(recv_hdr);
	}
	if (hdr != NULL) {
		free(hdr);
	}

	return ret;
}

int controller_data_send(void *buf, uint32_t buf_len, uint32_t ip, uint64_t *block_id, uint32_t *record_id)
{
	int ret = EINVAL;
	struct data_server_hdr *hdr = malloc(buf_len + sizeof(struct data_server_hdr));
	if (hdr == NULL) {
		goto exit;
	}
	memset(hdr, 0, buf_len + sizeof(struct data_server_hdr));

	hdr->method = DATA_SERVER_METHOD_PUT;
	hdr->len = buf_len + sizeof(struct data_server_hdr);

	memcpy(hdr->data, buf, buf_len);

	ret = 0;

	struct data_server_hdr *recv_hdr = malloc(sizeof(struct data_server_hdr));
	if (recv_hdr == NULL) {
		goto exit;
	}

	ret = client(ip, hdr, recv_hdr, sizeof(struct data_server_hdr));
	if (ret != 0 || recv_hdr->error != 0) {
		goto exit;
	}

	*block_id = recv_hdr->block_id;	
	*record_id = recv_hdr->record_id;	
	ret = 0;

exit:	
	if (recv_hdr != NULL) {
		free(recv_hdr);
	}
	if (hdr != NULL) {
		free(hdr);
	}

	return ret;
}