file(GLOB_RECURSE COLLISION_SOURCE_FILES "*.cu")
cuda_add_library(cupoch_collision ${COLLISION_SOURCE_FILES})
target_link_libraries(cupoch_collision cupoch_geometry
                      ${3RDPARTY_LIBRARIES})

if (USE_HIP)
    # collision.cu includes lbvh (bvh.cuh/query.cuh), which pulls in CUDA-only
    # <vector_types.h>/<vector_functions.h>/<cuda_runtime.h>; the same compat
    # dir knn uses (BEFORE + PRIVATE) maps them onto the HIP runtime/vector-type
    # headers.
    target_include_directories(cupoch_collision BEFORE PRIVATE
                               ${CMAKE_SOURCE_DIR}/cmake/lbvh_hip_compat)
endif ()