#pragma once

#ifdef ENABLE_GPU
#include <iostream>
#include "cuda.h"
#include "cuda_profiler_api.h"
#include "curand_kernel.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "thrust/host_vector.h"
#include "thrust/device_vector.h"

#define ENTRY __global__
#define GPU __device__
#define CPU __host__
#define ALL __host__ __device__ 

#define std_swap(a,b)	thrust::swap(a, b)

#define cpu_fill(a,b,c) std::fill(a,b,c)
#define gpu_fill(a,b,c) thrust::fill(a,b,c)

#define checkX(val) check_cuda( (val), #val, __FILE__, __LINE__ )

static inline void check_cuda(cudaError_t result, char const *const func, const char *const file, int const line) {
    if (result) {
        std::cerr << "CUDA error = " << static_cast<unsigned int>(result) << " at " <<
            file << ":" << line << " '" << func << "' \n";
        cudaDeviceReset();
        exit(99);
    }
}

class gpu {
public:
	void *operator new(size_t len)
	{
		void *ptr;
		checkX(cudaMallocManaged(&ptr, len));
		return ptr;
	}
	void operator delete(void *ptr)
	{
		cudaFree(ptr);
	}
};

template<typename T> class gpu_vector;

template<typename T>
class cpu_vector : public std::vector<T> {
public:
	CPU cpu_vector() {}
	CPU cpu_vector(const gpu_vector<T> &src) {
		*this = src;
	}
	CPU void operator = (const gpu_vector<T> &src) {
		auto &dst = *this;
		int count = src.size();
		resize(count);
		if (count == 0)
			return ;
		int bytes =  count * sizeof(T);
		dst.resize(count);
		cudaMemcpy(&dst[0], src.data(), bytes, cudaMemcpyDeviceToHost);
	}
};

template<typename T>
class gpu_vector {
public:
	T *buf;
	size_t count;
public:
	CPU gpu_vector() {buf = nullptr; count = 0;}
	CPU ~gpu_vector() {if (buf != nullptr) cudaFree(buf);}
	CPU void operator = (const cpu_vector<T> &src) {
		if (src.size() == 0) {
			count = 0;
			return ;
		}
		if (buf != nullptr)
			cudaFree(buf);
		size_t bytes;
		count = src.size();
		bytes = count * sizeof(T);
		checkX(cudaMalloc(&buf, bytes));
		cudaMemcpy(buf, &src[0], bytes, cudaMemcpyHostToDevice);
	}
	CPU const void *data() const {return buf;}
	CPU void resize(int size) {
		if (buf != nullptr)
			cudaFree(buf);
		count = size;
		checkX(cudaMalloc(&buf, size * sizeof(T)));
	}
	ALL size_t size() const {
		return count;
	}
	GPU const T &operator [](int i) const {
		return buf[i];
	}
	GPU T &operator [](int i) {
		return buf[i];
	}
};

#define GPU_CALL(grid, block, func, ...) \
	func<<<grid, block>>>(__VA_ARGS__); \
	checkX(cudaGetLastError()); \
	checkX(cudaDeviceSynchronize()); 

struct thread_ctx {
	int x;
	int y;
	curandState rnd;
	GPU float rand() {
		return curand_uniform(&rnd);
	}
};

using random_ctx_t = curandState;

#else

#include <vector>

#define ENTRY 
#define GPU
#define CPU
#define ALL

#define std_swap std::swap

class gpu {};

template<typename T>
using gpu_vector = std::vector<T>;

template<typename T>
using cpu_vector = std::vector<T>;

using random_ctx_t = std::mt19937;

struct dim3 {
	int x; int y; int z;
};

struct thread_ctx {
	int x;
	int y;
	/*
	 12     static thread_local std::random_device dev;
   13     static thread_local std::mt19937 rng(dev());
   14     static thread_local std::uniform_real_distribution<float> dist(0.f, 1.f);
   15     return dist(rng);
*/
	random_ctx_t rnd;
	GPU float rand() {
		std::uniform_real_distribution<float> dist(0.f, 1.f);
		return dist(rnd);
	}
};




#define GPU_CALL(grid, block, func, ...) \
	func(__VA_ARGS__);

#endif