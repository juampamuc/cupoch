#pragma once

#include <Python.h>
#include <dlpack/dlpack.h>
#include <pybind11/pybind11.h>

namespace cupoch {
namespace pybind {

inline pybind11::capsule MakeDLpackCapsule(void *managed_tensor) {
    return pybind11::capsule(managed_tensor, "dltensor", [](::PyObject *obj) {
        auto *ptr = ::PyCapsule_GetPointer(obj, "dltensor");
        if (ptr != nullptr) {
            auto *managed = static_cast<::DLManagedTensor *>(ptr);
            managed->deleter(managed);
        } else {
            ::PyErr_Clear();
        }
    });
}

inline const DLManagedTensor *GetDLManagedTensor(
        const pybind11::capsule &dlpack) {
    return static_cast<const DLManagedTensor *>(
            ::PyCapsule_GetPointer(dlpack.ptr(), "dltensor"));
}

}  // namespace pybind
}  // namespace cupoch
