#include "cupoch/collision/collision.h"
#include "cupoch_pybind/device_vector_wrapper.h"

using namespace cupoch;

extern "C" {

wrapper::device_vector_size_t *cupoch_collision_result_get_first_indices(
        const collision::CollisionResult *result) {
    return new wrapper::device_vector_size_t(result->GetFirstCollisionIndices());
}

wrapper::device_vector_size_t *cupoch_collision_result_get_second_indices(
        const collision::CollisionResult *result) {
    return new wrapper::device_vector_size_t(
            result->GetSecondCollisionIndices());
}

wrapper::device_vector_vector2i *cupoch_collision_result_get_pairs(
        collision::CollisionResult *result) {
    return new wrapper::device_vector_vector2i(result->collision_index_pairs_);
}

void cupoch_collision_result_set_pairs(
        collision::CollisionResult *result,
        const wrapper::device_vector_vector2i *pairs) {
    wrapper::FromWrapper(result->collision_index_pairs_, *pairs);
}

collision::CollisionResult *cupoch_collision_compute_intersection_primitives_voxel(
        const wrapper::device_vector_primitives *primitives,
        const geometry::VoxelGrid *voxelgrid,
        float margin) {
    auto result =
            collision::ComputeIntersection(primitives->data_, *voxelgrid, margin);
    return new collision::CollisionResult(*result);
}

collision::CollisionResult *cupoch_collision_compute_intersection_voxel_primitives(
        const geometry::VoxelGrid *voxelgrid,
        const wrapper::device_vector_primitives *primitives,
        float margin) {
    auto result =
            collision::ComputeIntersection(*voxelgrid, primitives->data_, margin);
    return new collision::CollisionResult(*result);
}

collision::CollisionResult *
cupoch_collision_compute_intersection_primitives_occupancy(
        const wrapper::device_vector_primitives *primitives,
        const geometry::OccupancyGrid *occupancygrid,
        float margin) {
    auto result = collision::ComputeIntersection(primitives->data_,
                                                 *occupancygrid, margin);
    return new collision::CollisionResult(*result);
}

collision::CollisionResult *
cupoch_collision_compute_intersection_occupancy_primitives(
        const geometry::OccupancyGrid *occupancygrid,
        const wrapper::device_vector_primitives *primitives,
        float margin) {
    auto result = collision::ComputeIntersection(*occupancygrid,
                                                 primitives->data_, margin);
    return new collision::CollisionResult(*result);
}

}  // extern "C"
