#include "cupoch/registration/feature.h"
#include "cupoch_pybind/device_vector_wrapper.h"

using namespace cupoch;

extern "C" {

wrapper::device_vector_vector33f *cupoch_feature33_get_data(
        registration::Feature<33> *feature) {
    return new wrapper::device_vector_vector33f(feature->data_);
}

void cupoch_feature33_set_data(
        registration::Feature<33> *feature,
        const wrapper::device_vector_vector33f *data) {
    wrapper::FromWrapper(feature->data_, *data);
}

wrapper::device_vector_vector352f *cupoch_feature352_get_data(
        registration::Feature<352> *feature) {
    return new wrapper::device_vector_vector352f(feature->data_);
}

void cupoch_feature352_set_data(
        registration::Feature<352> *feature,
        const wrapper::device_vector_vector352f *data) {
    wrapper::FromWrapper(feature->data_, *data);
}

}  // extern "C"
