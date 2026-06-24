# build
file(GLOB_RECURSE ALL_CUDA_SOURCE_FILES "*.cu")

# create object library
cuda_add_library(cupoch_knn ${ALL_CUDA_SOURCE_FILES})
target_link_libraries(cupoch_knn cupoch_utility)

if (USE_HIP)
    # lbvh/lbvh_index (header-only, used by lbvh_knn.cu) include CUDA-only
    # <vector_functions.h>/<vector_types.h>/<cuda_runtime.h>; a compat dir
    # (BEFORE + PRIVATE) maps them to the HIP runtime/vector-type headers.
    target_include_directories(cupoch_knn BEFORE PRIVATE
                               ${CMAKE_SOURCE_DIR}/cmake/lbvh_hip_compat)
endif ()