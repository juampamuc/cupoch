#include "cupoch/geometry/graph.h"
#include "cupoch_pybind/device_vector_wrapper.h"

using namespace cupoch;

extern "C" {

wrapper::device_vector_float *cupoch_graph3_get_edge_weights(
        geometry::Graph<3> *graph) {
    return new wrapper::device_vector_float(graph->edge_weights_);
}

void cupoch_graph3_set_edge_weights(
        geometry::Graph<3> *graph,
        const wrapper::device_vector_float *edge_weights) {
    wrapper::FromWrapper(graph->edge_weights_, *edge_weights);
}

wrapper::device_vector_float *cupoch_graph2_get_edge_weights(
        geometry::Graph<2> *graph) {
    return new wrapper::device_vector_float(graph->edge_weights_);
}

void cupoch_graph2_set_edge_weights(
        geometry::Graph<2> *graph,
        const wrapper::device_vector_float *edge_weights) {
    wrapper::FromWrapper(graph->edge_weights_, *edge_weights);
}

}  // extern "C"
