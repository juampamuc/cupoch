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
#include "cupoch/geometry/graph.h"

#include "cupoch_pybind/docstring.h"
#include "cupoch_pybind/dlpack_utils.h"
#include "cupoch_pybind/geometry/geometry.h"
#include "cupoch_pybind/geometry/geometry_trampoline.h"

using namespace cupoch;

extern "C" {
void cupoch_lineset3_set_points(cupoch::geometry::LineSet<3>*,
    const cupoch::wrapper::device_vector_wrapper<Eigen::Vector3f>*);
void cupoch_lineset2_set_points(cupoch::geometry::LineSet<2>*,
    const cupoch::wrapper::device_vector_wrapper<Eigen::Matrix<float,2,1>>*);
cupoch::wrapper::device_vector_vector2i*
    cupoch_lineset3_get_lines(cupoch::geometry::LineSet<3>*);
void cupoch_lineset3_set_lines(cupoch::geometry::LineSet<3>*,
    const cupoch::wrapper::device_vector_vector2i*);
cupoch::wrapper::device_vector_vector2i*
    cupoch_lineset2_get_lines(cupoch::geometry::LineSet<2>*);
void cupoch_lineset2_set_lines(cupoch::geometry::LineSet<2>*,
    const cupoch::wrapper::device_vector_vector2i*);
void* cupoch_lineset3_lines_to_dlpack(cupoch::geometry::LineSet<3>*);
void cupoch_lineset3_lines_from_dlpack(cupoch::geometry::LineSet<3>*,
                                       const DLManagedTensor*);
void* cupoch_lineset2_lines_to_dlpack(cupoch::geometry::LineSet<2>*);
void cupoch_lineset2_lines_from_dlpack(cupoch::geometry::LineSet<2>*,
                                       const DLManagedTensor*);
cupoch::wrapper::device_vector_float *cupoch_graph3_get_edge_weights(
        cupoch::geometry::Graph<3> *graph);
void cupoch_graph3_set_edge_weights(
        cupoch::geometry::Graph<3> *graph,
        const cupoch::wrapper::device_vector_float *edge_weights);
cupoch::wrapper::device_vector_float *cupoch_graph2_get_edge_weights(
        cupoch::geometry::Graph<2> *graph);
void cupoch_graph2_set_edge_weights(
        cupoch::geometry::Graph<2> *graph,
        const cupoch::wrapper::device_vector_float *edge_weights);
}

namespace {

template <int Dim>
struct GraphBridge;

template <>
struct GraphBridge<3> {
    static void SetPoints(geometry::Graph<3> &graph,
                          const std::vector<Eigen::Vector3f> &points) {
        wrapper::device_vector_vector3f points_wrapper(
                points.data(), static_cast<int>(points.size()));
        cupoch_lineset3_set_points(&graph, &points_wrapper);
    }

    static wrapper::device_vector_vector2i GetEdges(geometry::Graph<3> &graph) {
        std::unique_ptr<wrapper::device_vector_vector2i> edges(
                cupoch_lineset3_get_lines(&graph));
        return std::move(*edges);
    }

    static void SetEdges(geometry::Graph<3> &graph,
                         const wrapper::device_vector_vector2i &edges) {
        cupoch_lineset3_set_lines(&graph, &edges);
    }

    static wrapper::device_vector_float GetEdgeWeights(
            geometry::Graph<3> &graph) {
        std::unique_ptr<wrapper::device_vector_float> weights(
                cupoch_graph3_get_edge_weights(&graph));
        return std::move(*weights);
    }

    static void SetEdgeWeights(geometry::Graph<3> &graph,
                               const wrapper::device_vector_float &weights) {
        cupoch_graph3_set_edge_weights(&graph, &weights);
    }

    static py::capsule ToEdgesDLpack(geometry::Graph<3> &graph) {
        return pybind::MakeDLpackCapsule(
                cupoch_lineset3_lines_to_dlpack(&graph));
    }

