/**
 * Copyright (c) 2020 Neka-Nat
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
**/
#include "cupoch/geometry/pointcloud.h"

#include "cupoch/camera/pinhole_camera_intrinsic.h"
#include "cupoch/geometry/rgbdimage.h"
#include "cupoch/geometry/laserscanbuffer.h"
#include "cupoch_pybind/dlpack_utils.h"
#include "cupoch_pybind/docstring.h"
#include "cupoch_pybind/geometry/geometry.h"
#include "cupoch_pybind/geometry/geometry_trampoline.h"

using namespace cupoch;

extern "C" {
cupoch::wrapper::device_vector_vector3f *cupoch_pointcloud_get_points(
        cupoch::geometry::PointCloud *pointcloud);
void cupoch_pointcloud_set_points(
        cupoch::geometry::PointCloud *pointcloud,
        const cupoch::wrapper::device_vector_vector3f *points);
cupoch::wrapper::device_vector_vector3f *cupoch_pointcloud_get_normals(
        cupoch::geometry::PointCloud *pointcloud);
void cupoch_pointcloud_set_normals(
        cupoch::geometry::PointCloud *pointcloud,
        const cupoch::wrapper::device_vector_vector3f *normals);
cupoch::wrapper::device_vector_vector3f *cupoch_pointcloud_get_colors(
        cupoch::geometry::PointCloud *pointcloud);
void cupoch_pointcloud_set_colors(
        cupoch::geometry::PointCloud *pointcloud,
        const cupoch::wrapper::device_vector_vector3f *colors);
cupoch::geometry::PointCloud *cupoch_pointcloud_select_by_index(
        const cupoch::geometry::PointCloud *pointcloud,
        const cupoch::wrapper::device_vector_size_t *indices,
        bool invert);
cupoch::geometry::PointCloud *cupoch_pointcloud_select_by_mask(
        const cupoch::geometry::PointCloud *pointcloud,
        const cupoch::wrapper::device_vector_bool *mask,
        bool invert);
void cupoch_pointcloud_remove_radius_outlier(
        const cupoch::geometry::PointCloud *pointcloud,
        size_t nb_points,
        float radius,
        cupoch::geometry::PointCloud **out_pointcloud,
        cupoch::wrapper::device_vector_size_t **out_indices);
void cupoch_pointcloud_remove_statistical_outlier(
        const cupoch::geometry::PointCloud *pointcloud,
        size_t nb_neighbors,
        float std_ratio,
        cupoch::geometry::PointCloud **out_pointcloud,
        cupoch::wrapper::device_vector_size_t **out_indices);
cupoch::wrapper::device_vector_int *cupoch_pointcloud_cluster_dbscan(
        const cupoch::geometry::PointCloud *pointcloud,
        float eps,
        size_t min_points,
        bool print_progress,
        size_t max_edges);
cupoch::wrapper::device_vector_size_t *cupoch_pointcloud_segment_plane(
        const cupoch::geometry::PointCloud *pointcloud,
        float distance_threshold,
        int ransac_n,
        int num_iterations,
        Eigen::Vector4f *plane_model);
void *cupoch_pointcloud_points_to_dlpack(
        cupoch::geometry::PointCloud *pointcloud);
void cupoch_pointcloud_points_from_dlpack(
        cupoch::geometry::PointCloud *pointcloud,
        const DLManagedTensor *managed);
void *cupoch_pointcloud_normals_to_dlpack(
        cupoch::geometry::PointCloud *pointcloud);
void cupoch_pointcloud_normals_from_dlpack(
        cupoch::geometry::PointCloud *pointcloud,
        const DLManagedTensor *managed);
void *cupoch_pointcloud_colors_to_dlpack(
        cupoch::geometry::PointCloud *pointcloud);
void cupoch_pointcloud_colors_from_dlpack(
        cupoch::geometry::PointCloud *pointcloud,
        const DLManagedTensor *managed);
}

