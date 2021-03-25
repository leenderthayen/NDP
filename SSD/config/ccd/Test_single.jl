using SolidStateDetectors
using IntervalSets
using SpecialFunctions
T=Float32

struct CustomChargeDensity{T, N} <: SolidStateDetectors.AbstractChargeDensity{T}
	G1::SolidStateDetectors.AbstractGeometry{T,N}
	G2::SolidStateDetectors.AbstractGeometry{T,N}
	L1::SolidStateDetectors.AbstractChargeDensity{T}
	L2::SolidStateDetectors.AbstractChargeDensity{T}
	G3::SolidStateDetectors.AbstractGeometry{T,N}
	L3::SolidStateDetectors.AbstractChargeDensity{T}
end

function SolidStateDetectors.get_charge_density(cdm::CustomChargeDensity{T}, pt::SolidStateDetectors.AbstractCoordinatePoint{T})::T where {T}
	(pt in cdm.G1) * SolidStateDetectors.get_charge_density(cdm.L1, pt) +
	(pt in cdm.G2) * SolidStateDetectors.get_charge_density(cdm.L2, pt) +
	(pt in cdm.G3) * SolidStateDetectors.get_charge_density(cdm.L3, pt) 
end

G1 = SolidStateDetectors.Tube{T}(0..0.00725,0..2π,0..0.002,CartesianPoint{T}(0.0, 0.0, 0))
G2 = SolidStateDetectors.Tube{T}(0..0.00725,0..2π,0..1e-05,CartesianPoint{T}(0.0, 0.0, 0))
CD1 = SolidStateDetectors.CylindricalChargeDensity{T}((0.0f0,0.0f0,50000000000.0),(3e+24,0.0f0,0.0f0))
CD2 = SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,-1.9e+16),(0.0f0,0.0f0,0.0f0))
G3 = SolidStateDetectors.HexagonalPrism{T}(0, 0.00515, 2e-05,CartesianPoint{T}(0, 0, 0.00199),0)
CD3= SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,1.9e+16),(0.0f0,0.0f0,0.0f0))

ccdm = CustomChargeDensity(G1,G2,CD1,CD2,G3,CD3)
