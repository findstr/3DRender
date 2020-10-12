#pragma once
#include <iostream>
#include <cmath>
//////
template<typename T, size_t N>
struct vectorx {
public:
	vectorx() {}
	vectorx(T x, T y) {
		static_assert(N == 2, "vector2 constructor");
		e[0] = x;
		e[1] = y;
		for (size_t i = 2; i < N; i++)
			e[i] = T();
	}
	vectorx(T x, T y, T z) {
		static_assert(N == 3, "vector3 constructor");
		e[0] = x;
		e[1] = y;
		e[2] = z;
		for (size_t i = 2; i < N; i++)
			e[i] = T();
	}
	vectorx(T x, T y, T z, T w) {
		static_assert(N == 4, "vector4 constructor");
		e[0] = x;
		e[1] = y;
		e[2] = z;
		e[3] = w;
	}
public:
	inline T u() const { static_assert(N >= 1, "no u"); return e[0]; };
	inline T v() const { static_assert(N >= 2, "no v"); return e[1]; };

	inline T x() const { static_assert(N >= 1, "no x"); return e[0]; };
	inline T y() const { static_assert(N >= 2, "no y"); return e[1]; };
	inline T z() const { static_assert(N >= 3, "no z"); return e[2]; };
	inline T w() const { static_assert(N >= 4, "no w"); return e[3]; };

	inline T &u() { static_assert(N >= 1, "no u"); return e[0]; };
	inline T &v() { static_assert(N >= 2, "no v"); return e[1]; };

	inline T &x() { static_assert(N >= 1, "no x"); return e[0]; };
	inline T &y() { static_assert(N >= 2, "no y"); return e[1]; };
	inline T &z() { static_assert(N >= 3, "no z"); return e[2]; };
	inline T &w() { static_assert(N >= 4, "no w"); return e[3]; };

	inline T &operator[](int i) { return e[i]; }
	inline T operator[](int i)  const { return e[i]; }
public:
	inline vectorx<T, N> operator - () const;
	inline vectorx<T, N> operator + (const vectorx<T, N> &a) const;
	inline vectorx<T, N> operator - (const vectorx<T, N> &a) const;
	inline vectorx<T, N> operator * (T a) const;
	inline vectorx<T, N> operator / (T a) const;

	inline vectorx<T, N> &operator = (const vectorx<T, N> &a);
	inline vectorx<T, N> &operator += (const vectorx<T, N> &a);
	inline vectorx<T, N> &operator -= (const vectorx<T, N> &a);
	inline vectorx<T, N> &operator *= (T a);
	inline vectorx<T, N> &operator /= (T a);
public:
	inline void zero();
	inline float norm() const;
	inline float squaredNorm() const;
	inline float dot(const vectorx<T, N> &a) const;
	inline vectorx<T, N> normalized() const;
	inline vectorx<T, N> cross(const vectorx<T, N> &a) const;
	inline vectorx<T, N> cwiseProduct(const vectorx<T, N> &a) const;

	friend std::ostream& operator<<(std::ostream& os, const vectorx<T, N> &v) {
		for (size_t i = 0; i < N; i++)
			os << v.e[i] << ",";
		return os;
	}
private:
	T e[N];
};