void pybind_pointcloud(py::module &m) {
    py::class_<geometry::PointCloud, PyGeometry3D<geometry::PointCloud>,
               std::shared_ptr<geometry::PointCloud>, geometry::GeometryBase3D>
            pointcloud(m, "PointCloud",
                       "PointCloud class. A point cloud consists of point "
                       "coordinates, and optionally point colors and point "
                       "normals.");
    py::detail::bind_default_constructor<geometry::PointCloud>(pointcloud);
    py::detail::bind_copy_functions<geometry::PointCloud>(pointcloud);
    pointcloud
            .def(py::init<const std::vector<Eigen::Vector3f> &>(),
                 "Create a PointCloud from points", "points"_a)
            .def(py::init([](const wrapper::device_vector_vector3f &points) {
                     auto pointcloud = std::unique_ptr<geometry::PointCloud>(
                             new geometry::PointCloud());
                     cupoch_pointcloud_set_points(pointcloud.get(), &points);
                     return pointcloud;
                 }),
                 "Create a PointCloud from points", "points"_a)
            .def("__repr__",
                 [](const geometry::PointCloud &pcd) {
                     return std::string("geometry::PointCloud with ") +
                            std::to_string(pcd.points_.size()) + " points.";
                 })
            .def_property(
                    "points",
                    [](geometry::PointCloud &pcd) {
                        std::unique_ptr<wrapper::device_vector_vector3f> points(
                                cupoch_pointcloud_get_points(&pcd));
                        return std::move(*points);
                    },
                    [](geometry::PointCloud &pcd,
                       const wrapper::device_vector_vector3f &vec) {
                        cupoch_pointcloud_set_points(&pcd, &vec);
                    })
            .def_property(
                    "normals",
                    [](geometry::PointCloud &pcd) {
                        std::unique_ptr<wrapper::device_vector_vector3f> normals(
                                cupoch_pointcloud_get_normals(&pcd));
                        return std::move(*normals);
                    },
                    [](geometry::PointCloud &pcd,
                       const wrapper::device_vector_vector3f &vec) {
                        cupoch_pointcloud_set_normals(&pcd, &vec);
                    })
            .def_property(
                    "colors",
                    [](geometry::PointCloud &pcd) {
                        std::unique_ptr<wrapper::device_vector_vector3f> colors(
                                cupoch_pointcloud_get_colors(&pcd));
                        return std::move(*colors);
                    },
                    [](geometry::PointCloud &pcd,
                       const wrapper::device_vector_vector3f &vec) {
                        cupoch_pointcloud_set_colors(&pcd, &vec);
                    })
            .def("to_points_dlpack",
                 [](geometry::PointCloud &pcd) {
                     return pybind::MakeDLpackCapsule(
                             cupoch_pointcloud_points_to_dlpack(&pcd));
                 })
            .def("to_normals_dlpack",
                 [](geometry::PointCloud &pcd) {
                     return pybind::MakeDLpackCapsule(
                             cupoch_pointcloud_normals_to_dlpack(&pcd));
                 })
            .def("to_colors_dlpack",
                 [](geometry::PointCloud &pcd) {
                     return pybind::MakeDLpackCapsule(
                             cupoch_pointcloud_colors_to_dlpack(&pcd));
                 })
            .def("from_points_dlpack",
                 [](geometry::PointCloud &pcd, py::capsule dlpack) {
                     cupoch_pointcloud_points_from_dlpack(
                             &pcd, pybind::GetDLManagedTensor(dlpack));
                 })
            .def("from_normals_dlpack",
                 [](geometry::PointCloud &pcd, py::capsule dlpack) {
                     cupoch_pointcloud_normals_from_dlpack(
                             &pcd, pybind::GetDLManagedTensor(dlpack));
                 })
            .def("from_colors_dlpack",
                 [](geometry::PointCloud &pcd, py::capsule dlpack) {
                     cupoch_pointcloud_colors_from_dlpack(
                             &pcd, pybind::GetDLManagedTensor(dlpack));
                 })
            .def(py::self + py::self)
            .def(py::self += py::self)
            .def("has_points", &geometry::PointCloud::HasPoints,
                 "Returns ``True`` if the point cloud contains points.")
            .def("has_normals", &geometry::PointCloud::HasNormals,
                 "Returns ``True`` if the point cloud contains point normals.")
            .def("has_colors", &geometry::PointCloud::HasColors,
                 "Returns ``True`` if the point cloud contains point colors.")
            .def("normalize_normals", &geometry::PointCloud::NormalizeNormals,
                 "Normalize point normals to length 1.")
            .def("transform", &geometry::PointCloud::Transform,
                 "Apply transformation (4x4 matrix) to the geometry "
                 "coordinates.")
            .def("get_oriented_bounding_box",
                 &geometry::PointCloud::GetOrientedBoundingBox,
                 "Returns an oriented bounding box of the pointcloud.")
            .def("paint_uniform_color",
                 &geometry::PointCloud::PaintUniformColor, "color"_a,
                 "Assigns each point in the PointCloud the same color.")
            .def(
                    "select_by_index",
                    [](const geometry::PointCloud &pcd,
                       const wrapper::device_vector_size_t &index,
                       bool invert) {
                        return std::shared_ptr<geometry::PointCloud>(
                                cupoch_pointcloud_select_by_index(
                                        &pcd, &index, invert));
                    },
                    "Function to select points from input pointcloud into "
                    "output "
                    "pointcloud. ``indices``: "
                    "Indices of points to be selected. ``invert``: Set to "
                    "``True`` to "
                    "invert the selection of indices.",
                    "indices"_a, "invert"_a = false)
            .def(
                    "select_by_mask",
                    [](const geometry::PointCloud &pcd,
                       const wrapper::device_vector_bool &mask,
                       bool invert) {
                        return std::shared_ptr<geometry::PointCloud>(
                                cupoch_pointcloud_select_by_mask(
                                        &pcd, &mask, invert));
                    },
                    "Function to select points from input pointcloud into "
                    "output "
                    "pointcloud. ``mask``: "
                    "Masks of points to be selected. ``invert``: Set to "
                    "``True`` to "
                    "invert the selection of masks.",
                    "mask"_a, "invert"_a = false)
            .def("voxel_down_sample", &geometry::PointCloud::VoxelDownSample,
                 "Function to downsample input pointcloud into output "
                 "pointcloud with "
                 "a voxel",
                 "voxel_size"_a)
            .def("uniform_down_sample",
                 &geometry::PointCloud::UniformDownSample,
                 "Function to downsample input pointcloud into output "
                 "pointcloud "
                 "uniformly. The sample is performed in the order of the "
                 "points with "
                 "the 0-th point always chosen, not at random.",
                 "every_k_points"_a)
            .def("gaussian_filter", &geometry::PointCloud::GaussianFilter,
                 "Function to apply Gaussian Filter to input pointcloud",
                 "search_radius"_a, "sigma2"_a, "num_max_search_points"_a = 50)
            .def("pass_through_filter", &geometry::PointCloud::PassThroughFilter,
                 "Function to apply Pass Through Filter to input pointcloud",
                 "axis_no"_a, "min_bound"_a, "max_bound"_a)
            .def("crop",
                 (std::shared_ptr<geometry::PointCloud>(
                         geometry::PointCloud::*)(
                         const geometry::AxisAlignedBoundingBox<3> &) const) &
                         geometry::PointCloud::Crop,
                 "Function to crop input pointcloud into output pointcloud",
                 "bounding_box"_a)
            .def("crop",
                 (std::shared_ptr<geometry::PointCloud>(
                         geometry::PointCloud::*)(
                         const geometry::OrientedBoundingBox &) const) &
                         geometry::PointCloud::Crop,
                 "Function to crop input pointcloud into output pointcloud",
                 "bounding_box"_a)
            .def("remove_none_finite_points",
                 &geometry::PointCloud::RemoveNoneFinitePoints,
                 "Function to remove none-finite points from the PointCloud",
                 "remove_nan"_a = true, "remove_infinite"_a = true)
            .def(
                    "remove_radius_outlier",
                    [](const geometry::PointCloud &pcd, size_t nb_points,
                       float search_radius) {
                        geometry::PointCloud *filtered = nullptr;
                        wrapper::device_vector_size_t *indices = nullptr;
                        cupoch_pointcloud_remove_radius_outlier(
                                &pcd, nb_points, search_radius, &filtered,
                                &indices);
                        return std::make_tuple(
                                std::shared_ptr<geometry::PointCloud>(filtered),
                                std::move(*std::unique_ptr<
                                          wrapper::device_vector_size_t>(
                                        indices)));
                    },
                    "Function to remove points that have less than nb_points"
                    " in a given sphere of a given radius",
                    "nb_points"_a, "radius"_a)
            .def(
                    "remove_statistical_outlier",
                    [](const geometry::PointCloud &pcd, size_t nb_neighbors,
                       float std_ratio) {
                        geometry::PointCloud *filtered = nullptr;
                        wrapper::device_vector_size_t *indices = nullptr;
                        cupoch_pointcloud_remove_statistical_outlier(
                                &pcd, nb_neighbors, std_ratio, &filtered,
                                &indices);
                        return std::make_tuple(
                                std::shared_ptr<geometry::PointCloud>(filtered),
                                std::move(*std::unique_ptr<
                                          wrapper::device_vector_size_t>(
                                        indices)));
                    },
                    "Function to remove points that are further away from "
                    "their "
                    "neighbors in average",
                    "nb_neighbors"_a, "std_ratio"_a)
            .def("estimate_normals", &geometry::PointCloud::EstimateNormals,
                 "Function to compute the normals of a point cloud. Normals "
                 "are oriented with respect to the input point cloud if "
                 "normals exist",
                 "search_param"_a = knn::KDTreeSearchParamKNN())
            .def("orient_normals_to_align_with_direction",
                 &geometry::PointCloud::OrientNormalsToAlignWithDirection,
                 "Function to orient the normals of a point cloud",
                 "orientation_reference"_a = Eigen::Vector3f(0.0, 0.0, 1.0))
            .def(
                    "cluster_dbscan",
                    [](const geometry::PointCloud &pcd, float eps,
                       size_t min_points, bool print_progress,
                       size_t max_edges) {
                        std::unique_ptr<wrapper::device_vector_int> labels(
                                cupoch_pointcloud_cluster_dbscan(
                                        &pcd, eps, min_points, print_progress,
                                        max_edges));
                        return std::move(*labels);
                    },
                    "Cluster PointCloud using the DBSCAN algorithm  Ester et "
                    "al., "
                    "'A Density-Based Algorithm for Discovering Clusters in "
                    "Large "
                    "Spatial Databases with Noise', 1996. Returns a list of "
                    "point "
                    "labels, -1 indicates noise according to the algorithm.",
                    "eps"_a, "min_points"_a, "print_progress"_a = false,
                    "max_edges"_a = knn::NUM_MAX_NN)
            .def("segment_plane",
                 [](const geometry::PointCloud &pcd,
                    float distance_threshold,
                    int ransac_n,
                    int num_iterations) {
                     Eigen::Vector4f plane_model;
                     std::unique_ptr<wrapper::device_vector_size_t> indices(
                             cupoch_pointcloud_segment_plane(
                                     &pcd, distance_threshold, ransac_n,
                                     num_iterations, &plane_model));
                     return std::make_tuple(plane_model, std::move(*indices));
                 },
                 "Segments a plane in the point cloud using the RANSAC "
                 "algorithm.",
                 "distance_threshold"_a, "ransac_n"_a, "num_iterations"_a)
            .def_static(
                    "create_from_depth_image",
                    &geometry::PointCloud::CreateFromDepthImage,
                    R"(Factory function to create a pointcloud from a depth image and a
        camera. Given depth value d at (u, v) image coordinate, the corresponding 3d
        point is:
              - z = d / depth_scale
              - x = (u - cx) * z / fx
              - y = (v - cy) * z / fy
        )",
                    "depth"_a, "intrinsic"_a,
                    "extrinsic"_a = Eigen::Matrix4f::Identity(),
                    "depth_scale"_a = 1000.0, "depth_trunc"_a = 1000.0,
                    "stride"_a = 1)
            .def_static(
                    "create_from_rgbd_image",
                    &geometry::PointCloud::CreateFromRGBDImage,
                    R"(Factory function to create a pointcloud from an RGB-D image and a
        camera. Given depth value d at (u, v) image coordinate, the corresponding 3d
        point is:
              - z = d / depth_scale
              - x = (u - cx) * z / fx
              - y = (v - cy) * z / fy
        )",
                    "image"_a, "intrinsic"_a,
                    "extrinsic"_a = Eigen::Matrix4f::Identity(),
                    "project_valid_depth_only"_a = true,
                    "depth_cutoff"_a = -1.0f,
                    "compute_normals"_a = false)
            .def_static(
                    "create_from_laserscanbuffer",
                    &geometry::PointCloud::CreateFromLaserScanBuffer,
                    "Factory function to create a pointcloud from an laser scan and a LiDAR.",
                    "scan"_a, "min_range"_a, "max_range"_a)
            .def_static(
                    "create_from_occupancygrid",
                    &geometry::PointCloud::CreateFromOccupancyGrid,
                    "Factory function to create a pointcloud from Occupancy Grid.")
            .def_static(
                    "create_from_disparity",
                    &geometry::PointCloud::CreateFromDisparity,
                    "Factory function to create a pointcloud from a disparity image.");
    docstring::ClassMethodDocInject(m, "PointCloud", "has_colors");
    docstring::ClassMethodDocInject(m, "PointCloud", "has_normals");
    docstring::ClassMethodDocInject(m, "PointCloud", "has_points");
    docstring::ClassMethodDocInject(m, "PointCloud", "normalize_normals");
    docstring::ClassMethodDocInject(
            m, "PointCloud", "paint_uniform_color",
            {{"color", "RGB color for the PointCloud."}});
    docstring::ClassMethodDocInject(
            m, "PointCloud", "select_by_index",
            {{"indices", "Indices of points to be selected."},
             {"invert",
              "Set to ``True`` to invert the selection of indices."}});
    docstring::ClassMethodDocInject(
            m, "PointCloud", "voxel_down_sample",
            {{"voxel_size", "Voxel size to downsample into."},
             {"invert", "set to ``True`` to invert the selection of indices"}});
    docstring::ClassMethodDocInject(
            m, "PointCloud", "uniform_down_sample",
            {{"every_k_points",
              "Sample rate, the selected point indices are [0, k, 2k, ...]"}});
    docstring::ClassMethodDocInject(
            m, "PointCloud", "remove_none_finite_points",
            {{"remove_nan", "Remove NaN values from the PointCloud"},
             {"remove_infinite",
              "Remove infinite values from the PointCloud"}});
    docstring::ClassMethodDocInject(
            m, "PointCloud", "remove_radius_outlier",
            {{"nb_points", "Number of points within the radius."},
             {"radius", "Radius of the sphere."}});
    docstring::ClassMethodDocInject(
            m, "PointCloud", "remove_statistical_outlier",
            {{"nb_neighbors", "Number of neighbors around the target point."},
             {"std_ratio", "Standard deviation ratio."}});
    docstring::ClassMethodDocInject(
            m, "PointCloud", "estimate_normals",
            {{"search_param",
              "The KDTree search parameters for neighborhood search."},
             {"fast_normal_computation",
              "If true, the normal estiamtion uses a non-iterative method to "
              "extract the eigenvector from the covariance matrix. This is "
              "faster, but is not as numerical stable."}});
    docstring::ClassMethodDocInject(
            m, "PointCloud", "orient_normals_to_align_with_direction",
            {{"orientation_reference",
              "Normals are oriented with respect to orientation_reference."}});
    docstring::ClassMethodDocInject(
            m, "PointCloud", "cluster_dbscan",
            {{"eps",
              "Density parameter that is used to find neighbouring points."},
             {"min_points", "Minimum number of points to form a cluster."},
             {"print_progress",
              "If true the progress is visualized in the console."}});
    docstring::ClassMethodDocInject(
            m, "PointCloud", "create_from_depth_image",
            {
                    {"project_valid_depth_only",
                     "If this value is True, return point cloud, which doesn't "
                     "have nan point. If this value is False, return point "
                     "cloud, which has whole points"},
            });
    docstring::ClassMethodDocInject(
            m, "PointCloud", "create_from_rgbd_image",
            {
                    {"project_valid_depth_only",
                     "If this value is True, return point cloud, which doesn't "
                     "have nan point. If this value is False, return point "
                     "cloud, which has whole points"},
            });
}
