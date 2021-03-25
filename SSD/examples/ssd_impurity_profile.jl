using SolidStateDetectors

sim = Simulation(SSD_examples[:InvertedCoax])

@info sim.detector.semiconductors[1].charge_density_model
@info supertype(typeof(sim.detector.semiconductors[1].charge_density_model))
# Custom made impurity profile:

struct CustomImpurityDensity{T} <: SolidStateDetectors.SolidStateDetectors.AbstractChargeDensityModel{T}
    # here you can add parameters of your profile (optional).
    # E.g.:
    offset::T # in 1/m^3
end

custom_impurity_density = CustomImpurityDensity{Float32}(1e16)

# overwrite the model defined in the config file
sim.detector.semiconductors[1].charge_density_model = custom_impurity_density

# calculate_electric_potential!(sim) will throw an error that a function is not defined
# since the packages needs to know how to evaluate the density at a certain point for the `CustomImpurityDensity`

function SolidStateDetectors.get_charge_density(cid::CustomImpurityDensity{T}, pt::CylindricalPoint{T}) where {T}
    # The return value must be in SI units: 1/m^3
    # dummy profile
    z = pt[3] # pt is in SI units. pt[1]: m, pt[2] = rad, pt[3] = m
    if z > 0.001 # > 1mm
        return cid.offset
    else
        0
    end
end

calculate_electric_potential!(sim)

# In case you are working in cartesian coordinates you will need to define it for cartesian points:
function SolidStateDetectors.get_charge_density(cid::CustomImpurityDensity{T}, pt::CartesianPoint{T}) where {T}
    # The return value must be in SI units: 1/m^3
    # dummy profile
    z = pt[3] # pt is in SI units. pt[1]: m, pt[2] = rad, pt[3] = m
    if z > 0.001 # > 1mm
        return cid.offset
    else
        0
    end
end
