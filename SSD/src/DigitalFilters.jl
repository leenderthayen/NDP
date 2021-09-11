using Random

function ExtendWaveform(x::AbstractArray, dt::Real, endTime::Real)::AbstractArray
    return vcat(x, [x[end] for i in 1:(endTime/dt-length(x))])
end

function PadWaveform(x::AbstractArray, dt::Real, startTime::Real, endTime::Real)::AbstractArray
	 y = vcat([x[1] for i in 1:(startTime/dt)],x)
	 return ExtendWaveform(y, dt, endTime)
end

function LowPassFilter(x::AbstractArray, RC::Real, dt::Real)::AbstractArray
    alpha = RC/(RC+dt)

    y = zeros(length(x))
    y[1] = alpha*x[1]
    for i = 2:length(x)
        y[i] = y[i-1] + alpha*(x[i]-y[i-1])
    end
    return y
end

function HighPassFilter(x::AbstractArray, RC::Real, dt::Real)::AbstractArray
    alpha = RC/(RC+dt)

    y = zeros(length(x))
    y[1] = x[1]
    for i = 2:length(x)
        y[i] = alpha*(y[i-1] + x[i] - x[i-1])
    end
    return y
end

#Convolution of the filter over the waveform, so the filter looks flipped
function PrepTrapFilter(Rise::Real, FTop::Real, Tau::Real)::AbstractArray
    y = zeros(2*Rise+FTop)

    for i = 1:Rise
        y[i] = i - Tau
        y[i + Rise + FTop] = Tau + Rise - i
    end
    for i = 1:FTop
        y[i+Rise] = Rise
    end
    return y
end

function TrapezoidalFilter(x::AbstractArray, t::AbstractArray, scale::Real)::AbstractArray
    y = zeros(length(x) - length(t))

    for i = 1:(length(x) - length(t))
        dummy = 0.0;
        for j = 1:length(t)
            dummy += x[i+j] * t[j]
        end
        y[i] = dummy/scale
    end
    return y
end

function RunTrapFilter(x::AbstractArray, Rise::Real, FTop::Real, Tau::Real)::AbstractArray
    y = PrepTrapFilter(Rise, FTop, Tau) #Nominal values: Rise = 800.0, FTop = 400.0, Tau = 5000.0, assuming 1 ns resolution
    y1 = TrapezoidalFilter(x,y,(Rise*Tau))

    return y1
end

function SampleNoise(x::AbstractArray, Freq::AbstractArray, Amp::AbstractArray; dt=1e-9)::AbstractArray
    Random.seed!()
    y = copy(x)
    for i = 1:length(Freq)
        rphase = rand()*2.0*pi
	#@info rphase
        for j = 1:length(x)
            delt = j
            y[j] += Amp[i]*cos((2.0*pi*Freq[i]*delt*dt)-rphase)
        end
    end
    return y
end

function CustomFilter(x::AbstractArray, dt::Real)::AbstractArray
    y1 = HighPassFilter(x, 5e-6, dt)
    y2 = LowPassFilter(y1, 7e-9, dt)
    y3 = LowPassFilter(y2, 7e-9, dt)
    #y3 = HighPassFilter(y2, 8e-9, dt)
    #y4 = LowPassFilter(y3, 10e-9, dt)

    return y3
end

function CustomNoisyFilter(x::AbstractArray, Freq::AbstractArray, Amp::AbstractArray, dt::Real)::AbstractArray
    y1 = HighPassFilter(x, 5e-6, dt)
    y2 = LowPassFilter(y1, 7e-9, dt)
    y3 = LowPassFilter(y2, 7e-9, dt)
    y4 = SampleNoise(y3, Freq, Amp, dt=dt)

    return y4
end

function Integrate(x::AbstractArray, dt::Real)::AbstractArray
    y = zeros(length(x))
    for i = 2:length(x)
        y[i] = y[i-1] + x[i]*dt
    end
    return y
end

function TestFilters()
    x = zeros(4000)
    offset = 10
    x[offset:offset+100] .= 1.0
    x[offset:offset+20] .= 1.5

    y = Integrate(x, 0.1)
    wf = CustomFilter(y, 1e-10)

    return x, y, wf
end
