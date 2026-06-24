// Copyright (c) 2026 Advanced Micro Devices, Inc.
// Author: Jeff Daily <jeff.daily@amd.com>
//
// HIP replacement for flann's vendored util/cutil_math.h, selected only on the
// ROCm build (the flann_hip_compat dir is BEFORE on flann_cuda_s's PRIVATE
// include path; NVIDIA builds use the real cutil_math.h unchanged).
//
// Why a replacement instead of #ifdef-guarding the original 1300-line header:
// HIP's HIP_vector_type<float,N> already ships componentwise + - * / for BOTH
// vector-vector AND vector-scalar, plus make_floatN and member .x/.y/.z/.w, so
// the original header's ~270 operator/min/max/fminf definitions are entirely
// redundant under HIP and only cause "ambiguous operator" / "redefinition of
// max" errors. flann's CUDA kdtree actually uses only a tiny set of the NAMED
// helpers cutil_math adds on top of CUDA's operator-less vector structs; those
// (dot, length, fabs on vectors) are provided here, built on HIP's operators.
#ifndef FLANN_UTIL_CUTIL_MATH_H_HIP
#define FLANN_UTIL_CUTIL_MATH_H_HIP

#include <hip/hip_runtime.h>
#include <math.h>

// dot products (HIP has no dot() for its vector types)
inline __host__ __device__ float dot(float2 a, float2 b) {
    return a.x * b.x + a.y * b.y;
}
inline __host__ __device__ float dot(float3 a, float3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
inline __host__ __device__ float dot(float4 a, float4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// Euclidean length
inline __host__ __device__ float length(float2 v) { return sqrtf(dot(v, v)); }
inline __host__ __device__ float length(float3 v) { return sqrtf(dot(v, v)); }
inline __host__ __device__ float length(float4 v) { return sqrtf(dot(v, v)); }

// componentwise absolute value on vectors (scalar fabs comes from <math.h>)
inline __host__ __device__ float2 fabs(float2 v) {
    return make_float2(fabsf(v.x), fabsf(v.y));
}
inline __host__ __device__ float3 fabs(float3 v) {
    return make_float3(fabsf(v.x), fabsf(v.y), fabsf(v.z));
}
inline __host__ __device__ float4 fabs(float4 v) {
    return make_float4(fabsf(v.x), fabsf(v.y), fabsf(v.z), fabsf(v.w));
}

#endif  // FLANN_UTIL_CUTIL_MATH_H_HIP