template<typename T, size_t N>
struct matrix {
public:
	matrix() {}
	matrix(
		const vectorx<T, N> &a,
		const vectorx<T, N> &b,
		const vectorx<T, N> &c)
	{
		static_assert(N == 3, "matrix3 constructor");
		e[0] = a;
		e[1] = b;
		e[2] = c;
	}
	matrix(
		const vectorx<T, N> &a,
		const vectorx<T, N> &b,
		const vectorx<T, N> &c,
		const vectorx<T, N> &d
		)
	{
		static_assert(N == 4, "matrix4 constructor");
		e[0] = a;
		e[1] = b;
		e[2] = c;
		e[3] = d;
	}
	matrix(
		T m00, T m01, T m02,
		T m10, T m11, T m12,
		T m20, T m21, T m22
	) {
		static_assert(N == 3, "matrix3 constructor");
		e[0] = vectorx<T, N>(m00, m10, m20);
		e[1] = vectorx<T, N>(m01, m11, m21);
		e[2] = vectorx<T, N>(m02, m12, m22);
	}
	matrix(
		T m00, T m01, T m02, T m03,
		T m10, T m11, T m12, T m13,
		T m20, T m21, T m22, T m23,
		T m30, T m31, T m32, T m33
	) {
		static_assert(N == 4, "matrix4 constructor");
		e[0] = vectorx<T, N>(m00, m10, m20, m30);
		e[1] = vectorx<T, N>(m01, m11, m21, m31);
		e[2] = vectorx<T, N>(m02, m12, m22, m32);
		e[3] = vectorx<T, N>(m03, m13, m23, m33);
	}
	inline void assign(
		T m00, T m01, T m02,
		T m10, T m11, T m12,
		T m20, T m21, T m22
	) {
		static_assert(N == 3, "matrix3 assign");
		e[0] = vectorx<T, N>(m00, m10, m20);
		e[1] = vectorx<T, N>(m01, m11, m21);
		e[2] = vectorx<T, N>(m02, m12, m22);
	}
	inline void assign(
		T m00, T m01, T m02, T m03,
		T m10, T m11, T m12, T m13,
		T m20, T m21, T m22, T m23,
		T m30, T m31, T m32, T m33
	) {
		static_assert(N == 4, "matrix4 assign");
		e[0] = vectorx<T, N>(m00, m10, m20, m30);
		e[1] = vectorx<T, N>(m01, m11, m21, m31);
		e[2] = vectorx<T, N>(m02, m12, m22, m32);
		e[3] = vectorx<T, N>(m03, m13, m23, m33);
	}
	inline void assign(const matrix<T, N-1> &a) {
		for (size_t i = 0; i < N-1; i++) {
			for (size_t j = 0; j < N-1; j++)
				e[j][i] = a(i, j);
			e[N-1][i] = T();
			e[i][N-1] = T();
		}
	}
public:
	static matrix<T, N> Identity() {
		matrix<T, N> x;
		for (size_t i = 0; i < N; i++) {
			x.e[i].zero();
			x.e[i][i] = 1;
		}
		return x;
	};
	friend std::ostream& operator<<(std::ostream& os, const matrix<T, N> &v) {
		for (size_t i = 0; i < N; i++)
			os << v.e[i] << ":";
		return os;
	}
	inline T operator () (int r, int c) const { return e[c][r]; }
	inline T &operator () (int r, int c) { return e[c][r]; }
	inline vectorx<T, N> operator * (const vectorx<T, N> &b) const;
	inline matrix<T, N> operator * (const matrix<T, N> &b) const;
private:
	vectorx<T, N> e[N];
};

////////vectorx

template<typename T, size_t N>
inline vectorx<T, N> vectorx<T, N>::operator - () const
{
	vectorx<T, N> r;
	for (size_t i = 0; i < N; i++)
		r.e[i] = -e[i];
	return r;
}

template<typename T, size_t N>
inline vectorx<T, N> vectorx<T, N>::operator + (const vectorx<T, N> &a) const
{
	vectorx<T, N> r;
	for (size_t i = 0; i < N; i++)
		r.e[i] = e[i] + a.e[i];
	return r;
}

template<typename T, size_t N>
inline vectorx<T, N> vectorx<T, N>::operator - (const vectorx<T, N> &a) const
{
	vectorx<T, N> r;
	for (size_t i = 0; i < N; i++)
		r.e[i] = e[i] - a.e[i];
	return r;
}

template<typename T, size_t N>
inline vectorx<T, N> vectorx<T, N>::operator * (T a) const
{
	vectorx<T, N> r;
	for (size_t i = 0; i < N; i++)
		r.e[i] = e[i] * a;
	return r;
}

template<typename T, size_t N>
inline vectorx<T, N> vectorx<T, N>::operator / (T a) const
{
	float div = 1.f / a;
	return (*this) * div;
}

