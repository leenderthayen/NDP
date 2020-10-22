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

function CustomFilter(x::AbstractArray, dt::Real)::AbstractArray
    y1 = HighPassFilter(x, 1e-3, dt)
    y2 = LowPassFilter(y1, 4e-6, dt)
    y3 = HighPassFilter(y2, 8e-9, dt)
    y4 = LowPassFilter(y3, 10e-9, dt)

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
    x = zeros(400)
    offset = 10
    x[offset:offset+10] .= 1.0
    x[offset:offset+2] .= 1.5

    y = Integrate(x, 1)
    wf = CustomFilter(y, 1e-9)

    return x, y, wf
end
