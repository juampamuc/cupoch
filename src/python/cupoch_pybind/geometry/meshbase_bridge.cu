#include "cupoch/geometry/meshbase.h"
#include "cupoch/utility/dl_converter.h"
#include "cupoch_pybind/device_vector_wrapper.h"

using namespace cupoch;

extern "C" {

wrapper::device_vector_vector3f *cupoch_meshbase_get_vertices(
        geometry::MeshBase *mesh) {
    return new wrapper::device_vector_vector3f(mesh->vertices_);
}

void cupoch_meshbase_set_vertices(
        geometry::MeshBase *mesh,
        const wrapper::device_vector_vector3f *vertices) {
    wrapper::FromWrapper(mesh->vertices_, *vertices);
}

wrapper::device_vector_vector3f *cupoch_meshbase_get_vertex_normals(
        geometry::MeshBase *mesh) {
    return new wrapper::device_vector_vector3f(mesh->vertex_normals_);
}

void cupoch_meshbase_set_vertex_normals(
        geometry::MeshBase *mesh,
        const wrapper::device_vector_vector3f *normals) {
    wrapper::FromWrapper(mesh->vertex_normals_, *normals);
}

wrapper::device_vector_vector3f *cupoch_meshbase_get_vertex_colors(
        geometry::MeshBase *mesh) {
    return new wrapper::device_vector_vector3f(mesh->vertex_colors_);
}

void cupoch_meshbase_set_vertex_colors(
        geometry::MeshBase *mesh,
        const wrapper::device_vector_vector3f *colors) {
    wrapper::FromWrapper(mesh->vertex_colors_, *colors);
}

void *cupoch_meshbase_vertices_to_dlpack(geometry::MeshBase *mesh) {
    return utility::ToDLPack(mesh->vertices_);
}

void cupoch_meshbase_vertices_from_dlpack(geometry::MeshBase *mesh,
                                          const DLManagedTensor *managed) {
    utility::FromDLPack<float, 3>(managed, mesh->vertices_);
}

void *cupoch_meshbase_vertex_normals_to_dlpack(geometry::MeshBase *mesh) {
    return utility::ToDLPack(mesh->vertex_normals_);
}

void cupoch_meshbase_vertex_normals_from_dlpack(
        geometry::MeshBase *mesh, const DLManagedTensor *managed) {
    utility::FromDLPack<float, 3>(managed, mesh->vertex_normals_);
}

void *cupoch_meshbase_vertex_colors_to_dlpack(geometry::MeshBase *mesh) {
    return utility::ToDLPack(mesh->vertex_colors_);
}

void cupoch_meshbase_vertex_colors_from_dlpack(
        geometry::MeshBase *mesh, const DLManagedTensor *managed) {
    utility::FromDLPack<float, 3>(managed, mesh->vertex_colors_);
}

}  // extern "C"
