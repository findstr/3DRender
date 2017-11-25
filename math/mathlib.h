#ifndef	_MATH_H
#define _MATH_H

#include "mathtype.h"
#include "mathconst.h"

#define PI ((float)3.141592654f)
#define PI2 (PI * 2)
#define PI_DIV_2 (PI / 2)
#define PI_DIV_4 (PI / 4)
#define PI_INV ((float)0.318309886f)


#define EPSILON_E4 ((float)1e-4)
#define EPSILON_E5 ((float)1e-5)
#define EPSILON_E6 ((float)1e-6)

#ifndef ARRAYSIZE
#define ARRAYSIZE(a)	(sizeof(a)/sizeof(a[0]))
#endif
#define MIN(a, b)	((a) < (b) ? (a) : (b))
#define MAX(a, b)	((a) > (b) ? (a) : (b))
#define SWAP(a, b, t)		{t = a; a = b; b = t;}

#define DEG_TO_RAD(ang)		((ang) * PI / 180.0f)
#define RAD_TO_DEG(rads)	((rads) * 180.0f / PI)

#define vector2_zero(v)	{(v)->x = (v)->y = 0.0f;}
#define vector3_zero(v)	{(v)->x = (v)->y = (v)->z = 0.0f;}
#define vector4_zero(v) {(v)->x = (v)->y = (v)->z = 0.0f; (v)->w = 1.0f;}

#define vector2_init(v,vx,vy) {(v)->x = (vx);(v)->y = (vy);}
#define vector3_init(v,vx,vy,vz) {(v)->x = (vx);(v)->y = (vy);(v)->z = (vz);}
#define vector4_init(v,vx,vy,vz) {\
	(v)->x = (vx);(v)->y = (vy);(v)->z = (vz); (v)->w = 1.0f;}
#define plane3d_init(plane, vp0, normal) {\
	(plane)->p0 = (*vp0); (plane)->n = (*(normal));}
#define vector3_init_normalize(v,x,y,z) {\
	vector3_init(v,x,y,z);vector3_normalize(v, v);}

void mathlib_init();

float fast_sin(float theta);
float fast_cos(float theta);


void vector3_normalize(const vector3_t *v, vector3_t *res);

void vector4_add(const vector4_t *a, const vector4_t *b, vector4_t *c);
void vector4_sub(const vector4_t *a, const vector4_t *b, vector4_t *c);
void vector4_cross(const vector4_t *a, const vector4_t *b, vector4_t *c);
float vector4_dot(const vector4_t *a, const vector4_t *b);
void vector4_mul_matrix(const vector4_t *v, const matrix_t *m, vector4_t *res);
void vector4_print(const char *str, const vector4_t *v);

void matrix_mul(const matrix_t *a, const matrix_t *b, matrix_t *c);
void matrix_rotate_x(matrix_t *m, float angle);
void matrix_rotate_y(matrix_t *m, float angle);
void matrix_rotate_z(matrix_t *m, float angle);
void matrix_print(const char *str, const matrix_t *m);

#endif

