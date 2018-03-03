#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mathlib.h"

static float sin_table[361];
static float cos_table[361];

void
mathlib_init()
{
	int ang;
	for (ang = 0; ang <= 360; ang++) {
		float theta = DEG_TO_RAD((float)ang);
		sin_table[ang] = sin(theta);
		cos_table[ang] = cos(theta);
	}
	return ;
}

static inline float
fast_do(float theta, const float *table)
{
	int i,f;
	theta = fmodf(theta, 360.0f);
	if (theta < 0.0f)
		theta += 360.0f;
	i = (int)theta;
	f = theta - i;
	return table[i] + f * (table[i + 1] - table[i]);
}

float
fast_sin(float theta)
{
	return fast_do(theta, sin_table);
}

float
fast_cos(float theta)
{
	return fast_do(theta, cos_table);
}

int
fast_distance_2d(int x, int y)
{
	int min;
	x = abs(x);
	y = abs(y);
	min = x < y ? x : y;
	return (x + y - (min >> 1) - (min >> 2) + (min >> 4));
}

float
fast_distance_3d(float fx, float fy, float fz)
{
	int temp, dist;
	int x, y, z;
	x = fabs(fx) * 1024;
	y = fabs(fy) * 1024;
	z = fabs(fz) * 1024;
	if (y < x) SWAP(x, y, temp);
	if (z < y) SWAP(z, y, temp);
	if (y < x) SWAP(x, y, temp);
	//compute distance with 8% error
	dist = (z + 11 * (y >> 5) + (x >> 2));
	return (float)(dist >> 10);
}

/////vector

void
vector3_normalize(const vector3_t *v, vector3_t *out)
{
	float fact;
	float x = v->x;
	float y = v->y;
	float z = v->z;
	float len = sqrtf(x * x + y * y + z * z);
	if (len < EPSILON_E5)
		return ;
	fact = 1.0f / len;
	out->x *= fact;
	out->y *= fact;
	out->z *= fact;
	return ;
}

void
vector4_add(const vector4_t *a, const vector4_t *b, vector4_t *c)
{
	c->x = a->x + b->x;
	c->y = a->y + b->y;
	c->z = a->z + b->z;
	c->w = 1.0f;
	return ;
}

void
vector4_sub(const vector4_t *a, const vector4_t *b, vector4_t *c)
{
	c->x = a->x - b->x;
	c->y = a->y - b->y;
	c->z = a->z - b->z;
	c->w = 1.0f;
	return ;
}


void
vector4_cross(const vector4_t *a, const vector4_t *b, vector4_t *c)
{
	vector4_t vn;
	vn.x = a->y * b->z - a->z * b->y;
	vn.y = a->z * b->x - a->x * b->z;
	vn.z = a->x * b->y - a->y * b->x;
	vn.w = 1.0f;
	*c = vn;
	return ;
}

float
vector4_dot(const vector4_t *a, const vector4_t *b)
{
	return (a->x * b->x + a->y * b->y + a->z * b->z);
}

float
vector4_magnitude(const vector4_t *a)
{
	float x = a->x, y = a->y, z = a->z;
	return sqrtf(x*x + y*y + z*z);
}

float
vector4_magnitude_fast(const vector4_t *a)
{
	return fast_distance_3d(a->x, a->y, a->z);
}

void
vector4_mul_matrix(const vector4_t *va, const matrix_t *mb, vector4_t *res)
{
	float x, y, z, w;
	x = va->x;
	y = va->y;
	z = va->z;
	w = va->w;
	res->x = x * mb->m00 +  y * mb->m10 + z * mb->m20 + w * mb->m30;
	res->y = x * mb->m01 +  y * mb->m11 + z * mb->m21 + w * mb->m31;
	res->z = x * mb->m02 +  y * mb->m12 + z * mb->m22 + w * mb->m32;
	res->w = x * mb->m03 +  y * mb->m13 + z * mb->m23 + w * mb->m33;
	return ;
}

void
vector4_mul_quaternion(const vector4_t *v, const quaternion_t *q, vector4_t *r)
{
	quaternion_t qv, inv;
	quaternion_init(&qv, 0.0f, v->x, v->y, v->z);
	quaternion_inverse(q, &inv);
	quaternion_cross(&inv, &qv, &qv);
	quaternion_cross(&qv, q, &qv);
	r->x = qv.x;
	r->y = qv.y;
	r->z = qv.z;
	r->w = v->w;
	return ;
}


void
vector4_print(const char *str, const vector4_t *v)
{
	printf("%s vector4:%f:%f:%f:%f\n", str, v->x, v->y, v->z, v->w);
}

void
quaternion_rotate_x(quaternion_t *q, float angle)
{
	float theta = DEG_TO_RAD(angle * 0.5f);
	q->w = cos(theta);
	q->x = sin(theta);
	q->y = 0.0f;
	q->z = 0.0f;
	return ;
}

