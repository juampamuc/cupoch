// Copyright (c) 2026 Advanced Micro Devices, Inc.
// Author: Jeff Daily <jeff.daily@amd.com>
//
// HIP compat prelude force-included (after <hip/hip_runtime.h>) into cupoch's
// vendored flann CUDA kdtree translation unit on the ROCm build. Bridges the few
// CUDA runtime / Thrust spellings flann uses that have no 1:1 HIP name in scope.
// NVIDIA builds never include this file.
#pragma once

#include <hip/hip_runtime.h>

// flann's algorithms/device_vector.h uses cudaStream_t and thrust::cuda::par.on
#ifndef cudaStream_t
#define cudaStream_t hipStream_t
#endif

// rocThrust exposes the stream-bound policy as thrust::hip::par, not
// thrust::cuda::par; make `thrust::cuda` resolve to `thrust::hip`.
#include <thrust/execution_policy.h>
#include <thrust/system/hip/execution_policy.h>
namespace thrust {
namespace hip {}  // ensure the namespace exists
namespace cuda = hip;
}  // namespace thrust

// flann calls cudaMemset2D in the builder; map to the HIP runtime entry point.
#ifndef cudaMemset2D
#define cudaMemset2D hipMemset2D
#endif
