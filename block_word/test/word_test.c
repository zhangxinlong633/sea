#include "../word.h"

int main(int argc, char **argv)
{
    int error;
    int ret = EINVAL;
    char *word = "test";
    char *dir = "/Users/zhangxinlong/tmp/";

    sea_word_create(dir, word);

    struct sea_word *word_conn = sea_word_open(dir, word, &error);
    if (word_conn == NULL || error != 0) {
        goto exit;
    }

    uint64_t word_id[1024] = {0};
    uint64_t read_word_id[1024] = {0};

    for (int i = 0; i < 1024; i ++) {
        word_id[i] = i;
    }

    int size = 1024 * sizeof(uint64_t);

    ret = sea_word_insert_doc(word_conn, word_id, size);
    if (ret != 0) {
        goto exit;
    }

    uint32_t ret_len = 0;
    ret = sea_word_find_doc_by_words_forward(word_conn, 1024, read_word_id, 1024 * 8, &ret_len);
    if (ret != 0 || ret_len == 0) {
        goto exit;
    }

    for (int i = 0; i < 1024; i ++) {
        if (word_id[i] != read_word_id[i]) {
            ret = ENOENT;
            goto exit;
        }
    }

    ret = 0;

exit:
    if (word_conn) {
        sea_word_close(word_conn);
    }
    sea_word_destroy(dir, word);
    return ret;
}
