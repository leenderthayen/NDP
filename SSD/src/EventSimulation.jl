using SolidStateDetectors
using Unitful
using DataFrames
using ProgressMeter

T = Float32

function QuantumEfficiency(t, p0, p1, l)
    f(z) = z < t ? p0 : 1 + (p1 - 1)*exp(-(z-t)/l)
    return f
end

function CreateSSDEvent(df::SubDataFrame, offset::CartesianPoint{T}, qe)
    pos = Matrix(df[:, [:X, :Y, :Z]])
    starting_positions = Vector{CartesianPoint{T}}()
    energy_deps = convert(Array{T}, df[:, :E]) * u"keV"
    for i in 1:size(pos, 1)
        push!(starting_positions, CartesianPoint{T}(CartesianPoint{T}(pos[i, :]/1000) + offset))
        energy_deps[i] *= qe(pos[i, 3]/1000)
    end

    return Event(starting_positions, energy_deps)
end

function GetClusteredGeant4Events(gdf::GroupedDataFrame{DataFrame}, offset::CartesianPoint{T},
    radius = 0.2u"mm", qe=x->1)::DetectorHitEvents
    events = Vector{Event}()
    for i in length(gdf)
        event = GenerateSSDEvent(gdf[i], offset, qe)
        push!(events, event)
    end
    events_clustered = SolidStateDetectors.cluster_detector_hits(events, radius)

    return events_clustered
end

function DriftGeant4Events(gdf::GroupedDataFrame{DataFrame}, sim::Simulation, offset::CartesianPoint{T}; time_step = 0.1u"ns", stepLimiter::Integer=Inf, qe=x->1)
    events = Vector{Event}()
    @info "Simulating events"
    @showprogress for i in 1:min(stepLimiter, length(gdf))
        event = CreateSSDEvent(gdf[i], offset, qe)
        DriftEvent(event, sim, time_step)
        SimulateWaveform(event, sim, time_step)
        push!(events, event)
    end
    return events
end

function DriftEvent(event::Event, simulation::Simulation, time_step)
    drift_charges!(event, simulation, Δt = time_step)
end

function SimulateWaveform(event::Event, simulation::Simulation, time_step = 0.1u"ns")
    simulate!(event, simulation, Δt = time_step)
end
