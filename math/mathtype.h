#ifndef _MATH_TYPE_H
#define _MATH_TYPE_H

/*
typedef struct vertex2i {
	int x, y;
}vextex2i_t;

typedef struct vertex2f {
	float x, y;
}vertex2f_t;


*/

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

////////line

typedef struct paramline2d {
	vector2_t p0;
	vector2_t p1;
	vector2_t v;
}paramline2d_t;

typedef struct paramline3d {
	vector3_t p0;
	vector3_t p1;
	vector3_t v;
}paramline3d_t;

////////plane

typedef struct plane3d {
	vector3_t p0;
	vector3_t n;
}plane3d_t;

///////matrix
typedef struct matrix4x4 {
	struct {
		float m00, m01, m02, m03;
		float m10, m11, m12, m13;
		float m20, m21, m22, m23;
		float m30, m31, m32, m33;
	};
}matrix4x4_t, matrix_t;

typedef struct matrix4x3 {
	union {
		float a[4][3];
		struct {
			float m00, m01, m02;
			float m10, m11, m12;
			float m20, m21, m22;
			float m30, m31, m32;
		};
	};

}matrix4x3_t;

typedef struct matrix1x4 {
	union {
		float a[4];
		struct {
			float m00, m01, m02, m03;
		};
	};
}matrix1x4_t;

typedef struct matrix3x3 {
	union {
		float a[3][3];
		struct {
			float m00, m01, m02;
			float m10, m11, m12;
			float m20, m21, m22;
		};
	};
}matrix3x3_t;


typedef struct matrix1x3 {
	union {
		float a[3];
		struct {
			float m00, m01, m02;
		};
	};
}matrix1x3_t;

typedef struct matrix3x2 {
	union {
		float a[3][2];
		struct {
			float m00, m01;
			float m10, m11;
			float m20, m21;
		};
	};
}matrix3x2_t;

typedef struct matrix2x2 {
	union {
		float a[2][2];
		struct {
			float m00, m01;
			float m10, m11;
		};
	};
}matrix2x2_t;

typedef struct matrix1x2 {
	union {
		float a[2];
		struct {
			float m00, m01;
		};
	};
}matrix1x2_t;

typedef struct quat {
	union {
		float a[4];
		struct {
			float q0;
			vector3_t qv;
		};
		struct {
			float w, x, y, z;
		};
	};
}quat_t;

///////////////ange coord
typedef struct polar2d {
	float r;
	float theta;
}polar2d_t;

typedef struct cylindrical3d {
	float r;
	float theta;
	float z;
}cylindrical3d_t;

typedef struct spherical3d {
	float p;
	float theta;
	float phi;
}spherical3d_t;

#endif

