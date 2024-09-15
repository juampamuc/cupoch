#include <thrust/device_vector.h>
#include "cupoch/geometry/boundingvolume.h"

#include "cupoch_pybind/device_vector_wrapper.h"

using namespace cupoch;

extern "C" {

wrapper::device_vector_size_t*
cupoch_aabb3_get_point_indices_within_bounding_box(
    const geometry::AxisAlignedBoundingBox<3>* aabb,
    const wrapper::device_vector_wrapper<Eigen::Vector3f>* points)
{
    auto dv = aabb->GetPointIndicesWithinBoundingBox(points->data_);
    return new wrapper::device_vector_size_t(std::move(dv));
}

geometry::AxisAlignedBoundingBox<3>*
cupoch_aabb3_create_from_points(
    const wrapper::device_vector_wrapper<Eigen::Vector3f>* points)
{
    auto box = geometry::AxisAlignedBoundingBox<3>::CreateFromPoints(points->data_);
    return new geometry::AxisAlignedBoundingBox<3>(box);
}

wrapper::device_vector_size_t*
cupoch_obb_get_point_indices_within_bounding_box(
    const geometry::OrientedBoundingBox* obb,
    const wrapper::device_vector_vector3f* points)
{
    auto dv = obb->GetPointIndicesWithinBoundingBox(points->data_);
    return new wrapper::device_vector_size_t(std::move(dv));
}

wrapper::device_vector_size_t*
cupoch_aabb2_get_point_indices_within_bounding_box(
    const geometry::AxisAlignedBoundingBox<2>* aabb,
    const wrapper::device_vector_wrapper<Eigen::Vector2f>* points)
{
    auto dv = aabb->GetPointIndicesWithinBoundingBox(points->data_);
    return new wrapper::device_vector_size_t(std::move(dv));
}

geometry::AxisAlignedBoundingBox<2>*
cupoch_aabb2_create_from_points(
    const wrapper::device_vector_wrapper<Eigen::Vector2f>* points)
{
    auto box = geometry::AxisAlignedBoundingBox<2>::CreateFromPoints(points->data_);
    return new geometry::AxisAlignedBoundingBox<2>(box);
}

} // extern "C"
