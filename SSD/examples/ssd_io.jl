# using Pkg
# Pkg.add(PackageSpec(url="https://github.com/legend-exp/LegendHDF5IO.jl.git"))
using HDF5
using LegendHDF5IO: readdata, writedata
using SolidStateDetectors

hd5_sim_filename = "sim.h5"

sim = if isfile(hd5_sim_filename)
    @info "Reading from file $hd5_sim_filename"
    s = HDF5.h5open(hd5_sim_filename, "r") do h5f
        Simulation(readdata(h5f, "Simulation"))
    end
    set_charge_drift_model!(s, ADLChargeDriftModel())
    s
else
    s = Simulation(SSD_examples[:InvertedCoax])
    simulate!(s, max_refinements = 1)
    HDF5.h5open(hd5_sim_filename, "w") do h5f
        writedata(h5f, "Simulation", NamedTuple(s))
    end
end

println(sim)

# Storing and loading individual fields:

sim2 = Simulation(SSD_examples[:InvertedCoax])
calculate_weighting_potential!(sim2, 1) # Weighting potential of channel 1
calculate_weighting_potential!(sim2, 2) # Weighting potential of channel 1

hd5_wps_filename = "weighting_potentials.h5"

HDF5.h5open(hd5_wps_filename, "w") do h5f
    writedata(h5f, "WeightingPotential_001", NamedTuple(sim2.weighting_potentials[1]))
    writedata(h5f, "WeightingPotential_002", NamedTuple(sim2.weighting_potentials[2]))
end

# now read only the first one
sim2 = Simulation(SSD_examples[:InvertedCoax]) # empty Simulation object

sim2.weighting_potentials[1] = HDF5.h5open(hd5_wps_filename, "r") do h5f
    WeightingPotential(readdata(h5f, "WeightingPotential_001"))
end
