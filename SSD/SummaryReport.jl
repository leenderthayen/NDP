using SolidStateDetectors

include("Plotting.jl")

defaultDir = "plots/"

function BiasVariation(simName::String, biasRange::Array{Float64})::AbstractDict{Real, Simulation}

    @info "Constructing base detector"
    sim_base = Simulation(simName)
    sim_dict = Dict{Real, Simulation}()

    @info "Calculating weighting potentials"
    for i in 1:size(sim_base.detector.contacts, 1)
        calculate_weighting_potential!(sim_base, i)
    end

    for i in 1:size(biasRange, 1)
        @info "Setting bias voltage"
        det = deepcopy(sim_base.detector)
        det.contacts[1].potential = -biasRange[i]

        sim = Simulation(det)
        calculate_electric_potential!(sim, depletion_handling=true, max_refinements=2)
        calculate_electric_field!(sim)

        sim.weighting_potentials = sim_base.weighting_potentials

        sim_dict[biasRange[i]] = sim
    end

    return sim_dict
end

function PrepareSummaryPlots(simName::String, initVoltage, finalVoltage, length, prefix::String="", dir::String=defaultDir)::AbstractDict{Real, Simulation}
    biasRange = range(initVoltage, stop=finalVoltage, length=length)

    sims = BiasVariation(simName, collect(biasRange))

    PlotGeometry(sims[biasRange[1]], dir, prefix)

    for i in 1:size(collect(biasRange), 1)
        PlotFields(sims[biasRange[i]], dir, prefix, "_" * string(biasRange[i]) * "V")
        PlotMaterialProperties(sims[biasRange[i]], dir, prefix, "_" * string(biasRange[i]) * "V")
    end

    PlotCV(collect(values(sims)), dir, prefix)

    return sims
end
