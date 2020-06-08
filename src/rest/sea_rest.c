#include <ulfius.h>
#include <u_example.h>

#include "sea_rest.h"

static struct _u_instance instance;

char *sea_rest_method_get_name(uint8_t method) 
{
    switch (method) {
        case SEA_REST_METHOD_GET:
            return "GET"; 
        case SEA_REST_METHOD_POST:
            return "POST"; 
        case SEA_REST_METHOD_PUT:
            return "PUT"; 
        case SEA_REST_METHOD_DELETE:
            return "DELETE"; 
        default:
            return "";
    }
}

int sea_rest_register(struct sea_rest *rest, int num)
{
    for (int i = 0; i < num; i ++) {

        if (rest[i].method < SEA_REST_METHOD_GET 
            || rest[i].method > SEA_REST_METHOD_DELETE
            || rest[i].callback == NULL
            || rest[i].uri[0] == 0) {
                /* todo log or break */
                continue;
            }

        ulfius_add_endpoint_by_val(&instance, sea_rest_method_get_name(rest[i].method), SEA_API_PREFIX, rest[i].uri, 0, rest[i].callback, NULL);
    }

    return 0;    
}

/**
 * Default callback function called if no endpoint has a match
 */
int callback_default(const struct _u_request * request, struct _u_response * response, void * user_data) 
{
    ulfius_set_string_body_response(response, 404, "Page not found, do what you want");
    return U_CALLBACK_CONTINUE;
}

int sea_rest_start()
{
    return ulfius_start_framework(&instance);
}

int sea_rest_stop()
{
    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);
    return 0;
}

int sea_rest_init(uint16_t port)
{
    int ret = EINVAL;

    if (ulfius_init_instance(&instance, port, NULL, NULL) != U_OK) {
        goto exit;
    }

    instance.max_post_body_size = SEA_REST_MAX_BODY_SIZE;
    u_map_put(instance.default_headers, "Access-Control-Allow-Origin", "*");
    ulfius_set_default_endpoint(&instance, &callback_default, NULL);

    ret = 0;

exit:
    return ret;
}

int sea_rest_fini()
{
    sea_rest_stop();
    return 0;
}
