#include "cupoch/geometry/voxelgrid.h"
#include "cupoch_pybind/device_map_wrapper.h"
#include "cupoch_pybind/device_vector_wrapper.h"

using namespace cupoch;

extern "C" {

wrapper::VoxelMap *cupoch_voxelgrid_get_voxels(geometry::VoxelGrid *voxelgrid) {
    return new wrapper::VoxelMap(voxelgrid->voxels_keys_,
                                 voxelgrid->voxels_values_);
}

void cupoch_voxelgrid_set_voxels(geometry::VoxelGrid *voxelgrid,
                                 const wrapper::VoxelMap *voxels) {
    wrapper::FromWrapper(voxelgrid->voxels_keys_, voxelgrid->voxels_values_,
                         *voxels);
}

void cupoch_voxelgrid_paint_indexed_color(
        geometry::VoxelGrid *voxelgrid,
        const wrapper::device_vector_size_t *indices,
        const Eigen::Vector3f *color) {
    voxelgrid->PaintIndexedColor(indices->data_, *color);
}

}  // extern "C"
