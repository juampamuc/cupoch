#include "cupoch/utility/platform.h"
#include "cupoch_pybind/device_vector_wrapper.h"

namespace {

template <typename Type>
void DeviceVectorIAddHost(cupoch::wrapper::device_vector_wrapper<Type> *self,
                          const Type *host_data,
                          size_t size) {
    cupoch::utility::device_vector<Type> other(size);
    if (size > 0) {
        cudaSafeCall(cudaMemcpy(thrust::raw_pointer_cast(other.data()),
                                host_data, size * sizeof(Type),
                                cudaMemcpyHostToDevice));
    }
    thrust::transform(self->data_.begin(), self->data_.end(), other.begin(),
                      self->data_.begin(), thrust::plus<Type>());
}

}  // namespace

extern "C" {

void cupoch_device_vector_int_iadd_host(
        cupoch::wrapper::device_vector_int *self,
        const int *host_data,
        size_t size) {
    DeviceVectorIAddHost(self, host_data, size);
}

void cupoch_device_vector_ulong_iadd_host(
        cupoch::wrapper::device_vector_wrapper<unsigned long> *self,
        const unsigned long *host_data,
        size_t size) {
    DeviceVectorIAddHost(self, host_data, size);
}

void cupoch_device_vector_float_iadd_host(
        cupoch::wrapper::device_vector_float *self,
        const float *host_data,
        size_t size) {
    DeviceVectorIAddHost(self, host_data, size);
}

void cupoch_device_vector_vector2f_iadd_host(
        cupoch::wrapper::device_vector_vector2f *self,
        const Eigen::Vector2f *host_data,
        size_t size) {
    DeviceVectorIAddHost(self, host_data, size);
}

void cupoch_device_vector_vector3f_iadd_host(
        cupoch::wrapper::device_vector_vector3f *self,
        const Eigen::Vector3f *host_data,
        size_t size) {
    DeviceVectorIAddHost(self, host_data, size);
}

void cupoch_device_vector_vector2i_iadd_host(
        cupoch::wrapper::device_vector_vector2i *self,
        const Eigen::Vector2i *host_data,
        size_t size) {
    DeviceVectorIAddHost(self, host_data, size);
}

void cupoch_device_vector_vector3i_iadd_host(
        cupoch::wrapper::device_vector_vector3i *self,
        const Eigen::Vector3i *host_data,
        size_t size) {
    DeviceVectorIAddHost(self, host_data, size);
}

}  // extern "C"
