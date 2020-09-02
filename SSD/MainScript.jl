using Plots; pyplot(fmt = :png)

include("ReadGeant4Hits.jl")
include("SetupSSD.jl")
include("EventSimulation.jl")
include("WaveformAnalysis.jl")

println("Reading Geant4 Hits info from HDF5")
geantFilename = "data/p_30keV.hdf5"
gdf = GetHitInformation(geantFilename)

println("Setting up simulation")
ssdGeometryFile = "config_files/planar_si.json"
ssdChargeDriftConfigFile = "config_files/drift_velocity_config_linear_si.json"
sim = SetupSimulation(ssdGeometryFile, ssdChargeDriftConfigFile)

max_ref = 3

CalculateDefaultDetectorFields(sim, max_ref)

offset0 = CartesianPoint{T}(0, 0, 0)
events0 = DriftGeant4Events(gdf, sim, offset0)
riseTimes0 = CollectRiseTimes(events0)

offsetEdge = CartesianPoint{T}(3e-3, 0, 0)
eventsEdge = DriftGeant4Events(gdf, sim, offsetEdge)
riseTimesEdge = CollectRiseTimes(eventsEdge)

histogram(riseTimes0 / 1u"ns", bins = 50, alpha = 0.5, label="Center")
histogram!(riseTimesEdge / 1u"ns", bins = 50, alpha= 0.5, label="Edge")
xlabel!("Rise time [ns]")
