#include "cupoch/geometry/distancetransform.h"
#include "cupoch_pybind/device_vector_wrapper.h"

using cupoch::geometry::DistanceTransform;
using cupoch::wrapper::device_vector_vector3f;
using cupoch::wrapper::device_vector_float;

extern "C"
device_vector_float* _cupoch_distancetransform_get_distances_bridge(
    const DistanceTransform* self,
    const device_vector_vector3f* points) {
  auto dv = self->GetDistances(points->data_);
  return new device_vector_float(*dv);
}
