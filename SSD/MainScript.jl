using Plots; pyplot(fmt = :png)

include("src/ReadGeant4Hits.jl")
include("src/ReadNoiseROOT.jl")
include("src/SetupSSD.jl")
include("src/EventSimulation.jl")
include("src/WaveformAnalysis.jl")
include("src/DigitalFilters.jl")

@info "Start"

# Setup the simulation
simFilename = "save/1Hex_simple.h5"
geometryFile = "config/geometry/Si 1Hexagon 2.0-10.3-0.1.json"
chargeDriftConfigFile = "config/mobility/drift_velocity_si_140K.json"
CCDFile = "config/ccd/1Hex_simple.jl"
include(CCDFile)
max_ref = 2

dataDirectory = "../analysis/Power_Spectrum/"
noiseFilename = string(dataDirectory, "Run74_0.root")

@info "Loading Simulation"
# Get either a saved simulation or a new one
sim = ReadSimulation(simFilename)
if isnothing(sim)
    sim = SetupDefaultSimulation(geometryFile, CCDFile)
    sim.detector.contacts[1].potential = -150
    CalculateDefaultDetectorFields!(sim, chargeDriftConfigFile, max_ref)
    SaveSimulation(simFilename, sim)
end

@info "Setting CDM"
SetChargeDriftModel!(sim, chargeDriftConfigFile)

@info "Loading Geant Data"
geantFilename = "../data/proton_30keV_0inc.root"
gdf = GetHitInformation(geantFilename)

@info "Applying Noise"
# [Noise Filename , Scalar for the noise amplitude]: Noise has a fixed amplitude, but the SSD waveforms are scaled to be 1.
# Roughly taking 60 keV = 40 ADC channel | 1 = E_sim, scalar needs to be 60 / (40 * E_sim)
elecNoise = GetNoiseROOT(noiseFilename,0.003)
#
offset0 = CartesianPoint{T}(0, 0, 0)
events0 = DriftGeant4Events(gdf, sim, offset0, stepLimiter=1000000)

@info "Calculating rise times"
riseTimes0 = CollectRiseTimes(events0, contact=2)
@info "Calculating rise times with noise"
riseTimesNoisy = CollectRiseTimesNoisyFiltered(events0,elecNoise, contact=2)

#
# offsetEdge = CartesianPoint{T}(3e-3, 0, 0)
# eventsEdge = DriftGeant4Events(gdf, sim, offsetEdge)
# riseTimesEdge = CollectRiseTimes(eventsEdge)
#
histogram(riseTimes0 / 1u"ns", bins = 50, alpha = 0.5, label="Filtered, no noise")
histogram(riseTimesNoisy / 1u"ns", bins = 50, alpha = 0.5, label="Filtered, with noise")
# histogram!(riseTimesEdge / 1u"ns", bins = 50, alpha= 0.5, label="Edge")
xlabel!("Rise time [ns]")
png("NoiseRiseTimes")

@info "Stop"
