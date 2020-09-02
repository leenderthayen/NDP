using Plots; pyplot(fmt = :png)

include("ReadGeant4Hits.jl")
include("SetupSSD.jl")
include("EventSimulation.jl")
include("WaveformAnalysis.jl")

println("#######################")
println("# Start")
println("#######################")

# Setup the simulation
simFilename = "save/Planar_Si"
geometryFile = "config_files/planar_si.json"
chargeDriftConfigFile = "config_files/drift_velocity_config_linear_si.json"
max_ref = 3

# Get either a saved simulation or a new one
sim = ReadSimulation(simFilename)
if isnothing(sim)
    sim = SetupSimulation(geometryFile)
    CalculateDefaultDetectorFields!(sim, ssdChargeDriftConfigFile, max_ref)
    SaveSimulation(simFilename, sim)
end

println("Reading Geant4 Hits info from HDF5")
geantFilename = "data/p_30keV.hdf5"
gdf = GetHitInformation(geantFilename)

offset0 = CartesianPoint{T}(0, 0, 0)
events0 = DriftGeant4Events(gdf, sim, offset0)
riseTimes0 = CollectRiseTimes(events0)

offsetEdge = CartesianPoint{T}(3e-3, 0, 0)
eventsEdge = DriftGeant4Events(gdf, sim, offsetEdge)
riseTimesEdge = CollectRiseTimes(eventsEdge)

histogram(riseTimes0 / 1u"ns", bins = 50, alpha = 0.5, label="Center")
histogram!(riseTimesEdge / 1u"ns", bins = 50, alpha= 0.5, label="Edge")
xlabel!("Rise time [ns]")

println("#######################")
println("# Stop")
println("#######################")
