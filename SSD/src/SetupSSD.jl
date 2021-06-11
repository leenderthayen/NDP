#using HDF5
#using LegendHDF5IO: readdata, writedata
using JLD2
using SolidStateDetectors
using Unitful

using Plots;
pyplot(fmt = :png)

T = Float32

function SaveSimulation(filename::String, simulation::Simulation{T})::Nothing
    @info "Saving simulation"
    if !isfile(filename) mkpath(dirname(filename)) end
    jldopen(filename, "w") do f
        write(f, "Simulation", NamedTuple(simulation))
    end
end

function ReadSimulation(filename)::Union{Nothing, Simulation{T}}
    if isfile(filename)
        @info "Reading simulation"
        simulation = jldopen(filename, "r") do f
            ConvertNamedTuple(read(f, "Simulation"))
        end
        # SetChargeDriftModel!(simulation)

        return simulation
    end
    return nothing
end

function ConvertNamedTuple(nt::NamedTuple)::Union{Nothing, Simulation{T}}
    if !ismissing( nt.detector_json_string )
        det = SolidStateDetector{T}( Dict(nt.detector_json_string) )
        sim = Simulation(det)

        if hasproperty(nt, :weighting_potentials)
            for contact in sim.detector.contacts
                if !ismissing( values( nt.weighting_potentials[contact.id] )[1] )
                    sim.weighting_potentials[contact.id] = WeightingPotential( nt.weighting_potentials[contact.id] )
                end
            end
        end

        d = Dict(:ρ => EffectiveChargeDensity,
                 :ρ_fix => EffectiveChargeDensity,
                 :ϵ => DielectricDistribution,
                 :point_types => PointTypes,
                 :electric_potential => ElectricPotential,
                 :electric_field => ElectricField,
                 :electron_drift_field => ElectricField,
                 :hole_drift_field => ElectricField)

        for (prop, func) in d
            if !ismissing( nt[prop][1] ) setproperty!( sim, prop, func( nt[prop] ) ) end
        end
        return sim
    end
    return nothing
end

function CalculateDefaultDetectorFields!(sim::Simulation{T},
    driftConfigFile::Union{Missing, String} = missing,
    max_ref = 4)

    CalculateElectricPotential!(sim, max_ref)
    CalculateElectricField!(sim)
    CalculateWeightingPotential!(sim, max_ref)
    SetChargeDriftModel!(sim, driftConfigFile)
end

function SetupDefaultSimulation(geomConfigFile::String, CCDName::String="")::Simulation{T}
    simulation = Simulation{T}(geomConfigFile)

    if CCDName != ""
        include(CCDName) #put this line in the main script you are running
        simulation.detector.semiconductors[1].charge_density_model = ccdm
    end

    apply_initial_state!(simulation, ElectricPotential)
    for c in simulation.detector.contacts
        apply_initial_state!(simulation, WeightingPotential, c.id)
    end
    return simulation
end

function SetBiasVoltage!(sim, contact, voltage)
    sim.detector.contacts[contact].potential = voltage
end

function CalculateElectricPotential!(simulation::Simulation{T}, max_ref = 4, verbose = false)
    calculate_electric_potential!(simulation, depletion_handling = true, max_refinements = max_ref)
    if verbose
        println("Active volume: ", get_active_volume(simulation.point_types))
    end
end

function CalculateWeightingPotential!(simulation::Simulation{T}, max_ref = 2)
    # TODO Add options
    for contact in simulation.detector.contacts
        calculate_weighting_potential!(simulation, contact.id, max_refinements = max_ref, n_points_in_φ = 2, verbose = false)
    end
end

function CalculateElectricField!(simulation::Simulation{T}, points_in_φ = 72, verbose = true)
    calculate_electric_field!(simulation, n_points_in_φ = points_in_φ)
    if verbose
        println("Capacitance: ", calculate_capacitance(simulation))
    end
end

function SetChargeDriftModel!(simulation::Simulation{T}, driftConfigFile::Union{Missing, String})
    charge_drift_model = ADLChargeDriftModel(driftConfigFile, T=T)
    set_charge_drift_model!(simulation, charge_drift_model)
    calculate_drift_fields!(simulation)
end

#-------------------------------------------------------------------------------
# Plotting
#

function PlotDetector(simulation::Simulation{T})
    p = plot(simulation.detector, size = (700, 700), title = "Detector geometry")
    return p
end

function PlotElectricPotential(simulation::Simulation{T})
    p = plot(
    plot(simulation.electric_potential), # initial electric potential (boundary conditions)
    plot(simulation.point_types), # map of different point types: fixed point / inside or outside detector volume / depleted/undepleted
    plot(simulation.ρ), # charge density distribution
    plot(simulation.ϵ), # dielectric distribution
    layout = (1, 4), size = (1400, 700)
    )
    return p
end

function PlotWeightingPotential(simulation::Simulation{T}, contact::Int64)
    p = plot(
    plot(simulation.weighting_potentials[contact]),
    plot(simulation.point_types),
    plot(simulation.electric_potential),
    layout = (1, 3), size = (1000, 700)
    )
    return p
end
