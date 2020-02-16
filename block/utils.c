#include "public.h"

int make_sub_dir(char *dir, uint64_t block_id)
{
	char dirname[PATH_MAX] = {0};

	snprintf(dirname, PATH_MAX, "/%s/%llu", dir, block_id);
 	mkdir(dirname, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

	return 0;
}

uint64_t get_current_mills()
{
	uint64_t timestamp;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	timestamp = tv.tv_sec * (uint64_t)1000000 + tv.tv_usec;

	return timestamp;
}
