#ifndef _MATH_TYPE_H
#define _MATH_TYPE_H

typedef struct vector2 {
	struct {
		float x;
		float y;
	};
}vector2_t;

typedef struct vector3_t {
	float x;
	float y;
	float z;
}vector3_t;

typedef struct vector4 {
	float x;
	float y;
	float z;
	float w;
}vector4_t;

typedef struct quaternion {
	float w;
	float x;
	float y;
	float z;
} quaternion_t;

///////matrix
typedef struct matrix4x4 {
	struct {
		float m00, m01, m02, m03;
		float m10, m11, m12, m13;
		float m20, m21, m22, m23;
		float m30, m31, m32, m33;
	};
}matrix4x4_t, matrix_t;

#endif

