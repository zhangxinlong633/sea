#include <stdio.h>
#include <stdlib.h>

#include <nng/nng.h>
#include <nng/supplemental/http/http.h>
#include <nng/supplemental/util/platform.h>

#include "../include/public.h"
#include "../include/protocol.h"
#include "../controller/controller.h"

#define REST_URL "http://127.0.0.1:%u/api/sea"
struct sea_controller *controller = NULL;
#define DATA_DIR "/Users/zhangxinlong/tmp/controller"

// utility function
void fatal(const char *what, int rv)
{
	fprintf(stderr, "%s: %s\n", what, nng_strerror(rv));
	exit(1);
}

int sea_get(uint64_t block_id, uint32_t record_id, char *buf, int buf_len, uint32_t *ret_buf_len)
{
    return sea_controller_read(controller, block_id, record_id, buf, buf_len, ret_buf_len);
}


int sea_put(char *buf, int buf_len, uint64_t *block_id, uint32_t *record_id)
{
    return sea_controller_write(controller, buf, buf_len, block_id, record_id);
}

// uri: /api/sea?id=1010101_1
int get_block_id_and_record_id(const char *uri, uint64_t *block_id, uint32_t *record_id) 
{
    int ret = EINVAL;

    char *id_str = NULL;
    id_str = strtok((char *)uri, "=");
        if (id_str == NULL) {
        goto exit;
    }

    id_str = strtok(NULL, "=");
    if (id_str == NULL) {
        goto exit;
    }

    char *block_str = strtok(id_str, "_");
    if (block_str == NULL) {
        goto exit;
    }

    char *record_id_str = strtok(NULL, "_");
    if (record_id_str == NULL) {
        goto exit;
    }
    ret = 0;
    *record_id = atol(record_id_str);
    *block_id = atol(block_str);

exit:
    return ret;
}
static void rest_handle_get(nng_aio *aio)
{
    nng_http_req *req = nng_aio_get_input(aio, 0);
    nng_http_res *res;
    int           rv;
    char        body[16]="Succ!";
    size_t        len = 5;
    uint32_t ret_buf_len = 0;
    char buf[1024*128] = {0};
    int buf_len = 1024 * 128;

    //nng_http_req_get_data(req, &body, &len);
    rv = nng_http_res_alloc(&res);
    if ((rv != 0) ||
        ((rv = nng_http_res_set_header(
                res, "Content-type", "text/plain")) != 0) ||
        ((rv = nng_http_res_set_status(res, NNG_HTTP_STATUS_OK)) != 0)) {
        nng_http_res_free(res);
        nng_aio_finish(aio, rv);
        return;
    }

    const char *uri = nng_http_req_get_uri(req);
    printf("uri: %s\n", uri);
    
    uint64_t block_id = 101010;
    uint32_t record_id = 1;

    rv = get_block_id_and_record_id(uri, &block_id, &record_id);
    if (rv != 0) {
        goto exit;
    }

    // todo parse block_id, record_id

    int ret = sea_get(block_id, record_id, buf, buf_len, &ret_buf_len);
    if (ret == 0) {
        nng_http_res_set_header(res, "Content-type", "image/png");
        nng_http_res_copy_data(res, buf, ret_buf_len);
    }

exit:
    nng_aio_set_output(aio, 0, res);
    nng_aio_finish(aio, 0);
}

static void rest_handle_post(nng_aio *aio)
{
    nng_http_req *req = nng_aio_get_input(aio, 0);
    nng_http_res *res;
    int           rv;
    void *body ;
    size_t        len = 0;

    nng_http_req_get_data(req, &body, &len);

    if (((rv = nng_http_res_alloc(&res)) != 0) ||
        ((rv = nng_http_res_copy_data(res, body, len)) != 0) ||
        ((rv = nng_http_res_set_header(
                res, "Content-type", "text/plain")) != 0) ||
        ((rv = nng_http_res_set_status(res, NNG_HTTP_STATUS_OK)) != 0)) {
        nng_http_res_free(res);
        nng_aio_finish(aio, rv);
        return;
    }

    char resBuf[1024] = "";
    uint64_t block_id = 0;
    uint32_t record_id = 0;
    int ret = sea_put(body, len, &block_id, &record_id);
    printf("body: %s\n", body);

    if (ret == 0) {
        int res_len = snprintf(resBuf, 1024, "{\"id\":\"%llu_%u\"}", block_id, record_id);
        nng_http_res_copy_data(res, resBuf, res_len);
    }

    printf("res is : : %s" , resBuf);

    nng_aio_set_output(aio, 0, res);
    nng_aio_finish(aio, 0);
}

