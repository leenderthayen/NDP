using RadiationDetectorSignals

function CalculateRiseTime(t, v::Array, low::Real, high::Real)
    tLow = 0
    tHigh = 0
    
    v = abs.(v)
    vMax = findmax(v)[1]
    for i in 1:length(v)
        if v[i] >= low * vMax
            tLow = t[i]
            break
        end
    end
    for i in length(v):-1:1
        if v[i] <= high * vMax
            tHigh = t[i]
            break
        end
    end
    return tHigh-tLow
end

function CalculateRiseTime(w::RadiationDetectorSignals.RDWaveform, low::Real, high::Real)
    return CalculateRiseTime(w.time, abs.(w.value), low, high)
end

function CollectRiseTimes(events, low = 0.1, high = 0.9; contact = 1)
    riseTimes = Vector{Unitful.Time}()
    for i in 1:length(events)
        push!(riseTimes, CalculateRiseTime(events[i].waveforms[contact], low, high))
    end
    return riseTimes
end

function CollectRiseTimesFiltered(events, low = 0.1, high = 0.9; dt = 1e-9, contact = 1)
    riseTimes = Vector{Unitful.Time}()
    for i in 1:length(events)
        push!(riseTimes, CalculateRiseTime(events[i].waveforms[contact].time, CustomFilter(events[i].waveforms[contact].value, dt), low, high))
    end
    return riseTimes
end

function CollectRiseTimesNoisyFiltered(events, noises, low = 0.1, high = 0.9; dt = 1e-9, contact = 1, pixel=64)
    riseTimes = Vector{Unitful.Time}()
    for i in 1:length(events)
    #for i in 1:10
    	 @info i 
        push!(riseTimes, CalculateRiseTime(events[i].waveforms[contact].time, CustomNoisyFilter(events[i].waveforms[contact].value, noises[pixel,1] ,noises[pixel,2] , dt), low, high))
    end
    return riseTimes
end


function CollectRiseTimesOnDetector(events, detector, low = 0.1, high = 0.9;contact = 2)
    riseTimes = Vector{Unitful.Time}()
    for i in 1:length(events)
    	if in(events[i].locations[1], detector.contacts[1].geometry)
           push!(riseTimes, CalculateRiseTime(events[i].waveforms[contact], low, high))
        end
    end
    return riseTimes
end

function CollectRiseTimesFilteredOnDetector(events,detector, low = 0.1, high = 0.9; dt = 1e-9, contact = 2)
    riseTimes = Vector{Unitful.Time}()
    for i in 1:length(events)
    	if in(events[i].locations[1], detector.contacts[1].geometry)
           push!(riseTimes, CalculateRiseTime(events[i].waveforms[contact].time, CustomFilter(events[i].waveforms[contact].value, dt), low, high))
    	end
    end
    return riseTimes
end

function CollectRiseTimesNoisyFilteredOnDetector(events, noises, detector, low = 0.1, high = 0.9; dt = 1e-9, contact = 1, pixel=64)
    riseTimes = Vector{Unitful.Time}()
    for i in 1:length(events)
    	if in(events[i].locations[1], detector.contacts[1].geometry)
           push!(riseTimes, CalculateRiseTime(events[i].waveforms[contact].time, CustomNoisyFilter(events[i].waveforms[contact].value, noises[pixel,1] ,noises[pixel,2] , dt), low, high))
    	end
    end
    return riseTimes
end

function CollectEnergiesFilteredOnDetector(events,detector; dt = 1e-9, contact = 2)
    energies = Vector()
    for i in 1:length(events)
    	if in(events[i].locations[1], detector.contacts[1].geometry)
           push!(energies, findmax(abs.(CustomFilter(events[i].waveforms[contact].value,dt)))[1])
    	end
    end
    return energies
end

