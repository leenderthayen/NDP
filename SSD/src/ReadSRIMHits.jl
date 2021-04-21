using HDF5
using CSV
using DataFrames

function GetHitInformation(filename::String)::Union{Nothing, GroupedDataFrame}
    extension = filename[findlast(isequal('.'), filename):end]

    if extension == ".csv"
        return GetSRIMcsvHitInformation(filename)
    elseif extension in [".hdf", ".hdf5", ".h5"]
        return GetSRIMhdfHitInformation(filename)
    end
    return nothing
end

function GetSRIMhdfHitInformation(filename::String)::GroupedDataFrame
    @info "Reading SRIM hits info from $filename"
    fid = h5open(filename, "r")
    ions::Array{Int64, 1} = read(fid, "hits/ion")
    # All distances in angstroms
    # energy in keV
    x::Array{Float64, 1} = read(fid, "hits/X")
    y::Array{Float64, 1} = read(fid, "hits/Y")
    z::Array{Float64, 1} = read(fid, "hits/Z")
    eDep::Array{Float64, 1} = read(fid, "hits/E")

    df = DataFrame(ion = ions, X = x, Y = y, Z = z, E = eDep)
    # convert to mm
    df.X *= 1e-7
    df.Y *= 1e-7
    df.Z *= 1e-7

    return groupby(df, :ion)
end

function GetSRIMcsvHitInformation(filename::String)::GroupedDataFrame
    @info "Reading SRIM hits info from $filename"
    df = CSV.read(filename,DataFrame)
    # convert coordinates from angstroms to mm
    df.X *= 1e-7
    df.Y *= 1e-7
    df.Z *= 1e-7
    # energy in keV
    outdf = df[:, ["ion", "X", "Y", "Z", "E"]]
    return groupby(outdf, :ion)
end
