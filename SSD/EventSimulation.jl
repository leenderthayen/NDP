using SolidStateDetectors
using Unitful
using DataFrames
using ProgressMeter

T = Float32

function CreateSSDEvent(df::SubDataFrame, offset::CartesianPoint{T})
    pos = Matrix(df[:, [:X, :Y, :Z]])
    starting_positions = Vector{CylindricalPoint{T}}()
    for i in 1:size(pos, 1)
        push!(starting_positions, CylindricalPoint{T}(CartesianPoint{T}(pos[i, :]/1000) + offset))
    end
    energy_deps = convert(Array{T}, df[:, :E]) * u"keV"

    return Event(starting_positions, energy_deps)
end

function GetClusteredGeant4Events(gdf::GroupedDataFrame{DataFrame}, offset::CartesianPoint{T},
    radius = 0.2u"mm")::DetectorHitEvents
    events = Vector{Event}()
    for i in length(gdf)
        event = GenerateSSDEvent(gdf[i], offset)
        push!(events, event)
    end
    events_clustered = SolidStateDetectors.cluster_detector_hits(events, radius)

    return events_clustered
end

function DriftGeant4Events(gdf::GroupedDataFrame{DataFrame}, sim::Simulation, offset::CartesianPoint{T}; time_step = 0.1u"ns", stepLimiter::Integer=Inf)
    events = Vector{Event}()
    @info "Simulating events"
    @showprogress for i in 1:min(stepLimiter, length(gdf))
        event = CreateSSDEvent(gdf[i], offset)
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
