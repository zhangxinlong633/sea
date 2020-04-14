#include "public.h"

struct sea_block_record {
    uint32_t count;
    TAILQ_HEAD(sea_block_record_header, sea_block_record_data) head;
};

struct sea_block_record_data {
    uint32_t length;
    TAILQ_ENTRY(sea_block_record_data) entry;

    char data[0];
};

struct sea_block_record *sea_block_record_malloc()
{
    struct sea_block_record *record = malloc(sizeof(struct sea_block_record));
    if (record == NULL) {
        goto exit;
    }

    TAILQ_INIT(&record->head);

exit:
    return record;
}

void sea_block_record_free(struct sea_block_record *header)
{
    struct sea_block_record_data *data;
    struct sea_block_record_data *tmp;

    TAILQ_FOREACH_SAFE(data, &header->head, entry, tmp) {
        TAILQ_REMOVE(&header->head, data, entry);
        free(data);
    }

    free(header);
}

int sea_block_record_append(struct sea_block_record *header, void *buf, uint32_t length)
{ 
    int ret = EINVAL;

    struct sea_block_record_data *data = malloc(length + sizeof(struct sea_block_record_data));    
    if (data == NULL) {
        goto exit;
    }

    data->length = length;
    memcpy(data->data, buf, length);
    TAILQ_INSERT_TAIL(&header->head, data, entry);

    ret = 0;

exit:
    return ret;
}

int sea_block_record_foreach(struct sea_block_record *header, sea_block_record_foreach_func func, void *args)
{
    struct sea_block_record_data *data = NULL;

    TAILQ_FOREACH(data, &header->head, entry) {
        func(data->data, data->length, args);
    }

    return 0;
}


void *sea_block_record_get_first(struct sea_block_record *header, uint32_t *length)
{
	struct sea_block_record_data *data = TAILQ_FIRST(&header->head);

	if (data == NULL) {
		return NULL;
	}

	*length = data->length;

	return data->data;
}



