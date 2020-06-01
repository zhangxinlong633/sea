#include "../../src/block/word/word.h"

#define COUNT 10000 * 10000 * 10

int main(int argc, char **argv)
{
    int error = 0;
    int ret = EINVAL;
    char *dir = "/Users/zhangxinlong/tmp/word";
    char *word = "bench";

    sea_word_create(dir, word);

    struct sea_word *word_conn = sea_word_open(dir, word, &error);
    if (word_conn == NULL || error != 0) {
        goto exit;
    }

    uint32_t record_count = 1024 * 100;
    uint32_t record_size = record_count * 8;
    uint64_t *record = malloc(record_size);

    for (int i = 0; i < record_count; i ++) {
        record[i] = i;
    }

    uint32_t start = time(NULL);
    for (int i = 0; i < COUNT; i += record_count) {
        ret = sea_word_insert_doc(word_conn, record, record_size);
        if (ret != 0) {
            goto exit;
        }

        if (i % (10000 * 1000) == 0) {
            uint32_t stop = time(NULL);
            printf("word bench, count: %u, from %u to %u, speed: %u\n", i, start, stop,
                   i / (((stop - start) > 0) ? (stop - start) : 1));
        }
    }

    uint32_t stop = time(NULL);
    printf("word bench, count: %u, from %u to %u, speed: %u\n", COUNT, start, stop,
           COUNT / (((stop - start) > 0) ? (stop - start) : COUNT));
    ret = 0;

    exit:
    if (word_conn) {
        sea_word_close(word_conn);
    }

    return ret;
}
