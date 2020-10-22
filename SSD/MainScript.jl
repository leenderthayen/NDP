using Plots; pyplot(fmt = :png)

include("ReadGeant4Hits.jl")
include("SetupSSD.jl")
include("EventSimulation.jl")
include("WaveformAnalysis.jl")

@info "Start"

# Setup the simulation
simFilename = "save/hex_pixel_Si.h5"
geometryFile = "config_files/simple_si_pixel.json"
chargeDriftConfigFile = "config_files/drift_velocity_config_linear_si.json"
max_ref = 4

# Get either a saved simulation or a new one
sim = ReadSimulation(simFilename)
if isnothing(sim)
    sim = SetupSimulation(geometryFile)
    CalculateDefaultDetectorFields!(sim, chargeDriftConfigFile, max_ref)
    SaveSimulation(simFilename, sim)
end

SetChargeDriftModel!(sim, chargeDriftConfigFile)

geantFilename = "../data/g_50keV.root"
gdf = GetHitInformation(geantFilename)
#
offset0 = CartesianPoint{T}(0, 0, 0)
events0 = DriftGeant4Events(gdf, sim, offset0)
riseTimes0 = CollectRiseTimes(events0)
#
# offsetEdge = CartesianPoint{T}(3e-3, 0, 0)
# eventsEdge = DriftGeant4Events(gdf, sim, offsetEdge)
# riseTimesEdge = CollectRiseTimes(eventsEdge)
#
histogram(riseTimes0 / 1u"ns", bins = 50, alpha = 0.5, label="Center")
# histogram!(riseTimesEdge / 1u"ns", bins = 50, alpha= 0.5, label="Edge")
# xlabel!("Rise time [ns]")

@info "Stop"
