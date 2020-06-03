using SolidStateDetectors
using Unitful
using DataFrames

T = Float32

function GenerateEvent(df::SubDataFrame)
    pos = Matrix(df[:, [:X, :Y, :Z]])
    starting_positions = Vector{CartesianPoint{T}}()
    for i in 1:size(pos, 1)
        push!(starting_positions, CartesianPoint{T}(pos[i, :]))
    end
    energy_deps = convert(Array{T}, df[:, :E]) * u"keV"

    return Event(starting_positions, energy_deps)
end

function DriftEvent(event::Event, simulation::Simulation, time_step = 5u"ns")
    drift_charges!(event, simulation, Δt = time_step)
end

function SimulateWaveform(event::Event, simulation::Simulation)
    simulate!(event, simulation)
end
