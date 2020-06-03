using SolidStateDetectors
using Unitful

using Plots;
pyplot(fmt = :png)

T = Float32

function SetupSimulation(configFile::String)::Simulation{T}
    simulation = Simulation(configFile)
    apply_initial_state!(simulation, ElectricPotential)
    for c in simulation.detector.contacts
        apply_initial_state!(simulation, WeightingPotential, c.id)
    end

    return simulation
end

function CalculateElectricPotential(simulation::Simulation{T}, max_ref = 4, verbose = true)
    calculate_electric_potential!(simulation, depletion_handling = true, max_refinements = max_ref)
    if verbose
        println("Active volume: ", get_active_volume(simulation.point_types))
    end
    return nothing
end

function CalculateWeightingPotential(simulation::Simulation{T})
    # TODO Add options
    for contact in simulation.detector.contacts
        calculate_weighting_potential!(simulation, contact.id, max_refinements = 4, n_points_in_φ = 2, verbose = false)
    end
end

function CalculateElectricField(simulation::Simulation{T}, points_in_φ = 72, verbose = true)
    calculate_electric_field!(simulation, n_points_in_φ = points_in_φ)
    if verbose
        println("Capacitance: ", calculate_capacitance(simulation))
    end
end

function ApplyDriftModel(simulation::Simulation{T})
    # TODO Add more options
    apply_charge_drift_model!(simulation)
end

#-------------------------------------------------------------------------------
# Plotting
#

function PlotDetector(simulation::Simulation{T})
    plot(simulation.detector, size = (700, 700), title = "Detector geometry")
    return nothing
end

function PlotElectricPotential(simulation::Simulation{T})::Nothing
    plot(
    plot(simulation.electric_potential), # initial electric potential (boundary conditions)
    plot(simulation.point_types), # map of different point types: fixed point / inside or outside detector volume / depleted/undepleted
    plot(simulation.ρ), # charge density distribution
    plot(simulation.ϵ), # dielectric distribution
    layout = (1, 4), size = (1400, 700)
    )
    return nothing
end

function PlotWeightingPotential(simulation::Simulation{T})::Nothing
    plot(
    plot(simulation.weighting_potential),
    plot(simulation.point_types),
    plot(simulation.electric_potential),
    layout = (1, 3), size = (1000, 700)
    )
    return nothing
end
