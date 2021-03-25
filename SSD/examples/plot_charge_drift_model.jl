using SolidStateDetectors

#define the charge drift model in your custom json file
cdm = ADLChargeDriftModel("custom_config_file.json")

#define a range of relevant electric field strengths in units of V/m
E = Float32.(0:100:1000000)

#calculate the drift velocities from this in units of m/s
ve100 = map(E -> SolidStateDetectors.Vl(E, cdm.electrons.axis100), E)
ve111 = map(E -> SolidStateDetectors.Vl(E, cdm.electrons.axis111), E)
vh100 = map(E -> SolidStateDetectors.Vl(E, cdm.holes.axis100), E)
vh111 = map(E -> SolidStateDetectors.Vl(E, cdm.holes.axis111), E)

#cross check whether the results look reasonable by plotting (optional)
using Plots
plot(E, [ve100, ve111, vh100, vh111], 
    xlabel = "Electric field strength [V/m]", ylabel = "Drift velocity [m/s]")
