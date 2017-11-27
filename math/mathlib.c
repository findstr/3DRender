#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "mathlib.h"

/*
float fast_sin(float theta);
float fast_cos(float theta);

int fast_distance2(int x, int y);
float fast_distance3(float x, float y, float z);

void polar2d_to_vector2(polar2d_t *polar, vector2_t *rect);
void polar2d_to_rectxy(polar2d_t *polar, float *x, float *y);
void vector2_to_polar2d(vector2_t *rect, polar2d_t *polar);
void vector2_to_polarrt(vector2_t *rect, float *r, float theta);

void cylindrical3d_to_vector3(cylindrical3d_t *cyl, vector3_t *rect);
void cylindrical3d_to_rectxyz(cylindrical3d_t *cyl, float *x, float *y, float *z);
void vector3_to_cylindrical3d(vector3_t *rect, cylindrical3d_t *cyl);
void vector3_to_cylindricalrtz(vector3_t *rect, float *r, float *thea, float *z);

void spherical3d_to_vector3(spherical3d_t *sph, vector3_t *rect);
void shperical3d_to_rectxyz(spherical3d_t *sph, float *x, float *y, float *z);
void vector3_to_spherical3d(vector3_t *rect, spherical3d_t *sph);
void vector3_to_sphericalptp(vector3_t *rect, float *p, float *theta, float *phi);

void vector2_add(const vector2_t *a, const vector2_t *b, vector2_t *c);
void vector2_sub(const vector2_t *a, const vector2_t *b, vector2_t *c);
void vector2_scale(float k, const vector2_t *in, vector2_t *out);
float vector2_dot(const vector2_t *a, const vector2_t *b);
float vector2_length(const vector2_t *a);
void vector2_normalize(const vector2_t *a, vector2_t *b);
void vector2_build(const vector2_t *a, const vector2_t *term, vector2_t *res);

void vector3d_add(const vector3_t *a, const vector3_t *b, vector3_t *c);
void vector3d_sub(const vector3_t *a, const vector3_t *b, vector3_t *c);
void vector3_scale(float k, const vector3_t *in, vector3_t *res);
float vector3_dot(const vector3_t *a, const vector3_t *b);
float vector3_cross(const vector3_t *a, const vector3_t *b, vector3_t *res);
float vector3_length(const vector3_t *a);
float vector3_length_fast(const vector3_t *a);
void vector3_normalize(vector3_t *a);
void vector3_build(const vector3_t *init, const vector3_t *term, vector3_t *res);

void vector4d_add(const vector4_t *a, const vector4_t *b, vector4_t *c);
void vector4d_sub(const vector4_t *a, const vector4_t *b, vector4_t *c);
void vector4_scale(float k, const vector4_t *in, vector4_t *res);
float vector4_dot(const vector4_t *a, const vector4_t *b);
float vector4_cross(const vector4_t *a, const vector4_t *b, vector4_t *res);
float vector4_length(const vector4_t *a);
float vector4_length_fast(const vector4_t *a);
void vector4_normalize(vector4_t *a);
void vector4_build(const vector4_t *init, const vector4_t *term, vector4_t *res);

float matrix_det_2x2(const matrix2x2_t *m);
void matrix_add_2x2(const matrix2x2_t *a, const matrix2x2_t *b, matrix2x2_t *res);
void matrix_mul_2x2(const matrix2x2_t *a, const matrix2x2_t *b, matrix2x2_t *res);
void matrix_inverse_2x2(const matrix2x2_t *a, const matrix2x2_t *b, matrix2x2_t *res);
int solve_system_2x2(const matrix2x2_t *a, const matrix1x2_t *x, matrix1x2_t *b);

void matrix_add_3x3(const matrix3x3_t *a, const matrix3x3_t *b, matrix3x3_t *c);
void matrix_mul_vector3(const vector3_t *a, const matrix3x3_t *b, matrix3x3_t *res);
void matrix_inverse_3x3(const matrix3x3_t *m, matrix3x3_t *mi);
float matrix_det_3x3(const matrix3x3_t *m);
int solve_system_3x3(const matrix3x3_t *a, const matrix1x3_t *x, const matrix1x3_t *b);

void matrix_add_4x4(const matrix4x4_t *a, const matrix4x4_t *b, matrix4x4_t *c);
void matrix_mul_4x4(const matrix4x4_t *a, const matrix4x4_t *b, matrix4x4_t *c);
void matrix_inverse_4x4(const matrix4x4_t *a, matrix4x4_t *b);
void matrix_mul_1x4_4x4(const matrix1x4_t *a, const matrix4x4_t *b, matrix1x4_t *c);

void matrix_mul_vector3_4x4(const vector3_t *a, const matrix4x4_t *b, vector3_t *c);
void matrix_mul_vector3_4x3(const vector3_t *a, const matrix4x3_t *b, vector3_t *c);
void matrix_mul_vector3_4x4(const vector3_t *a, const matrix4x4_t *b, vector3_t *c);
void matrix_mul_vector3_4x3(const vector3_t *a, const matrix4x3_t *b, vector3_t *c);

void quat_add(quat_t *a, quat_t *b, quat_t *sum);
void quat_sub(quat_t *a, quat_t *b, quat_t *diff);
void quat_conjugate(quat_t *q, quat_t *conj);
void quat_scale(quat_t *q, float scale);
void quat_norm(quat_t *q);
void quat_norm2(quat_t *q);
void quat_normalize(quat_t *q);
void quat_unit_inverse(quat_t *q);
void quat_inverse(quat_t *q);
void quat_mul(quat_t *q);
void quat_triple_product(const quat_t *a, const quat_t *b, const quat_t *c, quat_t *res);
void quat_from_vector3_theta(quat_t *out, vector3_t *v, float theta);
void quat_from_vector4_theta(quat_t *out, vector4_t *v, float theta);
void quat_from_euler(quat_t *out, float z, float y, float x);
void quat_to_vector3_theta(const quat_t *in, vector3_t *v, float *theta);


void param_line2d_init(const vector2_t *init, const vector2_t *term, paramline2d_t *res);
void param_line2d_compute(paramline2d_t *p, float t, vector3_t *pt);
void param_line2d_intersect(paramline2d_t *p1, paramline2d_t *p2, float *t1, float *t2);
void param_line2d_intersectv(paramline2d_t *p1, paramline2d_t *p2, vector2_t *pt);

void param_line3d_init(const vector3_t *init, const vector3_t *term, paramline3d_t *res);
void param_line3d_compute(paramline3d_t *p, float t, vector3_t *pt);

float plane3d_compute_point(plane3d_t *plane, vector3_t *pt);
int plane3d_intersect_param_line3d(plane3d_t *plane, paramline3d_t *pline, float *t, vector3_t *pt);


*/

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

static float
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
	c->y = a->x - b->x;
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
	/*
	w1 = w*b->w - x*b->x - y*b->y - z*b->z;
	x1 = w*b->x + x*b->w + z*b->y - y*b->z;
	y1 = w*b->y + y*b->w + x*b->z - z*b->x;
	z1 = w*b->z + z*b->w + y*b->x - x*b->y;
	*/
	w1 = w*b->w - x*b->x - y*b->y - z*b->z;
	x1 = w*b->x + x*b->w + y*b->z - z*b->y;
	y1 = w*b->y + y*b->w + z*b->x - x*b->z;
	z1 = w*b->z + z*b->w + x*b->y - y*b->x;

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


