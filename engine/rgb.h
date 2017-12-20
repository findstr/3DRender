#ifndef _RGB_H
#define _RGB_H

#define RGB_SIZE	(3)
typedef int rgb_t;
static inline rgb_t rgb_max(rgb_t n)
{
	return n > 255 ? 255 : n;
}

static inline rgb_t rgb_add(rgb_t a, rgb_t b)
{
	int rr = ((a) >> 16) & 0x0f + ((b) >> 16) & 0x0f;
	int rg = ((a) >> 8) & 0x0f + ((b) >> 8) & 0x0f;
	int rb = (a) & 0x0f + (b) & 0x0f;
	rr = rgb_max(rr); rg = rgb_max(rg); rb = rgb_max(rb);
	return (rr << 16) | (rg << 8) | rb;
}



#endif

