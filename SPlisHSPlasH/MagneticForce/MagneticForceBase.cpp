#include "MagneticForceBase.h"
#include <iostream>

using namespace SPH;
using namespace GenParam;

int MagneticForceBase::SUSCEPTIBILITY = -1;
int MagneticForceBase::EXTERNAL_FIELD = -1;


MagneticForceBase::MagneticForceBase(FluidModel *model) :
        NonPressureForceBase(model)
{
    m_susceptibility = 1.2;
    m_ext_field = Vector3r(0,13000,0);
}

MagneticForceBase::~MagneticForceBase(void)
{
}

void MagneticForceBase::initParameters()
{
    NonPressureForceBase::initParameters();
    SUSCEPTIBILITY = createNumericParameter("susceptibility", "Magnetic susceptibility", &m_susceptibility);
    setGroup(SUSCEPTIBILITY, "Magnetic Force");
    setDescription(SUSCEPTIBILITY, "Susceptibility of the material");
    RealParameter* rparam = static_cast<RealParameter*>(getParameter(SUSCEPTIBILITY));
    rparam->setMinValue(0.0);

    EXTERNAL_FIELD = createVectorParameter("externalField", "Constant external magnetic field", 3u, m_ext_field.data());
    setGroup(EXTERNAL_FIELD, "Magnetic Force");
    setDescription(EXTERNAL_FIELD, "Vector to define the constant external field intensity.");

}

