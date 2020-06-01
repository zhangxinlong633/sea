#ifndef __CONTROLLER_H_
#define __CONTROLLER_H_

#include "../include/public.h"
#include "../block/time/sea_block.h"
struct sea_controller;

struct sea_controller *sea_controller_create(char *dir);
void sea_controller_destroy(struct sea_controller *controller);

int sea_controller_read(struct sea_controller *controller, uint64_t block_id, uint32_t record_id, char *buf, uint32_t buf_len, uint32_t *ret_buf_len);
int sea_controller_write(struct sea_controller *controller, char *buf, int buf_len, uint64_t *block_id, uint32_t *record_id);

int sea_controller_init(void);
void sea_controller_fini(void);

#endif

