#include "cupoch/geometry/pointcloud.h"
#include "cupoch/utility/dl_converter.h"
#include "cupoch_pybind/device_vector_wrapper.h"

using namespace cupoch;

extern "C" {

wrapper::device_vector_vector3f *cupoch_pointcloud_get_points(
        geometry::PointCloud *pointcloud) {
    return new wrapper::device_vector_vector3f(pointcloud->points_);
}

void cupoch_pointcloud_set_points(
        geometry::PointCloud *pointcloud,
        const wrapper::device_vector_vector3f *points) {
    wrapper::FromWrapper(pointcloud->points_, *points);
}

wrapper::device_vector_vector3f *cupoch_pointcloud_get_normals(
        geometry::PointCloud *pointcloud) {
    return new wrapper::device_vector_vector3f(pointcloud->normals_);
}

void cupoch_pointcloud_set_normals(
        geometry::PointCloud *pointcloud,
        const wrapper::device_vector_vector3f *normals) {
    wrapper::FromWrapper(pointcloud->normals_, *normals);
}

wrapper::device_vector_vector3f *cupoch_pointcloud_get_colors(
        geometry::PointCloud *pointcloud) {
    return new wrapper::device_vector_vector3f(pointcloud->colors_);
}

void cupoch_pointcloud_set_colors(
        geometry::PointCloud *pointcloud,
        const wrapper::device_vector_vector3f *colors) {
    wrapper::FromWrapper(pointcloud->colors_, *colors);
}

geometry::PointCloud *cupoch_pointcloud_select_by_index(
        const geometry::PointCloud *pointcloud,
        const wrapper::device_vector_size_t *indices,
        bool invert) {
    auto out = pointcloud->SelectByIndex(indices->data_, invert);
    return new geometry::PointCloud(*out);
}

geometry::PointCloud *cupoch_pointcloud_select_by_mask(
        const geometry::PointCloud *pointcloud,
        const wrapper::device_vector_bool *mask,
        bool invert) {
    auto out = pointcloud->SelectByMask(mask->data_, invert);
    return new geometry::PointCloud(*out);
}

void cupoch_pointcloud_remove_radius_outlier(
        const geometry::PointCloud *pointcloud,
        size_t nb_points,
        float radius,
        geometry::PointCloud **out_pointcloud,
        wrapper::device_vector_size_t **out_indices) {
    auto out = pointcloud->RemoveRadiusOutliers(nb_points, radius);
    *out_pointcloud = new geometry::PointCloud(*std::get<0>(out));
    *out_indices =
            new wrapper::device_vector_size_t(std::move(std::get<1>(out)));
}

void cupoch_pointcloud_remove_statistical_outlier(
        const geometry::PointCloud *pointcloud,
        size_t nb_neighbors,
        float std_ratio,
        geometry::PointCloud **out_pointcloud,
        wrapper::device_vector_size_t **out_indices) {
    auto out =
            pointcloud->RemoveStatisticalOutliers(nb_neighbors, std_ratio);
    *out_pointcloud = new geometry::PointCloud(*std::get<0>(out));
    *out_indices =
            new wrapper::device_vector_size_t(std::move(std::get<1>(out)));
}

wrapper::device_vector_int *cupoch_pointcloud_cluster_dbscan(
        const geometry::PointCloud *pointcloud,
        float eps,
        size_t min_points,
        bool print_progress,
        size_t max_edges) {
    auto out = pointcloud->ClusterDBSCAN(eps, min_points, print_progress,
                                         max_edges);
    return new wrapper::device_vector_int(std::move(*out));
}

wrapper::device_vector_size_t *cupoch_pointcloud_segment_plane(
        const geometry::PointCloud *pointcloud,
        float distance_threshold,
        int ransac_n,
        int num_iterations,
        Eigen::Vector4f *plane_model) {
    auto out = pointcloud->SegmentPlane(distance_threshold, ransac_n,
                                        num_iterations);
    *plane_model = std::get<0>(out);
    return new wrapper::device_vector_size_t(std::move(std::get<1>(out)));
}

void *cupoch_pointcloud_points_to_dlpack(geometry::PointCloud *pointcloud) {
    return utility::ToDLPack(pointcloud->points_);
}

void cupoch_pointcloud_points_from_dlpack(geometry::PointCloud *pointcloud,
                                          const DLManagedTensor *managed) {
    utility::FromDLPack<float, 3>(managed, pointcloud->points_);
}

void *cupoch_pointcloud_normals_to_dlpack(geometry::PointCloud *pointcloud) {
    return utility::ToDLPack(pointcloud->normals_);
}

void cupoch_pointcloud_normals_from_dlpack(geometry::PointCloud *pointcloud,
                                           const DLManagedTensor *managed) {
    utility::FromDLPack<float, 3>(managed, pointcloud->normals_);
}

void *cupoch_pointcloud_colors_to_dlpack(geometry::PointCloud *pointcloud) {
    return utility::ToDLPack(pointcloud->colors_);
}

void cupoch_pointcloud_colors_from_dlpack(geometry::PointCloud *pointcloud,
                                          const DLManagedTensor *managed) {
    utility::FromDLPack<float, 3>(managed, pointcloud->colors_);
}

}  // extern "C"