    static void FromEdgesDLpack(geometry::Graph<3> &graph,
                                py::capsule dlpack) {
        cupoch_lineset3_lines_from_dlpack(
                &graph, pybind::GetDLManagedTensor(dlpack));
    }
};

template <>
struct GraphBridge<2> {
    static void SetPoints(geometry::Graph<2> &graph,
                          const std::vector<Eigen::Vector2f> &points) {
        wrapper::device_vector_vector2f points_wrapper(
                points.data(), static_cast<int>(points.size()));
        cupoch_lineset2_set_points(&graph, &points_wrapper);
    }

    static wrapper::device_vector_vector2i GetEdges(geometry::Graph<2> &graph) {
        std::unique_ptr<wrapper::device_vector_vector2i> edges(
                cupoch_lineset2_get_lines(&graph));
        return std::move(*edges);
    }

    static void SetEdges(geometry::Graph<2> &graph,
                         const wrapper::device_vector_vector2i &edges) {
        cupoch_lineset2_set_lines(&graph, &edges);
    }

    static wrapper::device_vector_float GetEdgeWeights(
            geometry::Graph<2> &graph) {
        std::unique_ptr<wrapper::device_vector_float> weights(
                cupoch_graph2_get_edge_weights(&graph));
        return std::move(*weights);
    }

    static void SetEdgeWeights(geometry::Graph<2> &graph,
                               const wrapper::device_vector_float &weights) {
        cupoch_graph2_set_edge_weights(&graph, &weights);
    }

    static py::capsule ToEdgesDLpack(geometry::Graph<2> &graph) {
        return pybind::MakeDLpackCapsule(
                cupoch_lineset2_lines_to_dlpack(&graph));
    }

