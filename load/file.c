#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "file.h"

char *
file_load(const char *path)
{
	int n;
	char *buf;
	FILE *fp;
	struct stat st;
	if(stat(path, &st))
		return NULL;
	buf = malloc(st.st_size);
	fp = fopen(path, "rb");
	n = fread(buf, st.st_size, 1, fp);
	assert(n == 1);
	fclose(fp);
	return buf;
}

