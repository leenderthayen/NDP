using Plots
using SolidStateDetectors

T = Float32
sim = Simulation(SSD_examples[:InvertedCoax])

calculate_electric_potential!(sim, max_refinements = 0)
plot(sim.ρ)

# The charge density model (actually it should be named impurity density model, we have to change this at some point)
# is part of the semiconductor object of the detector:
sim.detector.semiconductors[1].charge_density_model

# It can be overwritten by an custom density model.
# This must be subtype of AbstractChargeDensity model:

struct CustomChargeDensity{T} <: SolidStateDetectors.AbstractChargeDensity{T}
    # Define all the parameters for your own model here
    # e.g.:
    offset::T
    z_gradient::T
    r_gradient::T
end

# Create a model with arbitray parameters
ccdm = CustomChargeDensity{Float32}( -10^16, -10^18, -10^18) # meter not mm

# overwrite the predefined model
sim.detector.semiconductors[1].charge_density_model = ccdm

# We also need to define two methods for this density model:
# (So actually the model)
function SolidStateDetectors.get_charge_density(cdm::CustomChargeDensity{T}, pt::CylindricalPoint{T}) where {T}
    # This is the actually the impurity density: it will be multiplied by the elementary charge (in SSD)
    # Just an example:
    charge_density::T = cdm.offset
    charge_density += cdm.z_gradient * pt[3] # z coordinate
    charge_density += cdm.r_gradient * pt[1] # r coordinate
    # Note, that pt will be parsed to this function in SI units -> meter
    return charge_density
end
# In case you use cartesian coordinates for the simulation just add this method:
function SolidStateDetectors.get_charge_density(cdm::CustomChargeDensity{T}, pt::CartesianPoint{T}) where {T}
    return SolidStateDetectors.get_charge_density(cdm, CylindricalPoint(pt))
end


calculate_electric_potential!(sim, max_refinements = 2, depletion_handling=true)

plot(sim.point_types)
plot(sim.ρ)

# Plot the profile:
# We want to add this as a plot recipe in the future but is not yet implemented.
rs = 0.0:1e-5:4e-2
charge_vec = similar(rs)
for ir in eachindex(rs)
    pt = CylindricalPoint{T}(rs[ir], 0, 1e-3)
    charge_vec[ir] = pt in sim.detector.semiconductors[1] ?
        SolidStateDetectors.get_charge_density(sim.detector.semiconductors[1], pt) :
        0
end
plot(rs, charge_vec, ylabel = "impurity density [1/m^3]", xlabel = "r [m]")
