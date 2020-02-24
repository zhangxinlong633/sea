#ifndef __NEIGHBOUR_LIST_H
#define __NEIGHBOUR_LIST_H

struct neighbour_list;
int neighbour_list_init();
int neighbour_list_fini();

struct neighbour_list *neighbour_list_create(uint32_t node_cap);
void neighbour_list_destroy(struct neighbour_list *neighbour_list);

int neighbour_list_insert(struct neighbour_list *neighbour_list, uint64_t block_id_start, uint64_t block_id_stop, void *node, uint32_t node_len);
int neighbour_list_get(struct neighbour_list *neighbour_list, uint64_t block_id, void *node, uint32_t node_len, uint32_t *ret_node_len);

int neighbour_list_save(char *path, int path_len);
int neighbour_list_load(char *path, int path_len);

#endif

