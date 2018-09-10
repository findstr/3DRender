#ifndef _RGB_H
#define _RGB_H

#define RGB_SIZE	(3)
#define RGBA(r,g,b,a)	((rgba_t)(r) << 24 | (rgba_t)(g) << 16 |\
		(rgba_t)(b) << 8 | (rgba_t)(a))
#define RGBA_R(rgba)	((int)(((rgba) >> 24) & 0xff))
#define RGBA_G(rgba)	((int)(((rgba) >> 16) & 0xff))
#define RGBA_B(rgba)	((int)(((rgba) >> 8) & 0xff))
#define RGBA_A(rgba)	((int)(((rgba) >> 0) & 0xff))


typedef unsigned int rgba_t;

#define inline __inline

static inline int rgb_max(int n)
{
	return n > 255 ? 255 : n;
}

static inline int rgb_min(int n)
{
	return n < 0 ? 0 : n;
}

static inline int rgb_clamp(int n)
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
	int r = (int)(RGBA_R(rgba) * factor);
	int g = (int)(RGBA_G(rgba) * factor);
	int b = (int)(RGBA_B(rgba) * factor);
	int a = (int)(RGBA_A(rgba) * factor);
	return RGBA(r, g, b, a);
}

static inline rgba_t rgba_div(rgba_t rgba, float factor)
{
	int r = (int)(RGBA_R(rgba) / factor);
	int g = (int)(RGBA_G(rgba) / factor);
	int b = (int)(RGBA_B(rgba) / factor);
	int a = (int)(RGBA_A(rgba) / factor);
	return RGBA(r, g, b, a);
}

static inline rgba_t rgba_lerp(rgba_t a, rgba_t b, float t)
{
	int rr = rgb_clamp((int)(RGBA_R(a) + (RGBA_R(b) - RGBA_R(a)) * t));
	int gg = rgb_clamp((int)(RGBA_G(a) + (RGBA_G(b) - RGBA_G(a)) * t));
	int bb = rgb_clamp((int)(RGBA_B(a) + (RGBA_B(b) - RGBA_B(a)) * t));
	int aa = rgb_clamp((int)(RGBA_A(a) + (RGBA_A(b) - RGBA_A(a)) * t));
	return RGBA(rr, gg, bb, aa);
}






#endif

