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
#include "cupoch/geometry/lineset.h"

#include "cupoch/geometry/pointcloud.h"
#include "cupoch_pybind/docstring.h"
#include "cupoch_pybind/dlpack_utils.h"
#include "cupoch_pybind/geometry/geometry.h"
#include "cupoch_pybind/geometry/geometry_trampoline.h"

using namespace cupoch;
extern "C" {
  // 3D
  cupoch::wrapper::device_vector_wrapper<Eigen::Vector3f>*
    cupoch_lineset3_get_points(cupoch::geometry::LineSet<3>*);
  void cupoch_lineset3_set_points(cupoch::geometry::LineSet<3>*,
    const cupoch::wrapper::device_vector_wrapper<Eigen::Vector3f>*);
  cupoch::wrapper::device_vector_vector2i*
    cupoch_lineset3_get_lines(cupoch::geometry::LineSet<3>*);
  void cupoch_lineset3_set_lines(cupoch::geometry::LineSet<3>*,
    const cupoch::wrapper::device_vector_vector2i*);
  cupoch::wrapper::device_vector_vector3f*
    cupoch_lineset3_get_colors(cupoch::geometry::LineSet<3>*);
  void cupoch_lineset3_set_colors(cupoch::geometry::LineSet<3>*,
    const cupoch::wrapper::device_vector_vector3f*);
  cupoch::geometry::LineSet<3>*
    cupoch_lineset3_create_from_wrappers(
      const cupoch::wrapper::device_vector_wrapper<Eigen::Vector3f>*,
      const cupoch::wrapper::device_vector_vector2i*);
  void cupoch_lineset3_paint_indexed_color(
    cupoch::geometry::LineSet<3>*,
    const cupoch::wrapper::device_vector_size_t*,
    const Eigen::Vector3f*);
  void cupoch_lineset3_assign_from_correspondences(
    cupoch::geometry::LineSet<3>* out,
    const cupoch::geometry::PointCloud* c0,
    const cupoch::geometry::PointCloud* c1,
    const cupoch::wrapper::device_vector_wrapper<thrust::pair<int, int>>* corrs);

  // 2D
  cupoch::wrapper::device_vector_wrapper<Eigen::Matrix<float,2,1>>*
    cupoch_lineset2_get_points(cupoch::geometry::LineSet<2>*);
  void cupoch_lineset2_set_points(cupoch::geometry::LineSet<2>*,
    const cupoch::wrapper::device_vector_wrapper<Eigen::Matrix<float,2,1>>*);
  cupoch::wrapper::device_vector_vector2i*
    cupoch_lineset2_get_lines(cupoch::geometry::LineSet<2>*);
  void cupoch_lineset2_set_lines(cupoch::geometry::LineSet<2>*,
    const cupoch::wrapper::device_vector_vector2i*);
  cupoch::wrapper::device_vector_vector3f*
    cupoch_lineset2_get_colors(cupoch::geometry::LineSet<2>*);
  void cupoch_lineset2_set_colors(cupoch::geometry::LineSet<2>*,
    const cupoch::wrapper::device_vector_vector3f*);
  cupoch::geometry::LineSet<2>*
    cupoch_lineset2_create_from_wrappers(
      const cupoch::wrapper::device_vector_wrapper<Eigen::Matrix<float,2,1>>*,
      const cupoch::wrapper::device_vector_vector2i*);
  void cupoch_lineset2_paint_indexed_color(
    cupoch::geometry::LineSet<2>*,
    const cupoch::wrapper::device_vector_size_t*,
    const Eigen::Vector3f*);
  void cupoch_lineset2_assign_from_correspondences(
    cupoch::geometry::LineSet<2>* out,
    const cupoch::geometry::PointCloud* c0,
    const cupoch::geometry::PointCloud* c1,
    const cupoch::wrapper::device_vector_wrapper<thrust::pair<int, int>>* corrs);
  void* cupoch_lineset3_lines_to_dlpack(cupoch::geometry::LineSet<3>*);
  void cupoch_lineset3_lines_from_dlpack(cupoch::geometry::LineSet<3>*,
    const DLManagedTensor*);
  void* cupoch_lineset2_lines_to_dlpack(cupoch::geometry::LineSet<2>*);
  void cupoch_lineset2_lines_from_dlpack(cupoch::geometry::LineSet<2>*,
    const DLManagedTensor*);
}

namespace {

template <int Dim>
struct LineSetDLPackBridge;

template <>
struct LineSetDLPackBridge<3> {
    static py::capsule ToLinesDLpack(geometry::LineSet<3> &line) {
        return pybind::MakeDLpackCapsule(
                cupoch_lineset3_lines_to_dlpack(&line));
    }

