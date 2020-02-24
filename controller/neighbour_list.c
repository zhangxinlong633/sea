

struct neighbour_list {
    uint32_t node_cap;

	RB_HEAD(neighbour_list_tree, neighbour_entry) head;
};

struct neighbour_entry {
	uint64_t block_id;
	uint64_t range;
	uint32_t node_len;
	RB_ENTRY(neighbour_entry)   entry;
};

static int __neighbour_list_compare(struct neighbour_entry *a, struct neighbour_entry *b)
{
    int ret = 0;

    if (a->block_id < b->block_id) {
        return -1;
    } else if (a->block_id > b->block_id) {
		return 1;
	}

    return 0;
}

RB_PROTOTYPE(neighbour_list_tree, neighbour_entry, entry, __neighbour_list_compare);
RB_GENERATE(neighbour_list_tree, neighbour_entry, entry, __neighbour_list_compare);


struct neighbour_list *neighbour_list_create(uint32_t node_cap)
{
	struct neighbour_list *list = NULL;

	list = malloc(sizeof(struct neighbour_list));
	if (list == NULL) {
		goto exit;
	}

	list->node_cap = node_cap;

	RB_INIT(&list->head);
exit:
	return list;
}

void neighbour_list_destroy(struct neighbour_list *neighbour_list)
{
	free(neighbour_list);
}

// todo://blockid range在同一台机器
int neighbour_list_insert(struct neighbour_list *neighbour_list, uint64_t block_id_start, uint64_t block_id_stop, void *node, uint32_t node_len)
{
	int ret = EINVAL;
	uint32_t node_count = 0;

	if (node_len > 0) {
		node_count = min(phone_list->node_cap, node_len);
	} else {
		node_count = phone_list->node_cap;
	}

	struct neighbour_entry *entry = malloc(sizeof(struct neighbour_entry) + sizeof(uint32_t) * node_count);
	if (entry == NULL) {
		goto exit;
	}

	entry->block_id = block_id_start; 
	entry->range = block_id_stop - block_id_start; 
	memcpy(entry->node, node, sizeof(uint32_t) * node_count);
	entry->node_len = node_count;

	RB_INSERT(neighbour_list_tree, &neighbour_list->head, entry);

exit:
	return ret;
}

int neighbour_list_get(struct neighbour_list *neighbour_list, uint64_t block_id, void *node, uint32_t node_len, uint32_t *ret_node_len)
{
	int ret = EINVAL;
	uint32_t node_count = 0;
	struct neighbour_entry tmp;
	struct neighbour_entry *entry = NULL;

	tmp.block_id  = block_id / neighbour_list->node_cap;

   	entry = RB_FIND(neighbour_list_tree, &neighbour_list->head, &tmp);
	if (entry == NULL) {
		goto exit;
	}

    node_count = min(entry->node_len, node_len);
    
    memcpy(node, entry->node, node_count);
    *ret_node_len = node_count;

exit:
	return ret;	
}

int neighbour_list_save(char *path, int path_len)
{
	return 0;
}

int neighbour_list_load(char *path, int path_len)
{
	return 0;
}

int neighbour_list_init()
{
	return 0;
}

int neighbour_list_fini()
{

}


