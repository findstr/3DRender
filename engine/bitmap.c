#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "mathlib.h"
#include "bitmap.h"

#define BM	('B' | 'M' << 8)

void
bitmap_load(const char *name, struct bitmap *map)
{
	int i, bitcount, pixel, x, y;
	uint8_t *ptr, *rgb, *end;
	rgba_t *buff;
	FILE *fp = fopen(name, "rb");
	assert(fp);
	fread((char *)&map->header, sizeof(map->header), 1, fp);
	if (map->header.type != BM) {
		fclose(fp);
		assert(0);
		return ;
	}
	fread((char *)&map->info, sizeof(map->info), 1, fp);
	bitcount = map->info.bitcount;
	fseek(fp, -((int)map->info.sizeimage), SEEK_END);
	assert(bitcount == 24);
	assert(map->info.sizeimage % 3 == 0);
	pixel = map->info.sizeimage / 3;
	ptr = rgb = (uint8_t *)malloc(map->info.sizeimage);
	end = rgb + map->info.sizeimage;
	printf("size:%d width:%d height:%d\n", map->info.sizeimage, map->info.width, map->info.height);
	buff = (rgba_t *)malloc(pixel * sizeof(rgba_t));
	map->buffer = buff;
	fread((char *)rgb, map->info.sizeimage, 1, fp);
	buff = &buff[pixel] - map->info.width;
	for (y = 0; y < map->info.height; y++) {
		for (x = 0; x < map->info.width; x++) {
			*buff++ = RGBA(ptr[2], ptr[1], ptr[0], 255);
			ptr += 3;
		}
		buff -= 2 * map->info.width;
	}
	free(rgb);
	fclose(fp);
	printf("bitmap load:%s ok\n", name);
	return ;
}


