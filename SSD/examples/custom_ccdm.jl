using SolidStateDetectors
using IntervalSets
using Plots
T = Float32

struct CustomChargeDensity{T, N} <: SolidStateDetectors.AbstractChargeDensity{T}
    G1::SolidStateDetectors.AbstractGeometry{T, N}
    G2::SolidStateDetectors.AbstractGeometry{T, N}
    L1::SolidStateDetectors.AbstractChargeDensity{T}
    L2::SolidStateDetectors.AbstractChargeDensity{T}
end

#compact version of the function I showed before
function SolidStateDetectors.get_charge_density(cdm::CustomChargeDensity{T}, pt::SolidStateDetectors.AbstractCoordinatePoint{T})::T where {T}
       (pt in cdm.G1) * SolidStateDetectors.get_charge_density(cdm.L1, pt) +
       (pt in cdm.G2) * SolidStateDetectors.get_charge_density(cdm.L2, pt)
end

G1 = SolidStateDetectors.Tube{T}(0..0.1,0..2π,0..0.04,missing)     #Tube with 0 ≤ z ≤ 0.04
G2 = SolidStateDetectors.Box{T}((-1,1),(-1,1),(0.02,0.08),missing) #Box with 0.02 ≤ z ≤ 0.08
CD1 = SolidStateDetectors.CylindricalChargeDensity{T}((0.0f0, 0.0f0, 1.0f16), (1.0f17, 0.0f0, 1.0f17))
CD2 = SolidStateDetectors.LinearChargeDensity{T}((0.0f0, 0.0f0, 1.0f15), (1.0f17, 0.0f0, 0.0f0))
ccdm = CustomChargeDensity(G1,G2,CD1,CD2)

simulation = Simulation{T}(SSD_examples[:InvertedCoax]) #adjusted phi in the grid to go from 0 to 360 to avoid 2D calculation
simulation.detector.semiconductors[1].charge_density_model = ccdm #apply CustomChargeDensity to the semiconductor
calculate_electric_potential!(simulation, max_refinements = 2)

plot(
    plot(simulation.ρ, φ = 0, resample=false),
    plot(simulation.ρ, z = 0.01),
    plot(simulation.ρ, z = 0.03, resample=false),
    plot(simulation.ρ, z = 0.05),
    size = (1200,1200)
)
