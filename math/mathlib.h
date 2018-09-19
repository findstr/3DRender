#ifndef	_MATH_LIB_H
#define _MATH_LIB_H

#include <math.h>
#include <stdlib.h>
#include "mathtype.h"
#include "mathconst.h"

#define PI ((float)3.141592654f)
#define PI2 (PI * 2)
#define PI_DIV_2 (PI / 2)
#define PI_DIV_4 (PI / 4)
#define PI_INV ((float)0.318309886f)


#define EPSILON_E3 ((float)1e-3)
#define EPSILON_E4 ((float)1e-4)
#define EPSILON_E5 ((float)1e-5)
#define EPSILON_E6 ((float)1e-6)

#ifndef ARRAYSIZE
#define ARRAYSIZE(a)	(sizeof(a)/sizeof(a[0]))
#endif
#define MIN(a, b)	((a) < (b) ? (a) : (b))
#define MAX(a, b)	((a) > (b) ? (a) : (b))
#define SWAP(a, b, t)	{t = a; a = b; b = t;}
#define FCMP(a,b)	((fabs(a - b) < EPSILON_E3) ? 1 : 0)

#define DEG_TO_RAD(ang)		((ang) * PI / 180.0f)
#define RAD_TO_DEG(rads)	((rads) * 180.0f / PI)

#define vector2_zero(v)	{(v)->x = (v)->y = 0.0f;}
#define vector3_zero(v)	{(v)->x = (v)->y = (v)->z = 0.0f;}
#define vector4_zero(v) {(v)->x = (v)->y = (v)->z = 0.0f; (v)->w = 1.0f;}

#define vector2_init(v,vx,vy) {(v)->x = (vx);(v)->y = (vy);}
#define vector3_init(v,vx,vy,vz) {(v)->x = (vx);(v)->y = (vy);(v)->z = (vz);}
#define vector4_init(v,vx,vy,vz) {\
	(v)->x = (vx);(v)->y = (vy);(v)->z = (vz); (v)->w = 1.0f;}
#define quaternion_init(q, vw, vx, vy, vz) {\
	(q)->w = (vw); (q)->x = (vx); (q)->y = (vy); (q)->z = (vz);}
#define plane3d_init(plane, vp0, normal) {\
	(plane)->p0 = (*vp0); (plane)->n = (*(normal));}
#define vector3_init_normalize(v,x,y,z) {\
	vector3_init(v,x,y,z);vector3_normalize(v, v);}

#define clamp(f, min, max) {\
	if (f > max) { \
		f = max; return ;}\
	if (f < min) { \
		f = min; return ;}}

void mathlib_init();

float fast_sin(float theta);
float fast_cos(float theta);

void vector2_add(const vector2_t *a, const vector2_t *b, vector2_t *c);
void vector2_sub(const vector2_t *a, const vector2_t *b, vector2_t *c);
void vector2_mul(const vector2_t *a, const vector2_t *b, vector2_t *c);
void vector2_div(const vector2_t *a, const vector2_t *b, vector2_t *c);
void vector2_mulf(const vector2_t *a, float f, vector2_t *c);
void vector2_divf(const vector2_t *a, float f, vector2_t *c);
void vector2_print(const char *str, const vector2_t *v);

void vector2_lerp(const vector2_t *a, const vector2_t *b, float t, vector2_t *c);
void vector3_lerp(const vector3_t *a, const vector3_t *b, float t, vector3_t *c);
void vector4_lerp(const vector4_t *a, const vector4_t *b, float t, vector4_t *c);

void vector3_normalize(const vector3_t *v, vector3_t *res);
void vector4_add(const vector4_t *a, const vector4_t *b, vector4_t *c);
void vector4_sub(const vector4_t *a, const vector4_t *b, vector4_t *c);
void vector4_divf(const vector4_t *a, float t, vector4_t *b);
void vector4_cross(const vector4_t *a, const vector4_t *b, vector4_t *c);
float vector4_dot(const vector4_t *a, const vector4_t *b);
float vector4_magnitude(const vector4_t *a);
float vector4_magnitude_fast(const vector4_t *a);

void vector4_mul_matrix(const vector4_t *v, const matrix_t *m, vector4_t *res);
void vector4_mul_quaternion(const vector4_t *v, const quaternion_t *q,
		vector4_t *r);
void vector4_print(const char *str, const vector4_t *v);

void quaternion_rotate_x(quaternion_t *q, float angle);
void quaternion_rotate_y(quaternion_t *q, float angle);
void quaternion_rotate_z(quaternion_t *q, float angle);
float quaternion_dot(const quaternion_t *a, const quaternion_t *b);
void quaternion_cross(const quaternion_t *a, const quaternion_t *b,
		quaternion_t *c);
void quaternion_normalize(quaternion_t *q);
void quaternion_inverse(const quaternion_t *a, quaternion_t *b);
void quaternion_to_matrix(const quaternion_t *a, matrix_t *b);
void quaternion_print(const char *str, const quaternion_t *q);

void matrix_mul(const matrix_t *a, const matrix_t *b, matrix_t *c);
void matrix_rotate_x(matrix_t *m, float angle);
void matrix_rotate_y(matrix_t *m, float angle);
void matrix_rotate_z(matrix_t *m, float angle);
void matrix_print(const char *str, const matrix_t *m);

#endif

