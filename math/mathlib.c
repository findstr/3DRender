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

void plane3d_init(plane3d_t *plane, vector3_t * p0, vector3_t *normal, int normalize);
float plane3d_compute_point(plane3d_t *plane, vector3_t *pt);
int plane3d_intersect_param_line3d(plane3d_t *plane, paramline3d_t *pline, float *t, vector3_t *pt);


*/

