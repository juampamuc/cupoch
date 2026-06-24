// HIP compat shim: cupoch's vendored flann CUDA kdtree #includes <cuda.h>,
// <cuda_runtime.h> and <vector_types.h> (CUDA driver/runtime headers that do not
// exist under ROCm). On the HIP build this directory is prepended to flann's
// include path so those includes resolve here and pull in the HIP runtime
// instead, which provides the CUDA-spelled intrinsics flann uses
// (__forceinline__, __int_as_float, float3/float4, ...). NVIDIA builds never see
// this directory.
#pragma once
#include <hip/hip_runtime.h>
