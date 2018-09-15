#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "mathlib.h"
#include "bitmap.h"

#define BM		('B' | 'M' << 8)
#define PC_RESERVED     0x01    /* palette index used for animation */
#define PC_EXPLICIT     0x02    /* palette index is explicit to device */
#define PC_NOCOLLAPSE   0x04    /* do not match color to system palette */

#pragma pack(push, 1)

struct bitmap_header {	//BITMAPFILEHEADER from MSDN
	uint16_t type;
	uint32_t size;
	uint16_t rev1;
	uint16_t rev2;
	uint32_t offbits;
};

#pragma pack(pop)

struct bitmap_info {	//BITMAPINFOHEADER from msdn
	uint32_t size;
	uint32_t width;
	uint32_t height;
	uint16_t planes;
	uint16_t bitcount;
	uint32_t compression;
	uint32_t sizeimage;
	uint32_t xpelspermeter;
	uint32_t ypelspermeter;
	uint32_t clrused;
	uint32_t clrimportant;
};

struct bitmap_palette { //PALETTEENTRY from msdn
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t flag;
};


struct bitmap {
	struct bitmap_header header;
	struct bitmap_info info;
	struct bitmap_palette palette[256];
	rgba_t *buffer;
};


void
bitmap_load(const char *name, struct texture *tex)
{
	struct bitmap_header header;
	struct bitmap_info info;
	struct bitmap_palette palette[256];

	int i, bitcount, pixel, x, y;
	rgba_t *buff;
	uint8_t *ptr, *rgb, *end;
	FILE *fp = fopen(name, "rb");
	assert(fp);
	fread((char *)&header, sizeof(header), 1, fp);
	if (header.type != BM) {
		fclose(fp);
		assert(0);
		return ;
	}
	fread((char *)&info, sizeof(info), 1, fp);
	bitcount = info.bitcount;
	fseek(fp, -((int)info.sizeimage), SEEK_END);
	assert(bitcount == 24);
	assert(info.sizeimage % 3 == 0);
	pixel = info.sizeimage / 3;
	ptr = rgb = (uint8_t *)malloc(info.sizeimage);
	end = rgb + info.sizeimage;
	printf("size:%d width:%d height:%d\n", info.sizeimage, info.width, info.height);
	buff = (rgba_t *)malloc(pixel * sizeof(rgba_t));
	tex->color = buff;
	tex->height = info.height;
	tex->width = info.width;
	fread((char *)rgb, info.sizeimage, 1, fp);
	buff = &buff[pixel] - info.width;
	for (y = 0; y < info.height; y++) {
		for (x = 0; x < info.width; x++) {
			*buff++ = RGBA(ptr[2], ptr[1], ptr[0], 255);
			ptr += 3;
		}
		buff -= 2 * info.width;
	}
	free(rgb);
	fclose(fp);
	printf("bitmap load:%s ok\n", name);
	return ;
}


