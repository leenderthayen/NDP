using HDF5
using UpROOT
using DataFrames

function GetHitInformation(filename::String)::Union{Nothing, GroupedDataFrame}
    extension = filename[findlast(isequal('.'), filename):end]

    if extension == ".root"
        return GetROOTHitInformation(filename)
    elseif extension == ".hdf5"
        return GetHDF5HitInformation(filename)
    end
    return nothing
end

function GetHDF5HitInformation(filename::String)::GroupedDataFrame
    @info "Reading Geant4 Hits info from HDF5"
    fid = h5open(filename, "r")
    id::Array{Int64, 1} = read(fid, "ntuple/hits/iD/pages")
    # All distances in mm
    x::Array{Float64, 1} = read(fid, "ntuple/hits/x/pages")
    y::Array{Float64, 1} = read(fid, "ntuple/hits/y/pages")
    z::Array{Float64, 1} = read(fid, "ntuple/hits/z/pages")
    eDep::Array{Float64, 1} = read(fid, "ntuple/hits/eDep/pages")

    df = DataFrame(ID = id, X = x, Y = y, Z = z, E = eDep)

    return groupby(df, :ID)
end

function GetROOTHitInformation(filename::String)::GroupedDataFrame
    # Functional, but might be crappy performance. Might look into lazy evaluation?
    @info "Reading Geant4 Hits info from ROOT $filename"
    file = TFile(filename)
    t = file["ntuple/hits"]

    df = DataFrame(ID = t.iD, X = t.x, Y = t.y, Z = t.z, E = t.eDep)

    return groupby(df, :ID)
end
