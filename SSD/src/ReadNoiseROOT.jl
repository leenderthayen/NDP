using UpROOT

function GetNoiseROOT(filename::String, AmpScale::Real)::Array{Array{Float64,1},2}
    # Functional, but might be crappy performance. Might look into lazy evaluation?
    @info "Reading Noise Spectra from ROOT $filename"
    file = TFile(filename)
    t = file["TrFFT"]

    TiD = t.SSDiD
    TFreq = t.Freq
    Temps = t.Amp

    nentries = length(Temps)
    lWF = length(t.Amp[1])

    @info "There are $nentries entries"
    @info "Each list is $lWF long"

    FreqAmp = fill(Float64[],(130,2))

    for i = 1:nentries
        FreqAmp[TiD[i],1] = TFreq[i]
        FreqAmp[TiD[i],2] = Temps[i]*AmpScale
    end

    @info "Read Done"

    return FreqAmp
end
