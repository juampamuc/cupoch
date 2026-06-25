// HIP compat shim for lbvh/lbvh_index CUDA include <cuda_runtime.h>.
// PRIVATE to cupoch_knn on the ROCm build; maps the CUDA header to the HIP
// runtime/vector-type header (which provides make_floatN, __clz, etc.).
#pragma once
#include <hip/hip_runtime.h>
