#include <kore/kore.h>
#include <kore/http.h>
#include <openssl/sha.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "assets.h"

struct sea_controller *controller = NULL;

int sea_http_server_load(int state)
{
	switch (state) {

		case KORE_MODULE_LOAD:
			sea_controller_init();
			controller = sea_controller_create();
			if (controller == NULL) {
				exit(-1);
			}

			kore_log(LOG_NOTICE, "module loading");
			http_server_version("Server/0.1");
			break;

		case KORE_MODULE_UNLOAD:
			sea_controller_destroy(controller);
			sea_controller_fini();
			kore_log(LOG_NOTICE, "module unloading");
			break;

		default:
			kore_log(LOG_NOTICE, "state %d unknown!", state);
			break;
	}

	return (KORE_RESULT_OK);
}

int sea_http_server_block_record_put(struct http_request *req)
{
	struct kore_buf		*b;
	char			buf[BUFSIZ];
	uint64_t block_id = 0;
	uint32_t record_id = 0;
	char ret_buf[1024]  = {0}; 

	b = kore_buf_alloc(asset_len_upload_html);

	kore_buf_append(b, asset_upload_html, asset_len_upload_html);

	int ret = sea_controller_write(controller, buf, asset_len_upload_html, &block_id, &record_id);
	if (ret != 0) {
		snprintf(ret_buf, 1024, "{\"ret\": -1, \"file_id\": -1}");
		goto exit;
	}

	snprintf(ret_buf, 1024, "{\"ret\": 0, \"fileid\": %16x_%8x}", block_id, record_id);

exit:

	http_response_header(req, "content-type", "text/html");
	http_response(req, 200, d, len);

	return (KORE_RESULT_OK);
}

int sea_http_server_block_record_get(struct http_request *req)
{
	uint64_t block_id = 0;
	uint32_t record_id = 0;
	int ret = EINVAL;
	char buf[BUFSIZ];
	int ret_buf_len = 0;

	ret = sea_controller_read(controller, block_id, record_id, buf, BUFSIZ, &ret_buf_len);
	if (ret != 0) {
		goto exit;
	}


exit:
	return (KORE_RESULT_OK);
}