void
quaternion_rotate_y(quaternion_t *q, float angle)
{
	float theta = DEG_TO_RAD(angle * 0.5f);
	q->w = cos(theta);
	q->x = 0.0f;
	q->y = sin(theta);
	q->z = 0.0f;
	return ;
}

void
quaternion_rotate_z(quaternion_t *q, float angle)
{
	float theta = DEG_TO_RAD(angle * 0.5f);
	q->w = cos(theta);
	q->x = 0.0f;
	q->y = 0.0f;
	q->z = sin(theta);
	return ;
}

float
quaternion_dot(const quaternion_t *a, const quaternion_t *b)
{
	return a->w*b->w + a->x*b->x + a->y*b->y + a->z*b->z;
}

void
quaternion_cross(const quaternion_t *a, const quaternion_t *b, quaternion_t *c)
{
	float w, x, y, z;
	float w1, x1, y1, z1;
	w = a->w;
	x = a->x;
	y = a->y;
	z = a->z;
	w1 = w*b->w - x*b->x - y*b->y - z*b->z;
	x1 = w*b->x + x*b->w + z*b->y - y*b->z;
	y1 = w*b->y + y*b->w + x*b->z - z*b->x;
	z1 = w*b->z + z*b->w + y*b->x - x*b->y;
	/*
	w1 = w*b->w - x*b->x - y*b->y - z*b->z;
	x1 = w*b->x + x*b->w + y*b->z - z*b->y;
	y1 = w*b->y + y*b->w + z*b->x - x*b->z;
	z1 = w*b->z + z*b->w + x*b->y - y*b->x;
	*/
	c->w = w1;
	c->x = x1;
	c->y = y1;
	c->z = z1;
	return ;
}

void
quaternion_normalize(quaternion_t *q)
{
	float w = q->w, x = q->x, y = q->y, z = q->z;
	float mag = (float)sqrt(w*w + x*x + y*y + z*z);
	if (mag > 0.0f) {
		float div = 1.0f / mag;
		q->w = w * div;
		q->x = x * div;
		q->y = y * div;
		q->z = z * div;
	} else {
		assert(0);
		*q = IQUATERNION;
	}
	return ;
}

void
quaternion_inverse(const quaternion_t *a, quaternion_t *b)
{
	b->w = a->w;
	b->x = -a->x;
	b->y = -a->y;
	b->z = -a->z;
	return ;
}

void
quaternion_print(const char *str, const quaternion_t *q)
{
	printf("%s,w:%f,x:%f,y:%f,z:%f\n", str,
			q->w, q->x, q->y, q->z);
	return ;
}




typedef float (*matrix_array_t)[4];

void
matrix_mul(const matrix_t *a, const matrix_t *b, matrix_t *res)
{
	int i, r, c;
	matrix_t tmp;
	matrix_array_t ma = (matrix_array_t)&a->m00;
	matrix_array_t mb = (matrix_array_t)&b->m00;
	matrix_array_t mc = (matrix_array_t)&tmp.m00;
	for (r = 0; r < 4; r++) {
		for (c = 0; c < 4; c++) {
			float sum = 0;
			for (i = 0; i < 4; i++)
				sum += ma[r][i] * mb[i][c];
			mc[r][c] = sum;
		}
	}
	*res = tmp;
	return ;
}

void
matrix_rotate_x(matrix_t *m, float angle)
{
	float s, c;
	s = fast_sin(angle);
	c = fast_cos(angle);
	matrix_t val = {
		1.0f,	0.0f,	0.0f,	0.0f,
		0.0f,	c,	s,	0.0f,
		0.0f,	-s,	c,	0.0f,
		0.0f,	0.0f,	0.0f,	1.0f
	};
	*m = val;
	return ;
}

void
matrix_rotate_y(matrix_t *m, float angle)
{
	float s, c;
	s = fast_sin(angle);
	c = fast_cos(angle);
	matrix_t val = {
		c,	0.0f,	-s,	0.0f,
		0.0f,	1.0f,	0.0f,	0.0f,
		s,	0.0f,	c,	0.0f,
		0.0f,	0.0f,	0.0f,	1.0f,
	};
	*m = val;
	return ;
}

void
matrix_rotate_z(matrix_t *m, float angle)
{
	float s, c;
	s = fast_sin(angle);
	c = fast_cos(angle);
	matrix_t val = {
		c,	s,	0.0f,	0.0f,
		-s,	c,	0.0f,	0.0f,
		0.0f,	0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,	0.0f,	1.0f,
	};
	*m = val;
	return ;
}

void
matrix_print(const char *str, const matrix_t *m)
{
	printf("%s matrix:\n%f-%f-%f-%f\n%f-%f-%f-%f\n%f-%f-%f-%f\n%f-%f-%f-%f\n",
			str,
			m->m00, m->m01, m->m02, m->m03,
			m->m10, m->m11, m->m12, m->m13,
			m->m20, m->m21, m->m22, m->m23,
			m->m30, m->m31, m->m32, m->m33);
	return ;
}


