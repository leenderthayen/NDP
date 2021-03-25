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
	G4::SolidStateDetectors.AbstractGeometry{T,N}
	L4::SolidStateDetectors.AbstractChargeDensity{T}
	G5::SolidStateDetectors.AbstractGeometry{T,N}
	L5::SolidStateDetectors.AbstractChargeDensity{T}
	G6::SolidStateDetectors.AbstractGeometry{T,N}
	L6::SolidStateDetectors.AbstractChargeDensity{T}
	G7::SolidStateDetectors.AbstractGeometry{T,N}
	L7::SolidStateDetectors.AbstractChargeDensity{T}
	G8::SolidStateDetectors.AbstractGeometry{T,N}
	L8::SolidStateDetectors.AbstractChargeDensity{T}
	G9::SolidStateDetectors.AbstractGeometry{T,N}
	L9::SolidStateDetectors.AbstractChargeDensity{T}
end

function SolidStateDetectors.get_charge_density(cdm::CustomChargeDensity{T}, pt::SolidStateDetectors.AbstractCoordinatePoint{T})::T where {T}
	(pt in cdm.G1) * SolidStateDetectors.get_charge_density(cdm.L1, pt) +
	(pt in cdm.G2) * SolidStateDetectors.get_charge_density(cdm.L2, pt) +
	(pt in cdm.G3) * SolidStateDetectors.get_charge_density(cdm.L3, pt) +
	(pt in cdm.G4) * SolidStateDetectors.get_charge_density(cdm.L4, pt) +
	(pt in cdm.G5) * SolidStateDetectors.get_charge_density(cdm.L5, pt) +
	(pt in cdm.G6) * SolidStateDetectors.get_charge_density(cdm.L6, pt) +
	(pt in cdm.G7) * SolidStateDetectors.get_charge_density(cdm.L7, pt) +
	(pt in cdm.G8) * SolidStateDetectors.get_charge_density(cdm.L8, pt) +
	(pt in cdm.G9) * SolidStateDetectors.get_charge_density(cdm.L9, pt) 
end

G1 = SolidStateDetectors.Tube{T}(0..0.01627006165897972,0..2π,0..0.002,CartesianPoint{T}(-1.2390881971262907e-19, 9.912705577010326e-19, 0))
G2 = SolidStateDetectors.Tube{T}(0..0.01627006165897972,0..2π,0..1e-05,CartesianPoint{T}(-1.2390881971262907e-19, 9.912705577010326e-19, 0))
CD1 = SolidStateDetectors.CylindricalChargeDensity{T}((0.0f0,0.0f0,50000000000.0),(60000000000.0,0.0f0,0.0f0))
CD2 = SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,-1.9e+16),(0.0f0,0.0f0,0.0f0))
G3 = SolidStateDetectors.HexagonalPrism{T}(0, 0.00515, 2e-05,CartesianPoint{T}(0, 0, 0.00199),0)
CD3= SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,1.9e+16),(0.0f0,0.0f0,0.0f0))
G4 = SolidStateDetectors.HexagonalPrism{T}(0, 0.00515, 2e-05,CartesianPoint{T}(0.009020061658979717, 0.0, 0.00199),0)
CD4= SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,1.9e+16),(0.0f0,0.0f0,0.0f0))
G5 = SolidStateDetectors.HexagonalPrism{T}(0, 0.00515, 2e-05,CartesianPoint{T}(0.0045100308294898596, 0.0078116025403784425, 0.00199),0)
CD5= SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,1.9e+16),(0.0f0,0.0f0,0.0f0))
G6 = SolidStateDetectors.HexagonalPrism{T}(0, 0.00515, 2e-05,CartesianPoint{T}(-0.004510030829489857, 0.007811602540378445, 0.00199),0)
CD6= SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,1.9e+16),(0.0f0,0.0f0,0.0f0))
G7 = SolidStateDetectors.HexagonalPrism{T}(0, 0.00515, 2e-05,CartesianPoint{T}(-0.009020061658979717, 1.1046389638781276e-18, 0.00199),0)
CD7= SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,1.9e+16),(0.0f0,0.0f0,0.0f0))
G8 = SolidStateDetectors.HexagonalPrism{T}(0, 0.00515, 2e-05,CartesianPoint{T}(-0.004510030829489863, -0.007811602540378441, 0.00199),0)
CD8= SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,1.9e+16),(0.0f0,0.0f0,0.0f0))
G9 = SolidStateDetectors.HexagonalPrism{T}(0, 0.00515, 2e-05,CartesianPoint{T}(0.004510030829489859, -0.0078116025403784425, 0.00199),0)
CD9= SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,1.9e+16),(0.0f0,0.0f0,0.0f0))

ccdm = CustomChargeDensity(G1,G2,CD1,CD2,G3,CD3,G4,CD4,G5,CD5,G6,CD6,G7,CD7,G8,CD8,G9,CD9)
