#ifndef __SEA_REST_H
#define __SEA_REST_H

#define SEA_API_PREFIX ""

#define SEA_REST_METHOD_GET      1
#define SEA_REST_METHOD_POST     2
#define SEA_REST_METHOD_PUT      3
#define SEA_REST_METHOD_DELETE   4

#define SEA_REST_MAX_BODY_SIZE   1024 * 1024 * 10 /* 10MB */
#define SEA_REST_URI_LEN         256

/* 
    使用 ulfius库, 可以参考下面这个例子来写callback,
    https://github.com/babelouest/ulfius/blob/master/example_programs/simple_example/simple_example.c 
*/
struct sea_rest {
    uint8_t method;
    char uri[SEA_REST_URI_LEN];
    int (*callback)(const struct _u_request * request, struct _u_response * response, void * user_data);
};

/* 各个配置模块注册函数 */
int sea_rest_register(struct sea_rest *rest, int num);

/* 主线程在启动的最后调用启动 */
int sea_rest_start();
int sea_rest_stop();

int sea_rest_init(uint16_t port);
int sea_rest_fini();

#endif
