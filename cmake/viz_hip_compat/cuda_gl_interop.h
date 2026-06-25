// Copyright (c) 2026 Advanced Micro Devices, Inc.
// ROCm/HIP port author: Jeff Daily <jeff.daily@amd.com>
//
// HIP compat shim for the CUDA <cuda_gl_interop.h> include in cupoch's
// visualization shaders. PRIVATE + BEFORE on cupoch_visualization only.
// Pulls in ROCm's HIP GL<->interop API and the cupoch CUDA->HIP aliases
// (cudaGraphicsGLRegisterBuffer -> hipGraphicsGLRegisterBuffer, etc.) so the
// shader .cu files compile unchanged.
#pragma once
// cuda_to_hip.h pulls in <hip/hip_runtime.h> (hipError_t etc.) first; the GL
// interop header needs those types in scope.
#include "cupoch/utility/cuda_to_hip.h"
#include <hip/hip_gl_interop.h>
