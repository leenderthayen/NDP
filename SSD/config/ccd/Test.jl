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
	G10::SolidStateDetectors.AbstractGeometry{T,N}
	L10::SolidStateDetectors.AbstractChargeDensity{T}
	G11::SolidStateDetectors.AbstractGeometry{T,N}
	L11::SolidStateDetectors.AbstractChargeDensity{T}
	G12::SolidStateDetectors.AbstractGeometry{T,N}
	L12::SolidStateDetectors.AbstractChargeDensity{T}
	G13::SolidStateDetectors.AbstractGeometry{T,N}
	L13::SolidStateDetectors.AbstractChargeDensity{T}
	G14::SolidStateDetectors.AbstractGeometry{T,N}
	L14::SolidStateDetectors.AbstractChargeDensity{T}
	G15::SolidStateDetectors.AbstractGeometry{T,N}
	L15::SolidStateDetectors.AbstractChargeDensity{T}
	G16::SolidStateDetectors.AbstractGeometry{T,N}
	L16::SolidStateDetectors.AbstractChargeDensity{T}
	G17::SolidStateDetectors.AbstractGeometry{T,N}
	L17::SolidStateDetectors.AbstractChargeDensity{T}
end

function SolidStateDetectors.get_charge_density(cdm::CustomChargeDensity{T}, pt::SolidStateDetectors.AbstractCoordinatePoint{T})::T where {T}
	(pt in cdm.G1) * SolidStateDetectors.get_charge_density(cdm.L1, pt) + 800000000000.0 * (1 + erf((sqrt(pt[1]^2+pt[2]^2)-(0.7))/(2*0.1)))/2 +
	(pt in cdm.G2) * SolidStateDetectors.get_charge_density(cdm.L2, pt) +
	(pt in cdm.G3) * SolidStateDetectors.get_charge_density(cdm.L3, pt) +
	(pt in cdm.G4) * SolidStateDetectors.get_charge_density(cdm.L4, pt) +
	(pt in cdm.G5) * SolidStateDetectors.get_charge_density(cdm.L5, pt) +
	(pt in cdm.G6) * SolidStateDetectors.get_charge_density(cdm.L6, pt) +
	(pt in cdm.G7) * SolidStateDetectors.get_charge_density(cdm.L7, pt) +
	(pt in cdm.G8) * SolidStateDetectors.get_charge_density(cdm.L8, pt) +
	(pt in cdm.G9) * SolidStateDetectors.get_charge_density(cdm.L9, pt) +
	(pt in cdm.G10) * SolidStateDetectors.get_charge_density(cdm.L10, pt) * erfc(-(pt[3]-(1.0074e-05))/(2*3.7e-08))/2 +
	(pt in cdm.G11) * SolidStateDetectors.get_charge_density(cdm.L11, pt) * (1 + erf(-(pt[3]-(0.0019797316718427))/(2*1.3416407864998738e-07)))/2 +
	(pt in cdm.G12) * SolidStateDetectors.get_charge_density(cdm.L12, pt) * (1 + erf(-(pt[3]-(0.0019797316718427))/(2*1.3416407864998738e-07)))/2 +
	(pt in cdm.G13) * SolidStateDetectors.get_charge_density(cdm.L13, pt) * (1 + erf(-(pt[3]-(0.0019797316718427))/(2*1.3416407864998738e-07)))/2 +
	(pt in cdm.G14) * SolidStateDetectors.get_charge_density(cdm.L14, pt) * (1 + erf(-(pt[3]-(0.0019797316718427))/(2*1.3416407864998738e-07)))/2 +
	(pt in cdm.G15) * SolidStateDetectors.get_charge_density(cdm.L15, pt) * (1 + erf(-(pt[3]-(0.0019797316718427))/(2*1.3416407864998738e-07)))/2 +
	(pt in cdm.G16) * SolidStateDetectors.get_charge_density(cdm.L16, pt) * (1 + erf(-(pt[3]-(0.0019797316718427))/(2*1.3416407864998738e-07)))/2 +
	(pt in cdm.G17) * SolidStateDetectors.get_charge_density(cdm.L17, pt) * (1 + erf(-(pt[3]-(0.0019797316718427))/(2*1.3416407864998738e-07)))/2 
end

G1 = SolidStateDetectors.Tube{T}(0..0.01627006165897972,0..2π,0..0.002,CartesianPoint{T}(-1.2390881971262907e-19, 9.912705577010326e-19, 0))
G2 = SolidStateDetectors.Tube{T}(0..0.01627006165897972,0..2π,0..1e-05,CartesianPoint{T}(-1.2390881971262907e-19, 9.912705577010326e-19, 0))
CD1 = SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,50000000000.0),(0.0f0,0.0f0,0.0f0))
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
G10 = SolidStateDetectors.Tube{T}(0..0.01627006165897972,0..2π,0..1.48e-07,CartesianPoint{T}(-1.2390881971262907e-19, 9.912705577010326e-19, 1.0001000000000001e-05))
CD10= SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,-1.9e+16),(0.0f0,0.0f0,0.0f0))
G11 = SolidStateDetectors.HexagonalPrism{T}(0, 0.00515, 5.366563145999495e-07,CartesianPoint{T}(0, 0, 0.0019797316718427),0)
CD11= SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,1.9e+16),(0.0f0,0.0f0,0.0f0))
G12 = SolidStateDetectors.HexagonalPrism{T}(0, 0.00515, 5.366563145999495e-07,CartesianPoint{T}(0.009020061658979717, 0.0, 0.0019797316718427),0)
CD12= SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,1.9e+16),(0.0f0,0.0f0,0.0f0))
G13 = SolidStateDetectors.HexagonalPrism{T}(0, 0.00515, 5.366563145999495e-07,CartesianPoint{T}(0.0045100308294898596, 0.0078116025403784425, 0.0019797316718427),0)
CD13= SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,1.9e+16),(0.0f0,0.0f0,0.0f0))
G14 = SolidStateDetectors.HexagonalPrism{T}(0, 0.00515, 5.366563145999495e-07,CartesianPoint{T}(-0.004510030829489857, 0.007811602540378445, 0.0019797316718427),0)
CD14= SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,1.9e+16),(0.0f0,0.0f0,0.0f0))
G15 = SolidStateDetectors.HexagonalPrism{T}(0, 0.00515, 5.366563145999495e-07,CartesianPoint{T}(-0.009020061658979717, 1.1046389638781276e-18, 0.0019797316718427),0)
CD15= SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,1.9e+16),(0.0f0,0.0f0,0.0f0))
G16 = SolidStateDetectors.HexagonalPrism{T}(0, 0.00515, 5.366563145999495e-07,CartesianPoint{T}(-0.004510030829489863, -0.007811602540378441, 0.0019797316718427),0)
CD16= SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,1.9e+16),(0.0f0,0.0f0,0.0f0))
G17 = SolidStateDetectors.HexagonalPrism{T}(0, 0.00515, 5.366563145999495e-07,CartesianPoint{T}(0.004510030829489859, -0.0078116025403784425, 0.0019797316718427),0)
CD17= SolidStateDetectors.LinearChargeDensity{T}((0.0f0,0.0f0,1.9e+16),(0.0f0,0.0f0,0.0f0))

ccdm = CustomChargeDensity(G1,G2,CD1,CD2,G3,CD3,G4,CD4,G5,CD5,G6,CD6,G7,CD7,G8,CD8,G9,CD9,G10,CD10,G11,CD11,G12,CD12,G13,CD13,G14,CD14,G15,CD15,G16,CD16,G17,CD17)
