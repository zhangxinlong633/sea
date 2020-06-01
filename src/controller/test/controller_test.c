#include "../controller.h"
#include "../../include/public.h"
#include "../../block/public.h"

void dump_data(void *data, int data_len)
{
	for (int i = 0; i < data_len; i ++) {
		printf ("%x",  ((char *)data)[i]);
		if (i % 8 == 0) {
			printf(" ");
		}
		if (i % 16 == 0) {
			printf("\n");
		}
	}
}

int main(int argc, char **argv)
{
	int ret = EINVAL;
	sea_controller_init();
	struct sea_controller *controller;

	controller = sea_controller_create(2);
	if (controller == NULL) {
		goto exit;
	}

	char buf[1024] = {0};
	#define COUNT 1024

	uint64_t block_id[COUNT] = {0}; 
	uint32_t record_id[COUNT] = {0};

	for (int i = 0; i < 100; i ++) {
		memset(buf, i, 1024);
		ret = sea_controller_write(controller, buf, 1024, &block_id[i], &record_id[i]);
		if (ret != 0) {
			goto exit;
		}
	}
	uint32_t ret_buf_len = 0;
	for (int i = 0; i < 100; i ++) {
		ret = sea_controller_read(controller, block_id[i], record_id[i], buf, 1024, &ret_buf_len);
		if (ret != 0) {
			goto exit;
		}
	}

exit:
	if (controller) {
		sea_controller_destroy(controller);
	}
	sea_controller_fini();
	return 0;
}

