using Plots
using NBodySimulator
using StaticArrays

struct CustomPotentialParameters <: PotentialParameters
    a::AbstractFloat
end

import NBodySimulator.get_accelerating_function
function get_accelerating_function(p::CustomPotentialParameters, simulation::NBodySimulation)
    ms = get_masses(simulation.system)
    (dv, u, v, t, i) -> begin custom_accel = SVector(0.0, 0.0, p.a); dv .= custom_accel*ms[i] end
end

parameters = CustomPotentialParameters(-9.8)
system = PotentialNBodySystem(bodies, Dict(:custom_potential_params => parameters))

function MakeParticles(N, m, q, size, offset, velocity)
    coords = size * rand(3, N) .+ offset
    speeds = velocity * rand(3, N)

    particles = Array{ChargedParticle{cType, Float64, Float64} where cType<:Real, 1}()

    for i in 1:N
        push!(particles, ChargedParticle(SVector(coords[1, i], coords[2, i], coords[3, i]), SVector(speeds[1, i], speeds[2, i], speeds[3, i]), m, q))
    end

    return particles
end
