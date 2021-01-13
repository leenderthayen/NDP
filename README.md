# NDP -- Nab detector pipeline


## Purpose

Simulate Silicon detector response in a simplified version of the Nab detector geometry

## Installation


### Geant4


Compilation is performed using CMake. The CMake script forces an out of source build (i.e. make a separate build folder).

Note that right now the choice of output format (HDF5/ROOT) is determined at compile time. In the project Geant4 simulation itself this can be changed in the NDDAnalysis.hh file by modifying the include.

In order to enable HDF5, your local Geant4 installation should have HDF5 enabled, which puts constraints on your local HDF5 installation. This is still flagged as experimental by the G4 documentation, and so depends on your own experience. If you run into trouble, switch to ROOT or get in touch.

### SSD

Follow the general Julia procedure, i.e. use the Manifest.toml and Project.toml files.

Open julia, press ']' to go into package manager mode, and type 'activate .'. This activates the local environment. Type backspace to go back to julia prompt, and type 'using Pkg; Pkg.instantiate()' to finish the local install of the required packages.

Run 'MainScript.jl' or make your own.

We are working on a separate fork of the SSD.jl package at times. Use the package manager to choose which one to use if you are missing functionality.

### Make detector and CCD

make_simple_detector.py and makeCCD.py take user defined parameters to create detectors similar in design to the detectors used in the Nab experiment. make_simple_detector.py defines the geometries of the bulk silicon and contacts. makeCCD.py defines the impurity density profile of the bulk silicon. The IDP includes any depostions in the bulk (p+, n+, pstop or pspray). To run a simulation using these scripts first run them using python3: 

    python3 config_files/make_simple_detector.py
    
    Number of pixels: 

    Detector height(mm): 

    Pixel size(mm): 

    Pixel seperation: 

    grid size(xstart,xend,ystart,yend): 
    
    
    
    python3 config_files/makeCCD.py
    
    Config file name: 

    Number of pixels: 

    Detector height(mm): 

    Pixel size(mm): 

    Pixel seperation: 

    pspray width: 

    pspray thickness: 

    pspray impurity concentration: 

    p+ thickness: 

    p+ impurity concentration: 

    n+ thickness: 

    n+ impurity concentration: 

    Bulk impurity concentration: 

    IDP gradient(None, Gauss, Erf): 

    Radial IDP gradient(None, Linear, Erf): 
    
The user will be asked for a number of paramaters, all of which can be skipped by pressing 'enter', in which case they will be set to default values. All units are in mm. make_simple_detector.py will create a file named "Si **N**Hexagon **H**-**D**-**s**.json" where N is the number of pixels, H is the detector height, D is the pixel size, and s is the pixel seperation. There are to many parameters to automatically name the CCD files so the user must specify a name. The CCD file extension will be .jl.

To setup a simulation use 'SetupSimulation("config_files/Si **N**Hexagon **H**-**D**-**s**.json","config_files/CCDName.jl")' which is defined in SetupSSD.jl. These files can also be used with 'BiasVariation()' in SummaryReport.jl where the CCD file name is added after all other parameters.

You can also use these files in your own scripts. The detector file is added in the normal SSD way, and the CCD file is added by including the file and defining the detector's bulk silicon CCD:
```julia
    using SolidStateDetectors
    using IntervalSets
    
    include("config_files/CCDName.jl")
    
    simulation = Simulation{T}("config_files/Si **N**Hexagon **H**-**D**-**s**.json")
    
    #this sets the charge density model (cdm) to the custom ccd defined in CCDName.jl
    simulation.detector.semiconductors[1].charge_density_model = cdm
    
    #rest of your code
```

If you find any bugs or have any suggestions for improvements to makeCCD.py or make_simple_detector.py reach out to rjtayl.
