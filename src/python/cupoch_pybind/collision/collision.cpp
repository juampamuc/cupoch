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
#include "cupoch_pybind/collision/collision.h"

#include "cupoch/collision/collision.h"
#include "cupoch/geometry/lineset.h"
#include "cupoch/geometry/occupancygrid.h"
#include "cupoch/geometry/voxelgrid.h"
#include "cupoch_pybind/docstring.h"

using namespace cupoch;

extern "C" {
cupoch::wrapper::device_vector_size_t *
cupoch_collision_result_get_first_indices(
        const cupoch::collision::CollisionResult *result);
cupoch::wrapper::device_vector_size_t *
cupoch_collision_result_get_second_indices(
        const cupoch::collision::CollisionResult *result);
cupoch::wrapper::device_vector_vector2i *cupoch_collision_result_get_pairs(
        cupoch::collision::CollisionResult *result);
void cupoch_collision_result_set_pairs(
        cupoch::collision::CollisionResult *result,
        const cupoch::wrapper::device_vector_vector2i *pairs);
cupoch::collision::CollisionResult *
cupoch_collision_compute_intersection_primitives_voxel(
        const cupoch::wrapper::device_vector_primitives *primitives,
        const cupoch::geometry::VoxelGrid *voxelgrid,
        float margin);
cupoch::collision::CollisionResult *
cupoch_collision_compute_intersection_voxel_primitives(
        const cupoch::geometry::VoxelGrid *voxelgrid,
        const cupoch::wrapper::device_vector_primitives *primitives,
        float margin);
cupoch::collision::CollisionResult *
cupoch_collision_compute_intersection_primitives_occupancy(
        const cupoch::wrapper::device_vector_primitives *primitives,
        const cupoch::geometry::OccupancyGrid *occupancygrid,
        float margin);
cupoch::collision::CollisionResult *
cupoch_collision_compute_intersection_occupancy_primitives(
        const cupoch::geometry::OccupancyGrid *occupancygrid,
        const cupoch::wrapper::device_vector_primitives *primitives,
        float margin);
}

