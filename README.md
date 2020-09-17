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
