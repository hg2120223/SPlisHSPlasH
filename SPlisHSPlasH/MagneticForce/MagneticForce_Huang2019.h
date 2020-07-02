#ifndef SPLISHSPLASH_MAGNETICFORCE_HUANG2019_H
#define SPLISHSPLASH_MAGNETICFORCE_HUANG2019_H

#include "SPlisHSPlasH/Common.h"
#include "SPlisHSPlasH/FluidModel.h"
#include "MagneticForceBase.h"

namespace SPH
{
    /** \brief This class implements the magnetic force method introduced
    * by Huang et al. \cite Huang:2019.
    */
    class MagneticForce_Huang2019 : public MagneticForceBase
    {
    private:
        unsigned int m_step;
        std::vector<Vector3r> m_forces;
        float m_const_ext_field[3];

    public:
        MagneticForce_Huang2019(FluidModel *model);
        virtual ~MagneticForce_Huang2019(void);

        virtual void step();
        virtual void reset();
    };
}

#endif //SPLISHSPLASH_MAGNETICFORCE_HUANG2019_H
