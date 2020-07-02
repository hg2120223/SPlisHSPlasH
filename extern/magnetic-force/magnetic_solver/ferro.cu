#include "magnetic_force.cuh"
#include <iostream>

std::vector<std::vector<float>> ferro(std::vector<std::vector<float>> particle_positions, float chi, float* const_ext_field) {
    int nx = 2, ny = 2, nz = 2;
    float dx = 0.0005;

    //source particles must not be empty.
    #ifndef one
    //two particles, bug free
    auto ext_source_pos = std::vector<float3>{ float3{ 0.f,-100.f, 0.f },float3{ 0.f,-101.f, 0.f } };
    auto ext_source_M = std::vector<float3>{ float3{ 0,1,0 }, float3{ 0,1,0 } };
    #else
    //one particle, singularity bug
    auto ext_source_pos = std::vector<float3>{ float3{ 0.f, 0, 0.f } };
    auto ext_source_M = std::vector<float3>{ float3{ 0,1,0 }};
    #endif
    
    
    
    auto target_pos = std::vector<float3>{};
    auto updated_target_pos = target_pos;
    updated_target_pos.reserve(nx*ny*(nz - 1));
    
    for (int i = 0; i < particle_positions.size(); i++) {
        target_pos.push_back(float3{ particle_positions[i][0], particle_positions[i][1], particle_positions[i][2] });
    }
    

    
    size_t  num_particles = target_pos.size();
    float h = dx;
    float ext_mag_h = dx;
    float volume = dx*dx*dx;
    Libo::cuda_magnet_cgOptimizer::CtorParam solver_ctor{ num_particles,h,ext_mag_h,volume,chi,const_ext_field,ext_source_pos,ext_source_M,target_pos };
    Libo::cuda_magnet_cgOptimizer a{ solver_ctor };

    a.setTargetpos(target_pos);
    Libo::cuda_magnet_cgOptimizer::setHextParams seth;
    seth.const_ext_field[0] = const_ext_field[0];
    seth.const_ext_field[1] = const_ext_field[1];
    seth.const_ext_field[2] = const_ext_field[2];
    seth.ext_source_M = ext_source_M;
    seth.ext_source_pos = ext_source_pos;
    seth.ext_mag_h = h;
    a.setHext(seth);

    a.solve(20);
    a.evaluate_fitted_near_foece();
    float * host_forcex = new float[num_particles];
    float * host_forcey = new float[num_particles];
    float * host_forcez = new float[num_particles];
    cudaMemcpy(host_forcex, a.magnetic_bodyforce.dev_x, sizeof(float)*num_particles, cudaMemcpyDeviceToHost);
    cudaMemcpy(host_forcey, a.magnetic_bodyforce.dev_y, sizeof(float)*num_particles, cudaMemcpyDeviceToHost);
    cudaMemcpy(host_forcez, a.magnetic_bodyforce.dev_z, sizeof(float)*num_particles, cudaMemcpyDeviceToHost);
    
    std::vector<std::vector<float>> f_ferro;
    for (int i = 0; i < target_pos.size(); i++)
    {
        f_ferro.push_back(std::vector<float>{ host_forcex[i], host_forcey[i], host_forcez[i] });
    }
    delete[] host_forcex;
    delete[] host_forcey;
    delete[] host_forcez;

    return f_ferro;
}