#include "MagneticForce_Huang2019.h"
#include "SPlisHSPlasH/TimeManager.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "../TimeStep.h"
#include "../Simulation.h"
#include <vector>
#include "extern/magnetic-force/magnetic_solver/ferro.cuh"
#include <iostream>
#include <cmath>
#include <fstream>
#include <iomanip>


using namespace SPH;

MagneticForce_Huang2019::MagneticForce_Huang2019(FluidModel *model) :
    MagneticForceBase(model)
{
    m_step = 0;
    const unsigned int numParticles = m_model->numActiveParticles();
    for (int i = 0; i < int(numParticles); i++)
        m_forces.push_back(Vector3r(0,0,0));
    m_const_ext_field[0] = 0;
    m_const_ext_field[1] = 0;
    m_const_ext_field[2] = 0;

}

MagneticForce_Huang2019::~MagneticForce_Huang2019(void)
{
}

void MagneticForce_Huang2019::step()
{


    Simulation *sim = Simulation::getCurrent();
    std::cout << m_step << std::endl;

    const unsigned int numParticles = m_model->numActiveParticles();
    const unsigned int nFluids = sim->numberOfFluidModels();
    const unsigned int nBoundaries = sim->numberOfBoundaryModels();
    FluidModel *model = m_model;

    if (m_step % 10 == 0)
    {
        std::vector<std::vector<float>> particlePos{};
        std::vector<int> outliers{};

        if (m_step == 0)
        {
            m_const_ext_field[0] = static_cast<float>(m_ext_field(0));
            m_const_ext_field[1] = static_cast<float>(m_ext_field(1));
            m_const_ext_field[2] = static_cast<float>(m_ext_field(2));
        }

        for (int i = 0; i < numParticles; i++)
        {
            Vector3r &xi = m_model->getPosition(i);
            if (std::abs(xi(0)) > 0.025 || std::abs(xi(1)) > 0.025 || std::abs(xi(2)) > 0.025)
            {
                outliers.push_back(i);
            }
            else
            {
                particlePos.push_back(std::vector<float>{static_cast<float>(xi(0)), static_cast<float>(xi(1)),
                                                         static_cast<float>(xi(2))});
            }
        }


        std::vector<std::vector<float>> f = ferro(particlePos,m_susceptibility,m_const_ext_field);

        for (int i = 0; i < numParticles; i++)
        {
            m_forces[i](0, 0) = static_cast<Real>(f[i][0]);
            m_forces[i](1, 0) = static_cast<Real>(f[i][1]);
            m_forces[i](2, 0) = static_cast<Real>(f[i][2]);
        }

        for (int i = 0; i < outliers.size(); i++)
            f.insert(f.begin() + outliers[i], 1, std::vector<float>{0.f,0.f,0.f});

        std::cout << "f_t for particle 0 " << std::endl;
        std::cout << m_forces[0](0,0) << std::endl;
        std::cout << m_forces[0](1,0) << std::endl;
        std::cout << m_forces[0](2,0) << std::endl;

    }
    for (int i = 0; i < numParticles; i++) {
        Vector3r &ai = m_model->getAcceleration(i);
        float mi = model->getMass(i);
        ai(0) += m_forces[i](0) / mi;
        ai(1) += m_forces[i](1) / mi;
        ai(2) += m_forces[i](2) / mi;

    }
    m_step++;

}

void MagneticForce_Huang2019::reset()
{
}

