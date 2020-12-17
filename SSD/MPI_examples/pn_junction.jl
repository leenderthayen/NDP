using SolidStateDetectors
using Plots
using SpecialFunctions

struct CustomChargeDensity{T} <: SolidStateDetectors.AbstractChargeDensity{T}
    A::T
    S::T
    PN::T
    NP::T # N+
    PP::T # P+
end
function SolidStateDetectors.get_charge_density(cdm::CustomChargeDensity{T}, pt::CartesianPoint{T}) where {T}
    # just a dummy model for a pn-junction with overdoped n+ and p+ layer on both ends
    cdm.A * erf(cdm.S * (pt[1] - cdm.PN)) +
    2 * cdm.A * erf(10 * cdm.S * (pt[1] - cdm.NP)) +
    2 * cdm.A * erf(10 * cdm.S * (pt[1] - cdm.PP))
end
function SolidStateDetectors.get_charge_density(cdm::CustomChargeDensity{T}, pt::CylindricalPoint{T}) where {T}
    SolidStateDetectors.get_charge_density(cdm, CartesianPoint(pt))
end

begin
    T = Float32
    sim = Simulation{T}(SSD_examples[:InfiniteParallelPlateCapacitor])

    ccdm = CustomChargeDensity{Float32}( 1*10^17, -10^4, 0, 0.00035, -0.00035 ) # meter not mm
    sim.detector.semiconductors[1].charge_density_model = ccdm

    xs = -5e-4:1e-6:5e-4
    charge_vec = similar(xs)
    for ix in eachindex(xs)
        pt = CartesianPoint{T}(xs[ix], 0, 0)
        charge_vec[ix] = pt in sim.detector.semiconductors[1] ?
            SolidStateDetectors.get_charge_density(sim.detector.semiconductors[1], pt) :
            0
    end

    calculate_electric_potential!(sim,
        max_refinements = 4,
        init_grid_spacing = T.( (1e-4, 1e-3, 1e-3) ),
        convergence_limit = 1e-3,
        depletion_handling=true
    )

    plot(
        plot(xs, charge_vec, ylabel = "impurity density [1/m^3]", xlabel = "x [m]"),
        plot(sim.ρ, aspect_ratio = :none, y = 0),
        plot(sim.electric_potential, aspect_ratio = :none, y = 0),
        plot(sim.point_types, aspect_ratio = :none, y = 0),
        layout = (4,1), size = (800, 1200)
    )
end
