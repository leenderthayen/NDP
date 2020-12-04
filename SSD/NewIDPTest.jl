using SolidStateDetectors
using IntervalSets
using Plots; pyplot(fmt = :png);

T=Float32

struct CustomChargeDensity{T, N} <: SolidStateDetectors.AbstractChargeDensity{T}
       G1::SolidStateDetectors.AbstractGeometry{T,N}
       G2::SolidStateDetectors.AbstractGeometry{T,N}
       G3::SolidStateDetectors.AbstractGeometry{T,N}
       G4::SolidStateDetectors.AbstractGeometry{T,N}
       G5::SolidStateDetectors.AbstractGeometry{T,N}
       L1::SolidStateDetectors.AbstractChargeDensity{T}
       L2::SolidStateDetectors.AbstractChargeDensity{T}
       L3::SolidStateDetectors.AbstractChargeDensity{T}
       L4::SolidStateDetectors.AbstractChargeDensity{T}
       L5::SolidStateDetectors.AbstractChargeDensity{T}
end

function SolidStateDetectors.get_charge_density(cdm::CustomChargeDensity{T}, pt::SolidStateDetectors.AbstractCoordinatePoint{T})::T where {T}
	(pt in cdm.G1) * SolidStateDetectors.get_charge_density(cdm.L1, pt) +
	(pt in cdm.G2) * SolidStateDetectors.get_charge_density(cdm.L2, pt) +
	(pt in cdm.G3) * SolidStateDetectors.get_charge_density(cdm.L3, pt) +
	(pt in cdm.G4) * SolidStateDetectors.get_charge_density(cdm.L4, pt) +
	(pt in cdm.G5) * SolidStateDetectors.get_charge_density(cdm.L5, pt)
end

G1 = SolidStateDetectors.Tube{T}(0..0.00725,0..6.28318530718,0..0.002,missing)
G2 = SolidStateDetectors.Tube{T}(0..0.00725,0..6.28318530718,0..0.00001,missing)
G3 = SolidStateDetectors.HexagonalPrism{T}(0, 0.00515,0.00001,CartesianPoint{T}(0,0,0.001995),0)
#pstop
G4 = SolidStateDetectors.HexagonalPrism{T}(0.00524, 0.00525,0.00001,CartesianPoint{T}(0,0,0.001995),0)
#adding gradient to p+ side
G5 = SolidStateDetectors.Tube{T}(0..0.00725,0..6.28318530718,0..0.00005,CartesianPoint{T}(0,0,0.000010000001))
CD5 = SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,-1.9f16),(0.0f0,0.0f0,0.3f21))

CD1 = SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,5.0f10),(0.0f0,0.0f0,0.0f0))
CD2 = SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,-1.9f16),(0.0f0,0.0f0,0.0f0))
CD3 = SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,1.9f16),(0.0f0,0.0f0,0.0f0))
CD4 = SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,-1.9f16),(0.0f0,0.0f0,0.0f0))

ccdm = CustomChargeDensity(G1,G2,G3,G4,G5,CD1,CD2,CD3,CD4,CD5)

simulation = Simulation{T}("config_files/Si 1Hexagon 10.3-0.1.json")
simulation.detector.semiconductors[1].charge_density_model = ccdm

zs = 0:1e-6:0.002
charge_vec = similar(zs)
for iz in eachindex(zs)
    pt = CartesianPoint{T}(0,0,zs[iz])
    charge_vec[iz]=pt in simulation.detector.semiconductors[1] ?
    		      SolidStateDetectors.get_charge_density(simulation.detector.semiconductors[1], pt) : 0
end

zs2 = 0.0005:1e-6:0.0015
charge_vec2 = similar(zs2)
for iz in eachindex(zs2)
    pt = CartesianPoint{T}(0,0,zs2[iz])
    charge_vec2[iz]=pt in simulation.detector.semiconductors[1] ?
    		      SolidStateDetectors.get_charge_density(simulation.detector.semiconductors[1], pt) : 0
end

calculate_electric_potential!(simulation, max_refinements = 2,depletion_handling=true)

plot(zs, charge_vec, ylabel="impurity density", xlabel = "z")
png("1pixel_density_z")

plot(zs2, charge_vec2, ylabel="impurity density", xlabel = "z")
png("1pixel_density_z2")

plot(simulation.ρ)
png("1pixel_density")

plot(simulation.ρ, z=0.001995)
png("1pixel_density_top")
