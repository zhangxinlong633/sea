#ifndef SEA_WORD_H
#define SEA_WORD_H

#include "../include/public.h"

struct sea_word ;

int sea_word_create(char *dir, char *word);
int sea_word_destroy(char *dir, char *word);

struct sea_word *sea_word_open(char *dir, char *word, int *error);
void sea_word_close(struct sea_word *doc);

/**
 * 思考: todo: 是否可以抽象成 find property by doc
 */
/**
 * 读最早的limit条数据
 * @param word
 * @return
 */
int sea_word_find_doc_by_words_forward(struct sea_word *doc, int limit, uint64_t *doc_id_list, int doc_id_list_len, uint32_t *ret_len);

/**
 * 读最新的limit条数据
 * @param word
 * @param limit
 * @param doc_id_list
 * @param doc_id_list_len
 * @param ret_len
 * @return
 */
int sea_word_find_doc_by_words_backward(struct sea_word *word, int limit, uint64_t *doc_id_list, int doc_id_list_len, int *ret_len);

/**
 * 插入一条记录，只有一个单词，会将它插入到一个单词的block中去，然后，把doc id append到它后面
 * @param word
 * @param word_len
 * @param doc_id
 * @return
 */
int sea_word_insert_doc(struct sea_word *word, uint64_t *doc_id, int doc_id_len);

#endif
