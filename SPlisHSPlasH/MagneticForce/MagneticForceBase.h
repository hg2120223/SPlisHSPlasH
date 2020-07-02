#ifndef SPLISHSPLASH_MAGNETICFORCEBASE_H
#define SPLISHSPLASH_MAGNETICFORCEBASE_H

#include "SPlisHSPlasH/Common.h"
#include "SPlisHSPlasH/FluidModel.h"
#include "SPlisHSPlasH/NonPressureForceBase.h"

namespace SPH
{
    /** \brief Base class for all magnetic force methods.
    */
    class MagneticForceBase : public NonPressureForceBase
    {
    protected:
        float m_susceptibility;
        Vector3r m_ext_field;

        virtual void initParameters();

    public:
        static int SUSCEPTIBILITY;
        static int EXTERNAL_FIELD;

        MagneticForceBase(FluidModel *model);
        virtual ~MagneticForceBase(void);
    };
}

#endif //SPLISHSPLASH_MAGNETICFORCEBASE_H