    static void FromEdgesDLpack(geometry::Graph<2> &graph,
                                py::capsule dlpack) {
        cupoch_lineset2_lines_from_dlpack(
                &graph, pybind::GetDLManagedTensor(dlpack));
    }
};

template <class GraphT, int Dim>
void bind_def(GraphT& graph) {
    py::detail::bind_default_constructor<geometry::Graph<Dim>>(graph);
    py::detail::bind_copy_functions<geometry::Graph<Dim>>(graph);
    graph.def(py::init([](const std::vector<Eigen::Matrix<float, Dim, 1>> &points) {
                auto out = std::unique_ptr<geometry::Graph<Dim>>(
                        new geometry::Graph<Dim>());
                GraphBridge<Dim>::SetPoints(*out, points);
                return out;
              }),
              "Create a Graph from given nodes and edges", "points"_a)
            .def("construct_graph", &geometry::Graph<Dim>::ConstructGraph,
                 "Construct graph structure, when given nodes and edges.",
                 "set_edge_weights_from_distance"_a = true)
            .def("add_edge", &geometry::Graph<Dim>::AddEdge,
                 "Add an edge to the graph", "edge"_a, "weight"_a = 1.0,
                 "lazy_add"_a = false)
            .def("add_edges",
                 py::overload_cast<const std::vector<Eigen::Vector2i> &,
                                   const std::vector<float> &, bool>(
                         &geometry::Graph<Dim>::AddEdges),
                 "Add edges to the graph", "edges"_a,
                 "weights"_a = std::vector<float>(),
                 "lazy_add"_a = false)
            .def("remove_edge", &geometry::Graph<Dim>::RemoveEdge,
                 "Remove an edge from the graph", "edge"_a)
            .def("remove_edges",
                 py::overload_cast<const std::vector<Eigen::Vector2i> &>(
                         &geometry::Graph<Dim>::RemoveEdges),
                 "Remove edges from the graph", "edges"_a)
            .def("paint_edge_color", &geometry::Graph<Dim>::PaintEdgeColor,
                 "Paint an edge with the color", "edge"_a, "color"_a)
            .def("paint_edges_color",
                 py::overload_cast<const std::vector<Eigen::Vector2i> &,
                                   const Eigen::Vector3f &>(
                         &geometry::Graph<Dim>::PaintEdgesColor),
                 "Paint edges with the color", "edges"_a, "color"_a)
            .def("paint_node_color", &geometry::Graph<Dim>::PaintNodeColor,
                 "Paint a node with the color", "node"_a, "color"_a)
            .def("paint_nodes_color",
                 py::overload_cast<const std::vector<int> &,
                                   const Eigen::Vector3f &>(
                         &geometry::Graph<Dim>::PaintNodesColor),
                 "Paint nodes with the color", "nodes"_a, "color"_a)
            .def("set_edge_weights_from_distance",
                 &geometry::Graph<Dim>::SetEdgeWeightsFromDistance)
            .def("dijkstra_path",
                 [](const geometry::Graph<Dim> &graph, int start_node,
                    int end_node) {
                     auto res = graph.DijkstraPath(start_node, end_node);
                     return std::make_pair(*res.first, res.second);
                 })
            .def_static("create_from_triangle_mesh",
                        &geometry::Graph<Dim>::template CreateFromTriangleMesh<Dim>,
                        "Function to make graph from a TriangleMesh", "input"_a)
            .def_static(
                    "create_from_axis_aligned_bounding_box",
                    py::overload_cast<const geometry::AxisAlignedBoundingBox<Dim> &,
                                      const Eigen::Matrix<int, Dim, 1> &>(
                            &geometry::Graph<
                                    Dim>::CreateFromAxisAlignedBoundingBox),
                    "Function to make graph from a AlignedBoundingBox",
                    "input"_a, "resolutions"_a)
            .def_static("create_from_axis_aligned_bounding_box",
                        py::overload_cast<const Eigen::Matrix<float, Dim, 1> &,
                                          const Eigen::Matrix<float, Dim, 1> &,
                                          const Eigen::Matrix<int, Dim, 1> &>(
                                &geometry::Graph<
                                        Dim>::CreateFromAxisAlignedBoundingBox),
                        "Function to make graph from a AlignedBoundingBox",
                        "min_bound"_a, "max_bound"_a, "resolutions"_a)
            .def_property(
                    "edges",
                    [](geometry::Graph<Dim> &graph) {
                        return GraphBridge<Dim>::GetEdges(graph);
                    },
                    [](geometry::Graph<Dim> &graph,
                       const wrapper::device_vector_vector2i &vec) {
                        GraphBridge<Dim>::SetEdges(graph, vec);
                    })
            .def_property(
                    "edge_weights",
                    [](geometry::Graph<Dim> &graph) {
                        return GraphBridge<Dim>::GetEdgeWeights(graph);
                    },
                    [](geometry::Graph<Dim> &graph,
                       const wrapper::device_vector_float &vec) {
                        GraphBridge<Dim>::SetEdgeWeights(graph, vec);
                    })
            .def("to_edges_dlpack",
                 [](geometry::Graph<Dim> &graph) {
                     return GraphBridge<Dim>::ToEdgesDLpack(graph);
                 })
            .def("from_edges_dlpack",
                 [](geometry::Graph<Dim> &graph, py::capsule dlpack) {
                     GraphBridge<Dim>::FromEdgesDLpack(graph, dlpack);
                 });
}

}


void pybind_graph(py::module &m) {
    py::class_<geometry::Graph<3>, PyGeometry3D<geometry::Graph<3>>,
               std::shared_ptr<geometry::Graph<3>>, geometry::LineSet<3>>
            graph3d(m, "Graph", "Graph define a sets of nodes and edges in 3D.");
    bind_def<decltype(graph3d), 3>(graph3d);

    py::class_<geometry::Graph<2>, PyGeometry2D<geometry::Graph<2>>,
               std::shared_ptr<geometry::Graph<2>>, geometry::LineSet<2>>
            graph2d(m, "Graph2D", "Graph define a sets of nodes and edges in 2D.");
    bind_def<decltype(graph2d), 2>(graph2d);
}
