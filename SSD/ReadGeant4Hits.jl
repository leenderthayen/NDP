using HDF5
using DataFrames

function GetHitInformation(filename::String)::GroupedDataFrame
    fid = h5open(filename, "r")
    id::Array{Int64, 1} = read(fid, "ntuple/hits/iD/pages")
    x::Array{Float64, 1} = read(fid, "ntuple/hits/x/pages")
    y::Array{Float64, 1} = read(fid, "ntuple/hits/y/pages")
    z::Array{Float64, 1} = read(fid, "ntuple/hits/z/pages")
    eDep::Array{Float64, 1} = read(fid, "ntuple/hits/eDep/pages")

    df = DataFrame(ID = id, X = x, Y = y, Z = z, E = eDep)

    return groupby(df, :ID)
end
