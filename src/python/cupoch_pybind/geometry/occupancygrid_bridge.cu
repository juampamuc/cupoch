#include "cupoch/geometry/occupancygrid.h"
#include "cupoch_pybind/device_vector_wrapper.h"

using namespace cupoch;

extern "C" {

wrapper::device_vector_occupancyvoxel *cupoch_occupancygrid_get_voxels(
        const geometry::OccupancyGrid *occupancygrid) {
    return new wrapper::device_vector_occupancyvoxel(
            *occupancygrid->ExtractKnownVoxels());
}

}  // extern "C"
