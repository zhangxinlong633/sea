#include "word.h"
#include "md5.h"
#include "../block_array/array.h"

struct sea_word {
    struct col_array *array;
};


int sea_word_create(char *dir, char *word)
{
    struct col_array_info info;
    char word_md5[48] = {0};
    char path[PATH_MAX] = {0};

    memset(&info, 0, sizeof(struct col_array_info));
    uint32_t word_len = strlen(word) > 32 ? 31 : strlen(word);

    memcpy(info.col_name, word, word_len);
    info.record_type = VALUE_TYPE_ARR_UINT64;
    md5((unsigned char *)info.col_name, word_md5, 48);
    snprintf(path, PATH_MAX, "%s/%s", dir, word_md5);
    return col_array_create(path, &info);
}

int sea_word_destroy(char *dir, char *word)
{
    char word_md5[48] = {0};
    char path[PATH_MAX] = {0};
    md5((unsigned char *)word, word_md5, 48);

    snprintf(path, PATH_MAX, "%s/%s", dir, word_md5);

    col_array_destroy(path);
    return 0;
}

struct sea_word *sea_word_open(char *dir, char *word, int *error)
{
    struct sea_word *word_conn = malloc(sizeof(struct sea_word));
    if (word_conn == NULL) {
        goto exit;
    }
    char word_md5[48] = {0};
    char path[PATH_MAX] = {0};
    md5((unsigned char *)word, word_md5, 48);

    snprintf(path, PATH_MAX, "%s/%s", dir, word_md5);

    word_conn->array = col_array_open(path, error);

exit:
    return word_conn;
}

void sea_word_close(struct sea_word *word)
{
    col_array_close(word->array);
    free(word);
}

/**
 * 思考: todo: 是否可以抽象成 find property by doc
 */
/**
 * 读最早的limit条数据
 * @param word
 * @return
 */
int sea_word_find_doc_by_words_forward(struct sea_word *word, int limit, uint64_t *doc_id_list, int doc_id_list_len, uint32_t *ret_len)
{
    return col_array_read(word->array, 0, limit, (char *)doc_id_list, doc_id_list_len, ret_len);
}

int sea_word_find_doc_by_words_backward(struct sea_word *word, int limit, uint64_t *doc_id_list, int doc_id_list_len, int *ret_len)
{
    return 0;
}


/**
 * 插入一条记录，只有一个单词，会将它插入到一个单词的block中去，然后，把doc id append到它后面
 * @param word
 * @param word_len
 * @param doc_id
 * @return
 */
int sea_word_insert_doc(struct sea_word *word, uint64_t *doc_id, int doc_id_len)
{
    uint8_t over_limit = 0;
    return col_array_append(word->array, doc_id, doc_id_len, &over_limit);
}
