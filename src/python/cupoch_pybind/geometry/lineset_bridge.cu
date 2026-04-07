#include "cupoch/geometry/lineset.h"
#include "cupoch/geometry/pointcloud.h"
#include "cupoch/utility/dl_converter.h"
#include "cupoch_pybind/device_vector_wrapper.h"

using cupoch::geometry::LineSet;
using cupoch::geometry::PointCloud;
using cupoch::wrapper::device_vector_size_t;
using cupoch::wrapper::device_vector_vector2i;
using cupoch::wrapper::device_vector_vector3f;
template<int Dim>
using device_vector_points_t = cupoch::wrapper::device_vector_wrapper<Eigen::Matrix<float, Dim, 1>>;

extern "C" {
// ---- Getter/Setter: points, lines, colors ----
device_vector_points_t<3>* cupoch_lineset3_get_points(LineSet<3>* ls) {
    return new device_vector_points_t<3>(ls->points_);
}
void cupoch_lineset3_set_points(LineSet<3>* ls, const device_vector_points_t<3>* w) {
    cupoch::wrapper::FromWrapper(ls->points_, *w);
}
device_vector_vector2i* cupoch_lineset3_get_lines(LineSet<3>* ls) {
    return new device_vector_vector2i(ls->lines_);
}
void cupoch_lineset3_set_lines(LineSet<3>* ls, const device_vector_vector2i* w) {
    cupoch::wrapper::FromWrapper(ls->lines_, *w);
}
device_vector_vector3f* cupoch_lineset3_get_colors(LineSet<3>* ls) {
    return new device_vector_vector3f(ls->colors_);
}
void cupoch_lineset3_set_colors(LineSet<3>* ls, const device_vector_vector3f* w) {
    cupoch::wrapper::FromWrapper(ls->colors_, *w);
}

// 2D
device_vector_points_t<2>* cupoch_lineset2_get_points(LineSet<2>* ls) {
    return new device_vector_points_t<2>(ls->points_);
}
void cupoch_lineset2_set_points(LineSet<2>* ls, const device_vector_points_t<2>* w) {
    cupoch::wrapper::FromWrapper(ls->points_, *w);
}
device_vector_vector2i* cupoch_lineset2_get_lines(LineSet<2>* ls) {
    return new device_vector_vector2i(ls->lines_);
}
void cupoch_lineset2_set_lines(LineSet<2>* ls, const device_vector_vector2i* w) {
    cupoch::wrapper::FromWrapper(ls->lines_, *w);
}
device_vector_vector3f* cupoch_lineset2_get_colors(LineSet<2>* ls) {
    return new device_vector_vector3f(ls->colors_);
}
void cupoch_lineset2_set_colors(LineSet<2>* ls, const device_vector_vector3f* w) {
    cupoch::wrapper::FromWrapper(ls->colors_, *w);
}

// Constructor
LineSet<3>* cupoch_lineset3_create_from_wrappers(
        const device_vector_points_t<3>* points,
        const device_vector_vector2i* lines) {
    return new LineSet<3>(points->data_, lines->data_);
}
LineSet<2>* cupoch_lineset2_create_from_wrappers(
        const device_vector_points_t<2>* points,
        const device_vector_vector2i* lines) {
    return new LineSet<2>(points->data_, lines->data_);
}

// Methods
void cupoch_lineset3_paint_indexed_color(
        LineSet<3>* self, const device_vector_size_t* indices, const Eigen::Vector3f* color) {
    self->PaintIndexedColor(indices->data_, *color);
}
void cupoch_lineset2_paint_indexed_color(
        LineSet<2>* self, const device_vector_size_t* indices, const Eigen::Vector3f* color) {
    self->PaintIndexedColor(indices->data_, *color);
}

// Factory: correspondences
void cupoch_lineset3_assign_from_correspondences(
        LineSet<3>* out,
        const PointCloud* c0, const PointCloud* c1,
        const cupoch::wrapper::device_vector_wrapper<thrust::pair<int, int>>* corrs) {
    auto sp = LineSet<3>::CreateFromPointCloudCorrespondences<3>(*c0, *c1, corrs->data_);
    *out = std::move(*sp);
}
void cupoch_lineset2_assign_from_correspondences(
        LineSet<2>* out,
        const PointCloud* c0, const PointCloud* c1,
        const cupoch::wrapper::device_vector_wrapper<thrust::pair<int, int>>* corrs) {
    auto sp = LineSet<2>::CreateFromPointCloudCorrespondences<2>(*c0, *c1, corrs->data_);
    *out = std::move(*sp);
}

void* cupoch_lineset3_lines_to_dlpack(LineSet<3>* ls) {
    return cupoch::utility::ToDLPack(ls->lines_);
}
void cupoch_lineset3_lines_from_dlpack(LineSet<3>* ls,
                                       const DLManagedTensor* managed) {
    cupoch::utility::FromDLPack<int, 2>(managed, ls->lines_);
}
void* cupoch_lineset2_lines_to_dlpack(LineSet<2>* ls) {
    return cupoch::utility::ToDLPack(ls->lines_);
}
void cupoch_lineset2_lines_from_dlpack(LineSet<2>* ls,
                                       const DLManagedTensor* managed) {
    cupoch::utility::FromDLPack<int, 2>(managed, ls->lines_);
}
}
