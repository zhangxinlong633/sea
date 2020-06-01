#include "public.h"

#define DATA_SERVER_URL "tcp://0.0.0.0:8889"
#define DATA_SERVER_PORT 8889

#define DATA_SERVER_METHOD_GET 0x1
#define DATA_SERVER_METHOD_PUT 0x2

#define DATA_SERVER_ERROR_NONE 0x0
#define DATA_SERVER_ERROR_GET 0x1
#define DATA_SERVER_ERROR_PUT 0x2
#define DATA_SERVER_ERROR_UNKNOWN 0x3
struct data_server_hdr {
    uint8_t  method;
    uint16_t error;
    uint32_t len;
    uint64_t  block_id;
    uint64_t  record_id;  
    char data[0]; 
};