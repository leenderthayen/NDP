using SolidStateDetectors
using Unitful

using Plots;
pyplot(fmt = :png)

T = Float32

function SaveSimulation(filename::String, simulation::Simulation{T})::Nothing
    if !ispath(dirname(filename)) mkpath(dirname(filename)) end
    SolidStateDetectors.ssd_write(filename, simulation)
end

function ReadSimulation(filename)::Simulation{T}
    simulation = SolidStateDetectors.ssd_read(filename, Simulation)
    return simulation
end

function CalculateDefaultDetectorFields!(sim::Simulation{T},
    driftConfigFile::Union{Missing, String} = missing,
    max_ref = 4)

    CalculateElectricPotential!(sim, max_ref)
    CalculateElectricField!(sim)
    CalculateWeightingPotential!(sim, max_ref)
    ApplyDriftModel!(sim, driftConfigFile)
end

function SetupSimulation(geomConfigFile::String)::Simulation{T}
    simulation = Simulation{T}(configFile)
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

function ApplyDriftModel!(simulation::Simulation{T}, driftConfigFile::Union{Missing, String})
    charge_drift_model = ADLChargeDriftModel(driftConfigFile, T=T)
    apply_charge_drift_model!(simulation, charge_drift_model)
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
