// HIP compat shim for flann's <cuda_runtime.h>/"cuda_runtime.h" include (used by
// flann's cutil_math.h and kdtree builder). This directory is PRIVATE to the
// flann_cuda_s target only, so it does not shadow <cuda_runtime.h> for any other
// translation unit. See cuda.h in this dir.
#pragma once
#include <hip/hip_runtime.h>