void pybind_collision_methods(py::module& m) {
    py::class_<collision::CollisionResult,
               std::shared_ptr<collision::CollisionResult>>
            col_res(m, "CollitionResult", "Collision result class.");
    py::detail::bind_default_constructor<collision::CollisionResult>(col_res);
    py::detail::bind_copy_functions<collision::CollisionResult>(col_res);
    col_res.def("is_collided", &collision::CollisionResult::IsCollided)
            .def("__repr__",
                    [](const collision::CollisionResult &res) {
                        return std::string("collision::CollisionResult with ") +
                               std::to_string(res.collision_index_pairs_.size()) + " collisions.";
                    })
            .def("get_first_collision_indices",
                    [] (const collision::CollisionResult& self) {
                        std::unique_ptr<wrapper::device_vector_size_t> indices(
                                cupoch_collision_result_get_first_indices(&self));
                        return std::move(*indices);
                    })
            .def("get_second_collision_indices",
                    [] (const collision::CollisionResult& self) {
                        std::unique_ptr<wrapper::device_vector_size_t> indices(
                                cupoch_collision_result_get_second_indices(&self));
                        return std::move(*indices);
                    })
            .def_readwrite("first", &collision::CollisionResult::first_)
            .def_readwrite("second", &collision::CollisionResult::second_)
            .def_property(
                    "collision_index_pairs",
                    [](collision::CollisionResult& res) {
                        std::unique_ptr<wrapper::device_vector_vector2i> pairs(
                                cupoch_collision_result_get_pairs(&res));
                        return std::move(*pairs);
                    },
                    [](collision::CollisionResult& res,
                       const wrapper::device_vector_vector2i& vec) {
                        cupoch_collision_result_set_pairs(&res, &vec);
                    });
    py::enum_<collision::CollisionResult::CollisionType> collision_type(
            col_res, "Type", py::arithmetic());
    collision_type
            .value("Unspecified",
                   collision::CollisionResult::CollisionType::Unspecified)
            .value("Primitives",
                   collision::CollisionResult::CollisionType::Primitives)
            .value("VoxelGrid",
                   collision::CollisionResult::CollisionType::VoxelGrid)
            .value("OccupancyGrid",
                   collision::CollisionResult::CollisionType::OccupancyGrid)
            .value("LineSet",
                   collision::CollisionResult::CollisionType::LineSet)
            .export_values();

    m.def("compute_intersection",
          py::overload_cast<const geometry::VoxelGrid&,
                            const geometry::VoxelGrid&, float>(
                  &collision::ComputeIntersection),
          "Compute intersection betweeen VoxelGrid and VoxelGrid.",
          "voxelgrid1"_a, "voxelgrid2"_a, "margin"_a = 0.0f);
    m.def("compute_intersection",
          py::overload_cast<const geometry::VoxelGrid&,
                            const geometry::LineSet<3>&, float>(
                  &collision::ComputeIntersection),
          "Compute intersection betweeen VoxelGrid and LineSet.",
          "voxelgrid"_a, "lineset"_a, "margin"_a = 0.0f);
    m.def("compute_intersection",
          py::overload_cast<const geometry::LineSet<3>&,
                            const geometry::VoxelGrid&, float>(
                  &collision::ComputeIntersection),
          "Compute intersection betweeen LineSet and VoxelGrid.",
          "lineset"_a, "voxelgrid"_a, "margin"_a = 0.0f);
    m.def("compute_intersection",
          py::overload_cast<const geometry::VoxelGrid&,
                            const geometry::OccupancyGrid&, float>(
                  &collision::ComputeIntersection),
          "voxelgrid"_a, "lineset"_a, "margin"_a = 0.0f);
    m.def("compute_intersection",
          py::overload_cast<const geometry::OccupancyGrid&,
                            const geometry::VoxelGrid&, float>(
                  &collision::ComputeIntersection),
          "Compute intersection betweeen OccupancyGrid and VoxelGrid.",
          "occgrid"_a, "voxelgrid"_a, "margin"_a = 0.0f);
    m.def("compute_intersection",
          py::overload_cast<const geometry::LineSet<3>&,
                            const geometry::OccupancyGrid&, float>(
                  &collision::ComputeIntersection),
          "lineset"_a, "occgrid"_a, "margin"_a = 0.0f);
    m.def("compute_intersection",
          py::overload_cast<const geometry::OccupancyGrid&,
                            const geometry::LineSet<3>&, float>(
                  &collision::ComputeIntersection),
          "occgrid"_a, "linset"_a, "margin"_a = 0.0f);
    m.def("compute_intersection",
          [](const wrapper::device_vector_primitives& primitives,
             const geometry::VoxelGrid& voxel, float margin) {
              return std::shared_ptr<collision::CollisionResult>(
                      cupoch_collision_compute_intersection_primitives_voxel(
                              &primitives, &voxel, margin));
          },
          "Compute intersection betweeen Primitives and VoxelGrid.",
          "primitives"_a, "voxelgrid"_a, "margin"_a = 0.0f);
    m.def("compute_intersection",
          [](const geometry::VoxelGrid& voxel,
             const wrapper::device_vector_primitives& primitives,
             float margin) {
              return std::shared_ptr<collision::CollisionResult>(
                      cupoch_collision_compute_intersection_voxel_primitives(
                              &voxel, &primitives, margin));
          },
          "Compute intersection betweeen VoxelGrid and Primitives.",
          "voxelgrid"_a, "primitives"_a, "margin"_a = 0.0f);
    m.def("compute_intersection",
          [](const wrapper::device_vector_primitives& primitives,
             const geometry::OccupancyGrid& occgrid, float margin) {
              return std::shared_ptr<collision::CollisionResult>(
                      cupoch_collision_compute_intersection_primitives_occupancy(
                              &primitives, &occgrid, margin));
          },
          "Compute intersection betweeen Primitives and OccupancyGrid.",
          "primitives"_a, "occgrid"_a, "margin"_a = 0.0f);
    m.def("compute_intersection",
          [](const geometry::OccupancyGrid& occgrid,
             const wrapper::device_vector_primitives& primitives,
             float margin) {
              return std::shared_ptr<collision::CollisionResult>(
                      cupoch_collision_compute_intersection_occupancy_primitives(
                              &occgrid, &primitives, margin));
          },
          "Compute intersection betweeen OccupancyGrid and Primitives.",
          "occgrid"_a, "primitives"_a, "margin"_a = 0.0f);
}

void pybind_collision(py::module& m) {
    py::module m_submodule = m.def_submodule("collision");
    pybind_collision_methods(m_submodule);
    pybind_primitives(m_submodule);
}
