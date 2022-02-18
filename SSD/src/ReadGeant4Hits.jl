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
    eventID::Array{Int64, 1} = read(fid, "ntuple/hits/eventID/pages")
    trackID::Array{Int64, 1} = read(fid, "ntuple/hits/trackID/pages")
    # All distances in mm
    x::Array{Float64, 1} = read(fid, "ntuple/hits/x/pages")
    y::Array{Float64, 1} = read(fid, "ntuple/hits/y/pages")
    z::Array{Float64, 1} = read(fid, "ntuple/hits/z/pages")
    eDep::Array{Float64, 1} = read(fid, "ntuple/hits/eDep/pages")
    particle::Array{String, 1} = read(fid, "ntuple/hits/particle/pages")

    df = DataFrame(eID = eventID, tID = trackID, X = x, Y = y, Z = z, E = eDep, p = particle)

    return groupby(df, :eID)
end

function GetROOTHitInformation(filename::String)::GroupedDataFrame
    # Functional, but might be crappy performance. Might look into lazy evaluation?
    @info "Reading Geant4 Hits info from ROOT $filename"
    file = TFile(filename)
    t = file["ntuple/hits"]

    df = DataFrame(eID = t.eventID, tID = t.trackID, X = t.x, Y = t.y, Z = t.z, E = t.eDep, p = t.particle)

    return groupby(df, :eID)
end
