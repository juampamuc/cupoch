/**
 * Copyright (c) 2020 Neka-Nat
 * Copyright (c) 2026 Advanced Micro Devices, Inc.
 *
 * ROCm/HIP port author: Jeff Daily <jeff.daily@amd.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 **/
#pragma once

// Single point of CUDA<->HIP translation for cupoch. On ROCm this
// aliases the small CUDA runtime surface cupoch uses to the HIP spellings and
// pulls in the HIP runtime; on NVIDIA it is a plain include of the CUDA runtime.
// rocThrust/hipCUB are header drop-ins (same thrust::/cub:: API and header
// paths), so no Thrust/CUB symbols are aliased here.
#if defined(USE_HIP)

// libc host declarations must win over HIP's __device__ memcpy/memset overloads
// once <hip/hip_runtime.h> is in scope (host helpers in .cu compiled as HIP).
#include <cstdlib>
#include <cstring>

#include <hip/hip_runtime.h>

// --- error / status ---
#define cudaError_t hipError_t
#define cudaSuccess hipSuccess
#define cudaGetLastError hipGetLastError
#define cudaGetErrorString hipGetErrorString

// --- device management ---
#define cudaGetDevice hipGetDevice
#define cudaSetDevice hipSetDevice
#define cudaGetDeviceCount hipGetDeviceCount
#define cudaGetDeviceProperties hipGetDeviceProperties
#define cudaDeviceProp hipDeviceProp_t
#define cudaDeviceSynchronize hipDeviceSynchronize

// --- streams ---
#define cudaStream_t hipStream_t
#define cudaStreamCreate hipStreamCreate
#define cudaStreamDestroy hipStreamDestroy
#define cudaStreamSynchronize hipStreamSynchronize

// --- memory ---
#define cudaMalloc hipMalloc
#define cudaFree hipFree
#define cudaMallocHost hipHostMalloc
#define cudaFreeHost hipHostFree
#define cudaMemcpy hipMemcpy
#define cudaMemcpyAsync hipMemcpyAsync
#define cudaMemset hipMemset
#define cudaMemcpyKind hipMemcpyKind
#define cudaMemcpyHostToDevice hipMemcpyHostToDevice
#define cudaMemcpyDeviceToHost hipMemcpyDeviceToHost
#define cudaMemcpyDeviceToDevice hipMemcpyDeviceToDevice
#define cudaMemcpyHostToHost hipMemcpyHostToHost

// --- OpenGL interop (visualization only) ---
// ROCm ships the hipGraphics* GL<->HIP interop API in <hip/hip_gl_interop.h>.
// These aliases are defined unconditionally on HIP (harmless when GL is unused);
// the header itself is pulled in by the visualization compat include so that
// non-GL translation units do not need OpenGL headers in scope.
#define cudaGraphicsResource_t hipGraphicsResource_t
#define cudaGraphicsGLRegisterBuffer hipGraphicsGLRegisterBuffer
#define cudaGraphicsGLRegisterImage hipGraphicsGLRegisterImage
#define cudaGraphicsMapResources hipGraphicsMapResources
#define cudaGraphicsUnmapResources hipGraphicsUnmapResources
#define cudaGraphicsResourceGetMappedPointer hipGraphicsResourceGetMappedPointer
#define cudaGraphicsUnregisterResource hipGraphicsUnregisterResource
#define cudaGraphicsMapFlagsNone hipGraphicsRegisterFlagsNone

#else
#include <cuda_runtime.h>
#endif
