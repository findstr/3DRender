#ifndef _BITMAP_H
#define _BITMAP_H

#include <stdint.h>
#include "rgb.h"

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

void bitmap_load(const char *name, struct bitmap *map);






#endif

