# build
file(GLOB_RECURSE ALL_CUDA_SOURCE_FILES "*.cu")

if (USE_HIP)
    # scalable_tsdfvolume.cu stores an 80 KB VolumeUnit<16> (a 16^3 TSDFVoxel
    # block) BY VALUE inside a device stdgpu::unordered_map. OpenVolumeUnitKernel
    # emplaces that value, which materializes the 80 KB pair as a per-work-item
    # stack temporary (~164 KB after the map's internal copies). The AMDGPU
    # backend caps an addressable scratch (stack) frame at ~128 KB -- a hard
    # encoding limit, not a tunable warning -- so this kernel cannot be code-
    # generated for AMD GPUs without redesigning ScalableTSDFVolume to keep voxel
    # blocks out of line (an upstream data-structure change beyond this port).
    # UniformTSDFVolume (the primary dense integrator) is unaffected and builds.
    list(FILTER ALL_CUDA_SOURCE_FILES EXCLUDE REGEX "scalable_tsdfvolume\\.cu$")
endif ()

# create object library
cuda_add_library(cupoch_integration ${ALL_CUDA_SOURCE_FILES})
target_link_libraries(cupoch_integration cupoch_geometry)