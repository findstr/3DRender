#ifndef _RGB_H
#define _RGB_H

#define RGB_SIZE	(3)
#define RGBA(r,g,b,a)	((rgba_t)(r) << 24 | (rgba_t)(g) << 16 |\
		(rgba_t)(b) << 8 | (rgba_t)(a))
#define RGBA_R(rgba)	(((rgba) >> 24) & 0xff)
#define RGBA_G(rgba)	(((rgba) >> 16) & 0xff)
#define RGBA_B(rgba)	(((rgba) >> 8) & 0xff)
#define RGBA_A(rgba)	(((rgba) >> 0) & 0xff)


typedef int rgba_t;

static inline rgba_t rgb_max(rgba_t n)
{
	return n > 255 ? 255 : n;
}

static inline rgba_t rgb_min(rgba_t n)
{
	return n < 0 ? 0 : n;
}

static inline rgba_t rgb_clamp(rgba_t n)
{
	if (n > 255)
		return 255;
	if (n < 0)
		return 0;
	return n;
}

static inline rgba_t rgba_add(rgba_t a, rgba_t b)
{
	int rr = rgb_max(RGBA_R(a) + RGBA_R(b));
	int gg = rgb_max(RGBA_G(a) + RGBA_G(b));
	int bb = rgb_max(RGBA_B(a) + RGBA_B(b));
	return RGBA(rr, gg, bb, 255);
}

static inline rgba_t rgba_sub(rgba_t a, rgba_t b)
{
	int rr = rgb_min(RGBA_R(a) - RGBA_R(b));
	int gg = rgb_min(RGBA_G(a) - RGBA_G(b));
	int bb = rgb_min(RGBA_B(a) - RGBA_B(b));
	return RGBA(rr, gg, bb, 255);
}

static inline rgba_t rgba_mul(rgba_t rgba, float factor)
{
	int r = RGBA_R(rgba) * factor;
	int g = RGBA_G(rgba) * factor;
	int b = RGBA_B(rgba) * factor;
	int a = RGBA_A(rgba) * factor;
	return RGBA(r, g, b, a);
}

static inline rgba_t rgba_div(rgba_t rgba, float factor)
{
	int r = RGBA_R(rgba) / factor;
	int g = RGBA_G(rgba) / factor;
	int b = RGBA_B(rgba) / factor;
	int a = RGBA_A(rgba) / factor;
	return RGBA(r, g, b, a);
}






#endif

