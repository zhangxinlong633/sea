#ifndef __CONTROLLER_DATA_H
#define __CONTROLLER_DATA_H

#include "../include/public.h"

int controller_data_get(uint32_t ip, uint64_t block_id, uint32_t record_id, void *buf, uint32_t buf_len, uint32_t *ret_buf_len);
int controller_data_send(void *buf, uint32_t buf_len, uint32_t ip, uint64_t *block_id, uint32_t *record_id); 

#endif
