// HIP compat shim for the CUDA include <math_constants.h>.
// PRIVATE to the lbvh consumers (cupoch_knn, cupoch_collision) on the ROCm
// build; provides the few CUDART_* constants lbvh uses.
#pragma once
#include <limits>

#ifndef CUDART_INF_F
#define CUDART_INF_F __builtin_huge_valf()
#endif
#ifndef CUDART_INF
#define CUDART_INF __builtin_huge_val()
#endif
