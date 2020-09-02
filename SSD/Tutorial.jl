using Plots; pyplot(fmt = :png)
using SolidStateDetectors
using Unitful

T = Float32
simulation = Simulation{T}(SSD_examples[:InvertedCoax])

plot(simulation.detector, size=(700, 700))

apply_initial_state!(simulation, ElectricPotential) # optional
plot(
    plot(simulation.electric_potential), # initial electric potential (boundary conditions)
    plot(simulation.point_types), # map of different point types: fixed point / inside or outside detector volume / depleted/undepleted
    plot(simulation.ρ), # charge density distribution
    plot(simulation.ϵ), # dielectric distribution
    layout = (1, 4), size = (1400, 700)
)

calculate_electric_potential!( simulation,
                               max_refinements = 3)

calculate_electric_field!(simulation, n_points_in_φ = 72)

charge_drift_model = ADLChargeDriftModel()
set_charge_drift_model!(simulation, charge_drift_model)

calculate_drift_fields!(simulation)

starting_positions = [ CylindricalPoint{T}( 0.020, deg2rad(10), 0.015 ),
                       CylindricalPoint{T}( 0.015, deg2rad(20), 0.045 ),
                       CylindricalPoint{T}( 0.025, deg2rad(30), 0.025 ) ]
energy_depos = T[1460, 609, 1000] * u"keV" # are needed later in the signal generation

event = Event(starting_positions, energy_depos);

time_step = 5u"ns"
drift_charges!(event, simulation, Δt = time_step)

plot(simulation.detector, size = (700, 700))
plot!(event.drift_paths)

for contact in simulation.detector.contacts
    calculate_weighting_potential!(simulation, contact.id, max_refinements = 3, n_points_in_φ = 2, verbose = false)
end

plot(
    plot(simulation.weighting_potentials[1]),
    plot(simulation.weighting_potentials[2]),
    size = (900, 700)
)

simulate!(event, simulation) # drift_charges + signal generation of all channels

p_pc_signal = plot( event.waveforms[1], lw = 1.5, xlims = (0, 2000), xlabel = "Time / ns",
                    legend = false, tickfontsize = 12, ylabel = "Energy / eV", guidefontsize = 14)
