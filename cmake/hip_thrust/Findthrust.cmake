# HIP-aware replacement for stdgpu's bundled Findthrust.cmake.
#
# stdgpu 1.3.0's Findthrust.cmake, once it sees THRUST_VERSION >= 2.0.0,
# unconditionally requires the CUDA-only libcudacxx (cuda/std/version) and CUB
# (cub/version.cuh) include dirs. rocThrust 2.x (ROCm) ships neither -- the HIP
# device system uses hipCUB/rocPRIM instead and needs no libcudacxx -- so the
# stock module fails to find an otherwise valid rocThrust. This override locates
# only thrust/version.h (from the HIP runtime include dirs) and exposes the
# thrust::thrust interface target without the CUDA-only requirements.
#
# Placed on CMAKE_MODULE_PATH ahead of stdgpu's own cmake dir by
# cupoch's core/full third-party setup, so it is selected by stdgpu's
# `find_package(thrust ... MODULE)`. NVIDIA builds never see this file.

find_package(hip QUIET)

find_path(THRUST_INCLUDE_DIR
          HINTS
          ${hip_INCLUDE_DIRS}
          /opt/rocm/include
          NAMES
          "thrust/version.h")

if(THRUST_INCLUDE_DIR)
    file(STRINGS "${THRUST_INCLUDE_DIR}/thrust/version.h"
         THRUST_VERSION_STRING
         REGEX "#define THRUST_VERSION[ \t]+([0-9x]+)")
    string(REGEX REPLACE "#define THRUST_VERSION[ \t]+([0-9]+).*" "\\1" THRUST_VERSION_STRING ${THRUST_VERSION_STRING})
    math(EXPR THRUST_VERSION_MAJOR "${THRUST_VERSION_STRING} / 100000")
    math(EXPR THRUST_VERSION_MINOR "(${THRUST_VERSION_STRING} / 100) % 1000")
    math(EXPR THRUST_VERSION_PATCH "${THRUST_VERSION_STRING} % 100")
    set(THRUST_VERSION "${THRUST_VERSION_MAJOR}.${THRUST_VERSION_MINOR}.${THRUST_VERSION_PATCH}")
    unset(THRUST_VERSION_STRING)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(thrust
                                  REQUIRED_VARS THRUST_INCLUDE_DIR
                                  VERSION_VAR THRUST_VERSION)

if(thrust_FOUND)
    set(THRUST_INCLUDE_DIRS "${THRUST_INCLUDE_DIR}")
    if(NOT TARGET thrust::thrust)
        add_library(thrust::thrust INTERFACE IMPORTED)
        set_target_properties(thrust::thrust PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${THRUST_INCLUDE_DIRS}")
    endif()
    mark_as_advanced(THRUST_INCLUDE_DIR THRUST_INCLUDE_DIRS THRUST_VERSION
                     THRUST_VERSION_MAJOR THRUST_VERSION_MINOR THRUST_VERSION_PATCH)
endif()