    static void FromLinesDLpack(geometry::LineSet<3> &line,
                                py::capsule dlpack) {
        cupoch_lineset3_lines_from_dlpack(
                &line, pybind::GetDLManagedTensor(dlpack));
    }
};

template <>
struct LineSetDLPackBridge<2> {
    static py::capsule ToLinesDLpack(geometry::LineSet<2> &line) {
        return pybind::MakeDLpackCapsule(
                cupoch_lineset2_lines_to_dlpack(&line));
    }

    static void FromLinesDLpack(geometry::LineSet<2> &line,
                                py::capsule dlpack) {
        cupoch_lineset2_lines_from_dlpack(
                &line, pybind::GetDLManagedTensor(dlpack));
    }
};

template <class LineSetT, int Dim>
void bind_def(LineSetT& lineset) {
    py::detail::bind_default_constructor<geometry::LineSet<Dim>>(lineset);
    py::detail::bind_copy_functions<geometry::LineSet<Dim>>(lineset);
    lineset.def(py::init<const std::vector<Eigen::Matrix<float, Dim, 1>> &,
                         const std::vector<Eigen::Vector2i> &>(),
                "Create a LineSet from given points and line indices",
                "points"_a, "lines"_a)
            .def(py::init([](const wrapper::device_vector_wrapper<Eigen::Matrix<float, Dim, 1>> &points,
                             const wrapper::device_vector_vector2i &lines) {
                     if constexpr (Dim == 3) {
                         return std::unique_ptr<geometry::LineSet<Dim>>(
                             cupoch_lineset3_create_from_wrappers(&points, &lines));
                     } else {
                         return std::unique_ptr<geometry::LineSet<Dim>>(
                             cupoch_lineset2_create_from_wrappers(&points, &lines));
                     }
                 }))
            .def(py::init<const std::vector<Eigen::Matrix<float, Dim, 1>>>(),
                 "Create a LineSet from given path",
                 "path"_a)
            .def("__repr__",
                 [](const geometry::LineSet<Dim> &lineset) {
                     return std::string("geometry::LineSet with ") +
                            std::to_string(lineset.lines_.size()) + " lines.";
                 })
            .def("has_points", &geometry::LineSet<Dim>::HasPoints,
                 "Returns ``True`` if the object contains points.")
            .def("has_lines", &geometry::LineSet<Dim>::HasLines,
                 "Returns ``True`` if the object contains lines.")
            .def("has_colors", &geometry::LineSet<Dim>::HasColors,
                 "Returns ``True`` if the object's lines contain contain "
                 "colors.")
            .def("get_line_coordinate",
                 &geometry::LineSet<Dim>::GetLineCoordinate, "line_index"_a)
            .def("paint_uniform_color",
                 &geometry::LineSet<Dim>::PaintUniformColor,
                 "Assigns each line in the line set the same color.")
            .def("paint_indexed_color",
                 [] (geometry::LineSet<Dim>& self, const wrapper::device_vector_size_t& indices, const Eigen::Vector3f& color) {
                     if constexpr (Dim == 3) {
                         cupoch_lineset3_paint_indexed_color(&self, &indices, &color);
                     } else {
                         cupoch_lineset2_paint_indexed_color(&self, &indices, &color);
                     }
                 })
            .def_static(
                "create_from_point_cloud_correspondences",
                [] (const geometry::PointCloud& cloud0, const geometry::PointCloud& cloud1, const wrapper::device_vector_wrapper<thrust::pair<int, int>>& correspondences) {
                    auto out = std::make_shared<geometry::LineSet<Dim>>();
                    if constexpr (Dim == 3) {
                        cupoch_lineset3_assign_from_correspondences(out.get(), &cloud0, &cloud1, &correspondences);
                    } else {
                        cupoch_lineset2_assign_from_correspondences(out.get(), &cloud0, &cloud1, &correspondences);
                    }
                    return out;
                })
            .def_static("create_from_oriented_bounding_box",
                        &geometry::LineSet<Dim>::template CreateFromOrientedBoundingBox<Dim>,
                        "Factory function to create a LineSet from an "
                        "OrientedBoundingBox.",
                        "box"_a)
            .def_static("create_from_axis_aligned_bounding_box",
                        &geometry::LineSet<Dim>::template CreateFromAxisAlignedBoundingBox<Dim>,
                        "Factory function to create a LineSet from an "
                        "AxisAlignedBoundingBox.",
                        "box"_a)
            .def_static("create_from_triangle_mesh",
                        &geometry::LineSet<Dim>::template CreateFromTriangleMesh<Dim>,
                        "Factory function to create a LineSet from edges of a "
                        "triangle mesh.",
                        "mesh"_a)
            .def_static("create_camera_marker",
                        &geometry::LineSet<Dim>::template CreateCameraMarker<Dim>,
                        "Factory function to create a LineSet from camera parameter",
                        "intrinsic"_a, "extrinsic"_a, "marker_size"_a = 0.3)
            .def_property(
                    "points",
                    [](geometry::LineSet<Dim> &line) {
                        if constexpr (Dim == 3) {
                            std::unique_ptr<wrapper::device_vector_wrapper<Eigen::Matrix<float, Dim, 1>>>
                                tmp(cupoch_lineset3_get_points(&line));
                            return std::move(*tmp);
                        } else {
                            std::unique_ptr<wrapper::device_vector_wrapper<Eigen::Matrix<float, Dim, 1>>>
                                tmp(cupoch_lineset2_get_points(&line));
                            return std::move(*tmp);
                        }
                    },
                    [](geometry::LineSet<Dim> &line,
                       const wrapper::device_vector_wrapper<Eigen::Matrix<float, Dim, 1>> &vec) {
                        if constexpr (Dim == 3) cupoch_lineset3_set_points(&line, &vec);
                        else                    cupoch_lineset2_set_points(&line, &vec);
                    })
            .def_property(
                    "lines",
                    [](geometry::LineSet<Dim> &line) {
                        if constexpr (Dim == 3) {
                            std::unique_ptr<wrapper::device_vector_vector2i> tmp(cupoch_lineset3_get_lines(&line));
                            return std::move(*tmp);
                        } else {
                            std::unique_ptr<wrapper::device_vector_vector2i> tmp(cupoch_lineset2_get_lines(&line));
                            return std::move(*tmp);
                        }
                    },
                    [](geometry::LineSet<Dim> &line, const wrapper::device_vector_vector2i &vec) {
                        if constexpr (Dim == 3) cupoch_lineset3_set_lines(&line, &vec);
                        else                    cupoch_lineset2_set_lines(&line, &vec);
                    })
            .def_property(
                    "colors",
                    [](geometry::LineSet<Dim> &line) {
                        if constexpr (Dim == 3) {
                            std::unique_ptr<wrapper::device_vector_vector3f> tmp(cupoch_lineset3_get_colors(&line));
                            return std::move(*tmp);
                        } else {
                            std::unique_ptr<wrapper::device_vector_vector3f> tmp(cupoch_lineset2_get_colors(&line));
                            return std::move(*tmp);
                        }
                    },
                    [](geometry::LineSet<Dim> &line, const wrapper::device_vector_vector3f &vec) {
                        if constexpr (Dim == 3) cupoch_lineset3_set_colors(&line, &vec);
                        else                    cupoch_lineset2_set_colors(&line, &vec);
                    })
            .def("to_lines_dlpack",
                 [](geometry::LineSet<Dim> &line) {
                     return LineSetDLPackBridge<Dim>::ToLinesDLpack(line);
                 })
            .def("from_lines_dlpack",
                 [](geometry::LineSet<Dim> &line, py::capsule dlpack) {
                     LineSetDLPackBridge<Dim>::FromLinesDLpack(line, dlpack);
                 });
}

void doc_inject(py::module &m, const std::string& name) {
    docstring::ClassMethodDocInject(m, name, "has_colors");
    docstring::ClassMethodDocInject(m, name, "has_lines");
    docstring::ClassMethodDocInject(m, name, "has_points");
    docstring::ClassMethodDocInject(m, name, "get_line_coordinate",
                                    {{"line_index", "Index of the line."}});
    docstring::ClassMethodDocInject(m, name, "paint_uniform_color",
                                    {{"color", "Color for the LineSet."}});
    docstring::ClassMethodDocInject(
            m, name, "create_from_point_cloud_correspondences",
            {{"cloud0", "First point cloud."},
             {"cloud1", "Second point cloud."},
             {"correspondences", "Set of correspondences."}});
    docstring::ClassMethodDocInject(m, name,
                                    "create_from_oriented_bounding_box",
                                    {{"box", "The input bounding box."}});
    docstring::ClassMethodDocInject(m, name,
                                    "create_from_axis_aligned_bounding_box",
                                    {{"box", "The input bounding box."}});
    docstring::ClassMethodDocInject(m, name, "create_from_triangle_mesh",
                                    {{"mesh", "The input triangle mesh."}});
}

}

void pybind_lineset(py::module &m) {
    py::class_<geometry::LineSet<3>, PyGeometry3D<geometry::LineSet<3>>,
               std::shared_ptr<geometry::LineSet<3>>, geometry::GeometryBase3D>
            lineset(m, "LineSet",
                    "LineSet define a sets of lines in 3D. A typical "
                    "application is to display the point cloud correspondence "
                    "pairs.");
    bind_def<decltype(lineset), 3>(lineset);
    doc_inject(m, "LineSet");

    py::class_<geometry::LineSet<2>, PyGeometry2D<geometry::LineSet<2>>,
               std::shared_ptr<geometry::LineSet<2>>, geometry::GeometryBase2D>
            lineset2d(m, "LineSet2D",
                      "LineSet define a sets of lines in 2D. A typical "
                      "application is to display the point cloud correspondence "
                      "pairs.");
    bind_def<decltype(lineset2d), 2>(lineset2d);
    doc_inject(m, "LineSet2D");
}

void pybind_lineset_methods(py::module &m) {}
