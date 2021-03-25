using Plots; pyplot(fmt = :png)

include("ReadGeant4Hits.jl")
include("SetupSSD.jl")
include("EventSimulation.jl")
include("WaveformAnalysis.jl")
include("DigitalFilters.jl")
include("Plotting.jl")
include("SummaryReport.jl")

@info "Start"

temp = "140K"

# Setup the simulation
simFilename = "save/1Hex110K.h5"
geometryFile = "config_files/Si 1Hexagon 10.3-0.1.json"
chargeDriftConfigFile = string("config_files/drift_velocity_si_", temp, ".json")
#charge_drift_model = ADLChargeDriftModel(chargeDriftConfigFile)
CCDName = "config_files/1Hexagon.jl"
max_ref = 3

# Get either a saved simulation or a new one
sim = ReadSimulation(simFilename)
if isnothing(sim)
    sim = SetupDefaultSimulation(geometryFile, CCDName)
    CalculateDefaultDetectorFields!(sim, chargeDriftConfigFile, max_ref)
    SaveSimulation(simFilename, sim)
end

SetChargeDriftModel!(sim, chargeDriftConfigFile)
#SetChargeDriftModel!(sim, ADLChargeDriftModel())

geantDirectory = "/home/Group/NabSim/data/"
particle = "e-_40keV"
angles = [0,10,20,30,40,50,60,70,80]
geantFilenames = [string(geantDirectory, particle, "_", angle,"inc.root") for angle in angles]
gdfs = [GetHitInformation(file) for file in geantFilenames]

offset0 = CartesianPoint{T}(0, 0, 0)
events0 = []
for gdf in gdfs
    global events0 =[events0; DriftGeant4Events(gdf, sim, offset0, time_step=0.1u"ns",stepLimiter=1000000000)]
end
riseTimes0 = CollectRiseTimesFiltered(events0, dt=0.1e-9)

histogram(riseTimes0 / 1u"ns", bins = 50, alpha = 0.5, label=string(particle," ",temp))
savefig(string("plots/", particle, string("RiseTimeFiltered",temp,".png")))


@info "Stop"
