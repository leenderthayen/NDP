using SolidStateDetectors

include("Plotting.jl")
include("ReadGeant4Hits.jl")
include("EventSimulation.jl")

defaultDir = "plots/"

T = Float32

function BiasVariation(configFile::String, initVoltage::Real, finalVoltage::Real, length::Integer, CCDName::String="")::AbstractDict{Real, Simulation}
    biasRange = range(initVoltage, stop=finalVoltage, length=length)

    @info "Constructing base detector"
    sim_base = Simulation(configFile)
    sim_dict = Dict{Real, Simulation}()

    if CCDName != ""
        include(CCDName)  #This line needs to instead be put into the main script you are running
        sim_base.detector.semiconductors[1].charge_density_model = ccdm
    end

    @info "Calculating weighting potentials"
    for i in 1:size(sim_base.detector.contacts, 1)
        calculate_weighting_potential!(sim_base, i,
        max_refinements=2,
        convergence_limit=1e-3
        #init_grid_spacing = T.( (2e-4, 2e-4, 2e-4) )
        )
    end

    for i in 1:size(biasRange, 1)
        @info "Setting bias voltage"
        det = deepcopy(sim_base.detector)
        det.contacts[1].potential = -biasRange[i]

        sim = Simulation(det)
        calculate_electric_potential!(sim,
        depletion_handling=true,
        max_refinements=2,
        convergence_limit=1e-3
        #init_grid_spacing = T.( (2e-4, 2e-4, 2e-4) )
        )
        calculate_electric_field!(sim)

        sim.weighting_potentials = sim_base.weighting_potentials

        set_charge_drift_model!(sim, ADLChargeDriftModel())
        calculate_drift_fields!(sim)

        sim_dict[biasRange[i]] = sim
    end

    return sim_dict
end

function GetEligibleFiles(dataDir::String,
    minEnergy::Real=0., maxEnergy::Real=Inf,
    minAngle::Real=0., maxAngle::Real=Inf)::Vector{String}

    fileList = readdir(dataDir, join=true)

    filteredList = Vector{String}()

    regex = r"(\d+)+"

    for s in fileList
         m = collect(eachmatch(regex, s))
         E = parse(Int64, m[end-1].match)
         A = parse(Int64, m[end].match)
         if E >= minEnergy && E <= maxEnergy && A >= minAngle && A <= maxAngle
             push!(filteredList, s)
         end
    end

    return filteredList
end

function PlotSingleParticleDetectorResponse(sim::Simulation, fullPath::String,
    impactPosition::CartesianPoint=CartesianPoint{T}(0, 0, 0), prefix::String="",
    suffix::String="", dir::String=defaultDir, stepLimiter::Integer=1000, qe=x->1)::Vector{Event}

    fileName = split(fullPath, "/")[end]
    strippedName = split(fileName, ".")[1]
    suffix = "_" * strippedName * suffix

    gdf = GetHitInformation(fullPath)
    events = DriftGeant4Events(gdf, sim, impactPosition, stepLimiter=stepLimiter, qe=qe)
    PlotEvents(events, dir, prefix, suffix)
    return events
end

function PlotParticleDetectorResponse(sims::AbstractDict{Real, Simulation}, dataDir::String;
    impactPosition::CartesianPoint=CartesianPoint{T}(0, 0, 0), minEnergy::Real=0., maxEnergy::Real=Inf,
    minAngle::Real=0., maxAngle::Real=Inf,
    prefix::String="", dir::String=defaultDir, maxEvents::Integer=100, qe=x->1)::Nothing

    println(impactPosition)

    prefix = prefix * "particle_"
    biasRange = collect(keys(sims))
    fileList = GetEligibleFiles(dataDir, minEnergy, maxEnergy, minAngle, maxAngle)

    @showprogress for i in 1:size(biasRange, 1)
        sim = sims[biasRange[i]]
        suffix = @sprintf "_%.1fV" biasRange[i]
        @showprogress for s in fileList
            PlotSingleParticleDetectorResponse(sim, s, impactPosition, prefix, suffix, dir, maxEvents, qe)
        end
    end
end

function PlotDetectorPerformance(sims::AbstractDict{Real, Simulation}; prefix::String="", dir::String=defaultDir)::Nothing
    biasRange = collect(keys(sims))
    prefix = prefix * "detector_"
    PlotGeometry(sims[biasRange[1]], dir, prefix)
    Plot2DWeightingPotential(sims[biasRange[1]], dir, prefix)
    Plot1DWeightingPotential(sims[biasRange[1]], dir, prefix)
    PlotMaterialProperties(sims[biasRange[1]], dir, prefix)

    for i in 1:size(biasRange, 1)
        suffix = @sprintf "_%.1fV" biasRange[i]
        PlotElectricPotential(sims[biasRange[i]], dir, prefix, suffix)
        PlotElectricField(sims[biasRange[i]], dir, prefix, suffix)
        PlotPointType(sims[biasRange[i]], dir, prefix, suffix)
    end

    PlotCV(collect(values(sims)), dir, prefix)
end