template<typename T, size_t N>
inline vectorx<T, N> &vectorx<T, N>::operator = (const vectorx<T, N> &a)
{
	for (size_t i = 0; i < N; i++)
		e[i] = a.e[i];
	return *this;
}

template<typename T, size_t N>
inline vectorx<T, N> &vectorx<T, N>::operator += (const vectorx<T, N> &a)
{
	for (size_t i = 0; i < N; i++)
		e[i] += a.e[i];
	return *this;
}

template<typename T, size_t N>
inline vectorx<T, N> &vectorx<T, N>::operator -= (const vectorx<T, N> &a)
{
	for (size_t i = 0; i < N; i++)
		e[i] -= a.e[i];
	return *this;
}


template<typename T, size_t N>
inline vectorx<T, N> &vectorx<T, N>::operator *= (T a)
{
	for (size_t i = 0; i < N; i++)
		e[i] *= a;
	return *this;
}

template<typename T, size_t N>
inline vectorx<T, N> &vectorx<T, N>::operator /= (T a)
{
	float div = 1.f / a;
	for (size_t i = 0; i < N; i++)
		e[i] *= div;
	return *this;
}

template<typename T, size_t N>
inline void vectorx<T, N>::zero()
{
	for (size_t i = 0; i < N; i++)
		e[i] = T();
}

template<typename T, size_t N>
float vectorx<T, N>::squaredNorm() const
{
	T sum = T();
	for (size_t i = 0; i < N; i++)
		sum += e[i] * e[i];
	return sum;
}


template<typename T, size_t N>
float vectorx<T, N>::norm() const
{
	return std::sqrt(squaredNorm());
}

template<typename T, size_t N>
float vectorx<T, N>::dot(const vectorx<T, N> &a) const
{
	T sum = T();
	for (size_t i = 0; i < N; i++)
		sum += e[i] * a.e[i];
	return sum;
}

template<typename T, size_t N>
inline vectorx<T, N> vectorx<T, N>::normalized() const
{
	vectorx<T, N> r;
	float div = 1.f / norm();
	for (size_t i = 0; i < N; i++)
		r.e[i] = e[i] * div;
	return r;
}

template<typename T, size_t N>
inline vectorx<T, N> vectorx<T, N>::cross(const vectorx<T, N> &b) const
{
	static_assert(N == 3, "only vector3 can has cross");
	return vectorx<T, N>(
		y() * b.z() - z() * b.y(),
		z() * b.x() - x() * b.z(),
		x() * b.y() - y() * b.x()
	);
}

template<typename T, size_t N>
inline vectorx<T, N> vectorx<T, N>::cwiseProduct(const vectorx<T, N> &a) const
{
	vectorx<T, N> r;
	for (size_t i = 0; i < N; i++)
		r.e[i] = e[i] * a.e[i];
	return r;
}

template<typename T, size_t N>
inline vectorx<T, N> operator * (T k, const vectorx<T, N> &v)
{
	vectorx<T, N> r;
	for (size_t i = 0; i < N; i++)
		r[i] = k * v[i];
	return r;
}

////////matrix

template<typename T, size_t N>
inline vectorx<T, N> matrix<T, N>::operator * (const vectorx<T, N> &b) const
{
	vectorx<T, N> r;
	r.zero();
	for (size_t i = 0; i < N; i++)
		r += e[i] * b[i];
	return r;
}

template<typename T, size_t N>
inline matrix<T, N> matrix<T, N>::operator * (const matrix<T, N> &b) const
{
	matrix<T, N> r;
	for (size_t i = 0; i < N; i++)
		r.e[i] = (*this) * b.e[i];
	return r;
}

///////


using vector2i = vectorx<int, 2>;
using vector2f = vectorx<float, 2>;
using vector3f = vectorx<float, 3>;
using vector4f = vectorx<float, 4>;

using matrix3f = matrix<float, 3>;
using matrix4f = matrix<float, 4>;


