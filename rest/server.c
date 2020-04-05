#include <stdio.h>
#include <stdlib.h>

#include <nng/nng.h>
#include <nng/supplemental/http/http.h>
#include <nng/supplemental/util/platform.h>

#include "../include/public.h"
#include "../controller/controller.h"

#define REST_URL "http://127.0.0.1:%u/api/sea"
struct sea_controller *controller = NULL;

// utility function
void fatal(const char *what, int rv)
{
	fprintf(stderr, "%s: %s\n", what, nng_strerror(rv));
	exit(1);
}

int sea_get(uint64_t block_id, uint32_t record_id, char *buf, int buf_len, int *ret_buf_len)
{
    return sea_controller_read(controller, block_id, record_id, buf, buf_len, ret_buf_len);
}


int sea_put(char *buf, int buf_len, uint64_t *block_id, uint32_t *record_id)
{
    return sea_controller_write(controller, buf, buf_len, block_id, record_id);
}

static void rest_handle_get(nng_aio *aio)
{
    nng_http_req *req = nng_aio_get_input(aio, 0);
    nng_http_res *res;
    int           rv;
    char        body[16]="Succ!";
    size_t        len = 5;
    int ret_buf_len = 0;
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

    uint64_t block_id = 101010;
    uint32_t record_id = 1;

    // todo parse block_id, record_id

    int ret = sea_get(block_id, record_id, buf, buf_len, &ret_buf_len);
    if (ret == 0) {
        nng_http_res_set_header(res, "Content-type", "text/plain");
        nng_http_res_copy_data(res, buf, ret_buf_len);
    }

    nng_aio_set_output(aio, 0, res);
    nng_aio_finish(aio, 0);
}

static void rest_handle_post(nng_aio *aio)
{
    nng_http_req *req = nng_aio_get_input(aio, 0);
    nng_http_res *res;
    int           rv;
    void *        body;
    size_t        len;

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

int main(int argc, char **argv)
{
	uint16_t    port = 0;
    nng_thread *inproc_thr;

	int rv;
    sea_controller_init();
    uint64_t block_id = 101010;
    controller = sea_controller_create(block_id);
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
