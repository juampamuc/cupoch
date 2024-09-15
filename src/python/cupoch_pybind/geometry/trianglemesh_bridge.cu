#include "cupoch/geometry/trianglemesh.h"
#include "cupoch_pybind/device_vector_wrapper.h"

using namespace cupoch;

extern "C" {

wrapper::device_vector_vector3i *cupoch_trianglemesh_get_triangles(
        geometry::TriangleMesh *mesh) {
    return new wrapper::device_vector_vector3i(mesh->triangles_);
}

void cupoch_trianglemesh_set_triangles(
        geometry::TriangleMesh *mesh,
        const wrapper::device_vector_vector3i *triangles) {
    wrapper::FromWrapper(mesh->triangles_, *triangles);
}

wrapper::device_vector_vector3f *cupoch_trianglemesh_get_triangle_normals(
        geometry::TriangleMesh *mesh) {
    return new wrapper::device_vector_vector3f(mesh->triangle_normals_);
}

void cupoch_trianglemesh_set_triangle_normals(
        geometry::TriangleMesh *mesh,
        const wrapper::device_vector_vector3f *normals) {
    wrapper::FromWrapper(mesh->triangle_normals_, *normals);
}

wrapper::device_vector_vector2i *cupoch_trianglemesh_get_edge_list(
        geometry::TriangleMesh *mesh) {
    return new wrapper::device_vector_vector2i(mesh->edge_list_);
}

void cupoch_trianglemesh_set_edge_list(
        geometry::TriangleMesh *mesh,
        const wrapper::device_vector_vector2i *edge_list) {
    wrapper::FromWrapper(mesh->edge_list_, *edge_list);
}

wrapper::device_vector_vector2f *cupoch_trianglemesh_get_triangle_uvs(
        geometry::TriangleMesh *mesh) {
    return new wrapper::device_vector_vector2f(mesh->triangle_uvs_);
}

void cupoch_trianglemesh_set_triangle_uvs(
        geometry::TriangleMesh *mesh,
        const wrapper::device_vector_vector2f *triangle_uvs) {
    wrapper::FromWrapper(mesh->triangle_uvs_, *triangle_uvs);
}

}  // extern "C"