void rest_start(uint16_t port)
{
	nng_http_server * server;
	nng_http_handler *handler_post;
    nng_http_handler *handler_get;
	char              rest_addr[128];
	nng_url *         url;
	int               rv;

	// Set up some strings, etc.  We use the port number
	// from the argument list.
	snprintf(rest_addr, sizeof(rest_addr), REST_URL, port);
	if ((rv = nng_url_parse(&url, rest_addr)) != 0) {
		fatal("nng_url_parse", rv);
	}

	// Get a suitable HTTP server instance.  This creates one
	// if it doesn't already exist.
	if ((rv = nng_http_server_hold(&server, url)) != 0) {
		fatal("nng_http_server_hold", rv);
	}

	// Allocate the handler - we use a dynamic handler for REST
	// using the function "rest_handle" declared above.
	rv = nng_http_handler_alloc(&handler_post, url->u_path, rest_handle_post);
	if (rv != 0) {
		fatal("nng_http_handler_alloc", rv);
	}

	if ((rv = nng_http_handler_set_method(handler_post, "POST")) != 0) {
		fatal("nng_http_handler_set_method", rv);
	}
	// We want to collect the body, and we (arbitrarily) limit this to
	// 128KB.  The default limit is 1MB.  You can explicitly collect
	// the data yourself with another HTTP read transaction by disabling
	// this, but that's a lot of work, especially if you want to handle
	// chunked transfers.
	if ((rv = nng_http_handler_collect_body(handler_post, true, 1024 * 128)) !=
	    0) {
		fatal("nng_http_handler_collect_body", rv);
	}
	if ((rv = nng_http_server_add_handler(server, handler_post)) != 0) {
		fatal("nng_http_handler_add_handler", rv);
	}
	//GET

    // Allocate the handler - we use a dynamic handler for REST
    // using the function "rest_handle" declared above.
    rv = nng_http_handler_alloc(&handler_get, url->u_path, rest_handle_get);
    if (rv != 0) {
        fatal("nng_http_handler_alloc", rv);
    }

    if ((rv = nng_http_handler_set_method(handler_get, "GET")) != 0) {
        fatal("nng_http_handler_set_method", rv);
    }
    if ((rv = nng_http_handler_collect_body(handler_get, true, 1024 * 128)) != 0) {
        fatal("nng_http_handler_collect_body", rv);
    }
    if ((rv = nng_http_server_add_handler(server, handler_get)) != 0) {
        fatal("nng_http_handler_add_handler", rv);
    }

	if ((rv = nng_http_server_start(server)) != 0) {
		fatal("nng_http_server_start", rv);
	}

	nng_url_free(url);
}

void inproc_server(void *arg) {
    while(1) {
        sleep(1);
    }
}

void rest_server(void) 
{
	uint16_t    port = 0;
    nng_thread *inproc_thr;

	int rv;
    sea_controller_init();
    controller = sea_controller_create(DATA_DIR);
    if (controller == NULL) {
        exit(-1);
    }

    rv = nng_thread_create(&inproc_thr, inproc_server, NULL);
    if (rv != 0) {
        fatal("cannot start inproc server", rv);
    }

    if (getenv("PORT") != NULL) {
		port = (uint16_t) atoi(getenv("PORT"));
	}
	port = port ? port : 8888;
	rest_start(port);
    nng_thread_destroy(inproc_thr);
}

int data_server_process(void *buf, int size, char *send_buf, uint32_t *send_buf_size) 
{
    int ret = EINVAL;
    struct data_server_hdr *send_hdr = (struct data_server_hdr *)send_buf;
    struct data_server_hdr *hdr = (struct data_server_hdr *)buf;
    if (hdr->len != size) {
        send_hdr->error = DATA_SERVER_ERROR_UNKNOWN;        
        goto exit;
    }

    send_hdr->method = hdr->method;
    send_hdr->error = DATA_SERVER_ERROR_NONE;

    uint32_t ret_buf_len = 0;
    if (hdr->method == DATA_SERVER_METHOD_GET) {
        send_hdr->block_id = hdr->block_id;
        send_hdr->record_id = hdr->record_id;

        ret = sea_controller_read(controller, hdr->block_id, hdr->record_id, send_hdr->data, (*send_buf_size - sizeof(struct data_server_hdr)), &ret_buf_len);
        if (ret == 0) {
            *send_buf_size = sizeof(struct data_server_hdr) + ret_buf_len;
        } else {
            send_hdr->error = DATA_SERVER_ERROR_GET;
        }
    } else if (hdr->method == DATA_SERVER_METHOD_PUT) {
        ret = sea_controller_write(controller, hdr->data, hdr->len, &send_hdr->block_id, &send_hdr->record_id);
        if (ret == 0) {
            *send_buf_size = sizeof(struct data_server_hdr);
        } else {
            send_hdr->error = DATA_SERVER_ERROR_PUT;
        }
    } else {
        send_hdr->error = DATA_SERVER_ERROR_UNKNOWN;
    }

exit:
    if (ret != 0) {
        *send_buf_size = sizeof(struct data_server_hdr);
    }

    return ret;
}

void data_server_handle(void *arg)
{
	nng_socket sock;
    int rv;
    const char *url = DATA_SERVER_URL;

    if ((rv = nng_rep0_open(&sock)) != 0) {
		fatal("nng_rep0_open", rv);
	}
	if ((rv = nng_listen(sock, url, NULL, 0)) != 0) {
		fatal("nng_listen", rv);
	}
    char *send_buf = malloc(1024 * 1024 * 10);
    uint32_t send_buf_size = 1024 * 1024 * 10;
	for (;;) {
		char *   buf = NULL;
		size_t   sz;
		uint64_t val;
        send_buf_size = 1024*1024 * 10;
		if ((rv = nng_recv(sock, &buf, &sz, NNG_FLAG_ALLOC)) != 0) {
			fatal("nng_recv", rv);
		}
        data_server_process(buf, sz, send_buf, &send_buf_size);
        if (send_buf_size != 0) {
			rv = nng_send(sock, buf, sz, NNG_FLAG_ALLOC);
			if (rv != 0) {
				fatal("nng_send", rv);
			}
        }
		nng_free(buf, sz);
	}
}

void data_server()
{
    nng_thread *inproc_thr;
    int rv = nng_thread_create(&inproc_thr, data_server_handle, NULL);
    if (rv != 0) {
        fatal("cannot start inproc server", rv);
    }
    return;
}

int main(int argc, char **argv)
{
    rest_server();
    data_server();
}
