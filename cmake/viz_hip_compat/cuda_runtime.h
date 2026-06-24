// Copyright (c) 2026 Advanced Micro Devices, Inc.
// ROCm/HIP port author: Jeff Daily <jeff.daily@amd.com>
//
// HIP compat shim for the CUDA <cuda_runtime.h> include in cupoch's
// visualization shaders. PRIVATE + BEFORE on cupoch_visualization only, so it
// does not shadow the real <cuda_runtime.h> that rocThrust's own internals need
// on a CUDA system path. Routes to the cupoch CUDA->HIP alias header.
#pragma once
#include "cupoch/utility/cuda_to_hip.h"
