#include "common.h"
#include <SPlisHSPlasH/MagneticForce/MagneticForceBase.h>
#include <SPlisHSPlasH/MagneticForce/MagneticForce_Huang2019.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

void MagneticForceModule(py::module m_sub)
{
    // ---------------------------------------
    // Class Magnetic Force Base
    // ---------------------------------------
    py::class_<SPH::MagneticForceBase, SPH::NonPressureForceBase>(m_sub, "MagneticForceBase")
            .def_readwrite_static("SUSCEPTIBILITY", &SPH::MagneticForceBase::SUSCEPTIBILITY)
            .def_readwrite_static("EXTERNAL_FIELD", &SPH::MagneticForceBase::EXTERNAL_FIELD);

    // ---------------------------------------
    // Class Magnetic Force Huang 2019
    // ---------------------------------------
    py::class_<SPH::MagneticForce_Huang2019, SPH::MagneticForceBase>(m_sub, "MagneticForce_Huang2019")
            .def(py::init<SPH::FluidModel*>());

}