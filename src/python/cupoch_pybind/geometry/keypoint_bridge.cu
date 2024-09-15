#include "cupoch/geometry/keypoint.h"
#include "cupoch/geometry/pointcloud.h"
#include "cupoch_pybind/device_vector_wrapper.h"

using namespace cupoch;

extern "C" {

void cupoch_compute_iss_keypoints_bridge(
        const geometry::PointCloud *input,
        float salient_radius,
        float non_max_radius,
        float gamma_21,
        float gamma_32,
        int min_neighbors,
        int max_neighbors,
        geometry::PointCloud **keypoints,
        wrapper::device_vector_bool **mask) {
    auto out = geometry::keypoint::ComputeISSKeypoints(
            *input, salient_radius, non_max_radius, gamma_21, gamma_32,
            min_neighbors, max_neighbors);
    *keypoints = new geometry::PointCloud(*std::get<0>(out));
    *mask = new wrapper::device_vector_bool(*std::get<1>(out));
}

}  // extern "C"
