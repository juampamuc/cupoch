#include "cupoch/geometry/laserscanbuffer.h"
#include "cupoch_pybind/device_vector_wrapper.h"

using namespace cupoch;

namespace {

wrapper::device_vector_float MakeDeviceVectorFloat(const float *data,
                                                   size_t size) {
    if (size == 0) {
        return wrapper::device_vector_float();
    }
    return wrapper::device_vector_float(data, static_cast<int>(size));
}

}  // namespace

extern "C" {

void cupoch_laserscanbuffer_add_ranges_device(
        geometry::LaserScanBuffer *scan,
        const wrapper::device_vector_float *ranges,
        const Eigen::Matrix4f *transformation,
        const wrapper::device_vector_float *intensities) {
    if (intensities != nullptr) {
        scan->AddRanges(ranges->data_, *transformation, intensities->data_);
        return;
    }
    utility::device_vector<float> empty;
    scan->AddRanges(ranges->data_, *transformation, empty);
}

void cupoch_laserscanbuffer_add_ranges_host(
        geometry::LaserScanBuffer *scan,
        const float *ranges,
        size_t num_ranges,
        const Eigen::Matrix4f *transformation,
        const float *intensities,
        size_t num_intensities) {
    auto range_vec = MakeDeviceVectorFloat(ranges, num_ranges);
    auto intensity_vec = MakeDeviceVectorFloat(intensities, num_intensities);
    scan->AddRanges(range_vec.data_, *transformation, intensity_vec.data_);
}

wrapper::device_vector_float *cupoch_laserscanbuffer_get_ranges(
        geometry::LaserScanBuffer *scan) {
    return new wrapper::device_vector_float(scan->ranges_);
}

wrapper::device_vector_float *cupoch_laserscanbuffer_get_intensities(
        geometry::LaserScanBuffer *scan) {
    return new wrapper::device_vector_float(scan->intensities_);
}

}  // extern "C"
