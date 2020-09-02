using SolidStateDetectors

#define the charge drift model in your custom json file
cdm = ADLChargeDriftModel()

#define a range of relevant electric field strengths in units of V/m
E = Float32.(0:100:1000000)

#print(cdm.masses)
using StaticArrays
using LinearAlgebra

function SolidStateDetectors.Vl(Emag::T, params::SolidStateDetectors.VelocityParameters{T})::T where {T <: SolidStateDetectors.SSDFloat}
    #define your own parametrization here: e.g. vd = 500 m^2/(Vs) * E
    #beware of units!!
    return 500 * Emag
end

newE = [T(i)*SVector{3,T}(sin(cdm.phi110+π/4),cos(cdm.phi110+π/4),0) for i in E]
ve100a = map(x -> norm(SolidStateDetectors.getVe(x, cdm)), newE)
ve100b = map(x -> SolidStateDetectors.Vl(x,cdm.electrons.axis100), T.(E))

plot(E, [ve100a, ve100b],
    xlabel = "Electric field strength [V/m]", ylabel = "Drift velocity [m/s]")
