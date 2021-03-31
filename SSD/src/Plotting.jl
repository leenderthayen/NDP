using Printf
using Plots; pyplot(fmt = :png)
using SolidStateDetectors
using Unitful
using StatsPlots

include("WaveformAnalysis.jl")

figSize = (1400, 1000)
extension = ".png"

function GetDetectorFacePosition(sim::Simulation)
    #thickness = sim.detector.semiconductors[1].geometry.h
    thickness = 2e-3
    #z = sim.detector.semiconductors[1].geometry.translate.z
    z = thickness/2
    zBottom = z - thickness/2
    zTop = z + thickness/2

    return (zBottom, zTop)
end

function PlotEvents(events::Vector{Event}, dir::String="", prefix::String="", suffix::String="")::Nothing
    if !isempty(events)
        try
            for i in 1:size(events[1].waveforms, 1) # Loop over contacts
                # Plot integrated charge
                pIC = plot(events[1].waveforms[1].time / 1u"ns", [x.waveforms[i].value for x in events],
                xlabel="Time [ns]", ylabel="Integrated charge [arb.]", linecolor="red",
                linealpha=1/sqrt(length(events)), legend=false)

                #savefig(dir * prefix * "integrated_charge_contact_$i" * suffix * extension)

                # Histogram rise times

                riseTimes = CollectRiseTimes(events)
                pRT = histogram(riseTimes / 1u"ns", bins=:scott, xlabel="Rise time [ns]", ylabel="Counts [arb.]")

                #savefig(dir * prefix * "risetime_hist_$i" * suffix * extension)

                # Histogram maximal integrated current

                maxAmplitude = [maximum(abs.(x.waveforms[i].value)) for x in events]
                # println(maxAmplitude)
                # println(length(riseTimes), length(maxAmplitude))
                pMA = histogram(maxAmplitude, bins=:scott, xlabel="Maximal amplitude", ylabel="Counts [arb.]")

                #savefig(dir * prefix * "amplitude_hist_$i" * suffix * extension)

                # 2D Amplitude - Rise time interpolated scatter plot
                pKDE = marginalkde(riseTimes / 1u"ns", maxAmplitude)

                #savefig(dir * prefix * "rise_time_amplitude_kde_$i" * suffix * extension)

                plot(pRT, pIC, pKDE, pMA, layout=(2, 2), size=figSize)

                savefig(dir * prefix * "event_contact_$i" * suffix * extension)
            end
        catch

        end
    end
end

function PlotGeometry(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    @info "Plotting detector geometry"
    plot(sim.detector, size=figSize)

    savefig(dir * prefix * "geometry" * suffix * extension)
end

function PlotElectricPotential(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    zBottom, thickness = GetDetectorFacePosition(sim)
    zTop = zBottom + thickness
    if !ismissing(sim.electric_potential)
        plot(plot(sim.electric_potential, y=0, aspect_ratio = :none), plot(sim.electric_potential, x=0, aspect_ratio = :none),
        plot(sim.electric_potential, z=zBottom), plot(sim.electric_potential, z=zTop),
        layout=(2, 2), size=figSize)

        savefig(dir * prefix * "electric_potential" * suffix * extension)
    end
end

function Plot2DWeightingPotential(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    zBottom, zTop = GetDetectorFacePosition(sim)
    for i in 1:size(sim.weighting_potentials, 1)
        if !ismissing(sim.weighting_potentials[i])
            plot(plot(sim.weighting_potentials[i], y=0, aspect_ratio = :none), plot(sim.weighting_potentials[i], x=0, aspect_ratio = :none),
            plot(sim.weighting_potentials[i], z=zBottom), plot(sim.weighting_potentials[i], z=zTop),
            layout=(2, 2), size=figSize)

            p = @sprintf "weighting_potential_%d" i

            savefig(dir * prefix * p * suffix * extension)
        end
    end
end

function PlotElectricField(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    zBottom, zTop = GetDetectorFacePosition(sim)
    if !ismissing(sim.electric_field)
        p_xz = plot(sim.electric_field, y=0, aspect_ratio = :none)
        plot_electric_fieldlines!(sim)
        p_yz = plot(sim.electric_field, x=0, aspect_ratio = :none)
        plot_electric_fieldlines!(sim)
        p_xy_bottom = plot(sim.electric_field, z=zBottom)
        p_xy_top = plot(sim.electric_field, z=zTop)
        plot(p_xz, p_yz, p_xy_bottom, p_xy_top, layout=(2, 2), size=figSize)

        savefig(dir * prefix * "electric_field" * suffix * extension)
    end
end

function PlotFields(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    @info "Plotting fields"
    PlotElectricPotential(sim, dir, prefix, suffix)
    PlotWeightingPotential(sim, dir, prefix, suffix)
    PlotElectricField(sim, dir, prefix, suffix)
end

function PlotChargeDensity(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    zBottom, zTop = GetDetectorFacePosition(sim)
    if !ismissing(sim.ρ)
        plot(plot(sim.ρ, x=0, aspect_ratio = :none), plot(sim.ρ, y=0, aspect_ratio = :none), plot(sim.ρ, z=zBottom), plot(sim.ρ, z=zTop), layout=(2,2), size=figSize)
        savefig(dir * prefix * "rho" * suffix * extension)
    end
end

function PlotDielectric(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    zBottom, zTop = GetDetectorFacePosition(sim)
    if !ismissing(sim.ϵ)
        plot(plot(sim.ϵ, x=0, aspect_ratio = :none), plot(sim.ϵ, y=0, aspect_ratio = :none), plot(sim.ϵ, z=zBottom), plot(sim.ϵ, z=zTop), layout=(2,2), size=figSize)
        savefig(dir * prefix * "epsilon" * suffix * extension)
    end
end

function PlotPointType(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    zBottom, zTop = GetDetectorFacePosition(sim)
    if !ismissing(sim.point_types)
        plot(plot(sim.point_types, x=0, aspect_ratio = :none), plot(sim.point_types, y=0, aspect_ratio = :none), plot(sim.point_types, z=zBottom), plot(sim.point_types, z=zTop), layout=(2,2), size=figSize)
        savefig(dir * prefix * "point_types" * suffix * extension)
    end
end

function PlotMaterialProperties(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    @info "Plotting material properties"

    PlotChargeDensity(sim, dir, prefix, suffix)
    PlotDielectric(sim, dir, prefix, suffix)
    PlotPointType(sim, dir, prefix, suffix)

    zBottom, zTop = GetDetectorFacePosition(sim)

    if !(ismissing(sim.ρ) || ismissing(sim.ϵ) || ismissing(sim.point_types))
        plot(plot(sim.ρ, x=0), plot(sim.ϵ, x=0), plot(sim.point_types, x=0), layout=(1, 3), size=figSize)
        savefig(dir * prefix * "material_properties_yz" * suffix * extension)
        plot(plot(sim.ρ, y=0), plot(sim.ϵ, y=0), plot(sim.point_types, y=0), layout=(1, 3), size=figSize)
        savefig(dir * prefix * "material_properties_xz" * suffix * extension)
        plot(plot(sim.ρ, z=zBottom), plot(sim.ϵ, z=zBottom), plot(sim.point_types, z=zBottom), layout=(1, 3), size=figSize)
        savefig(dir * prefix * "material_properties_xy_bottom" * suffix * extension)
        plot(plot(sim.ρ, z=zTop), plot(sim.ϵ, z=zTop), plot(sim.point_types, z=zTop), layout=(1, 3), size=figSize)
        savefig(dir * prefix * "material_properties_xy_top" * suffix * extension)
    end
end

function PlotCV(sims::AbstractVector{Simulation}, dir::String="", prefix::String="", suffix::String="")::Nothing
    cv = zeros(size(sims, 1), 2)
    for i in 1:size(sims, 1)
        cv[i, 1] = sims[i].detector.contacts[1].potential
        cv[i, 2] = calculate_capacitance(sims[i]) / 1u"pF"
    end

    sort!(cv, dims=1)

    pCV = plot(cv[:, 1], cv[:, 2], xlabel="Voltage [V]", ylabel="Capacitance [pF]", size=figSize)

    savefig(dir * prefix * "cv" * suffix * extension)

    dCm2dV = zeros(size(sims, 1)-1, 2)
    for i in 1:size(dCm2dV, 1)
        dCm2dV[i, 1] = cv[i, 1]
        dCm2dV[i, 2] = ((cv[i+1, 2])^(-2)-cv[i, 2]^(-2))/(cv[i+1, 1] - cv[i, 1])
    end

    pDerivCV = plot(dCm2dV[:, 1], dCm2dV[:, 2], xlabel="Voltage [V]", ylabel="d(1/C^2)/dV", size=figSize)

    savefig(dir * prefix * "deriv_cv" * suffix * extension)

    plot(pCV, pDerivCV, layout=(1, 2), size=figSize)

    savefig(dir * prefix * "cv_deriv_cv" * suffix * extension)
end

function Plot1DWeightingPotential(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    # Edge and corner plots only work for hexagons
    @info "Plotting weighting potentials vs z"

    for i in 1:length(sim.weighting_potentials)
        grid = sim.weighting_potentials[i].grid
        gridX = collect(grid.x)
        gridY = collect(grid.y)
        # R = sim.detector.contacts[i].geometry.rOuter
        # r = sqrt(3)*R/2
        # edge = 0.9.*(r, 0.0) # only works for center pixel now, expand
        # corner = 0.9.*(r, R/2)

        edge = (0, 6e-3)
        corner = (6e-3, 0)

        thresholdX = maximum(gridX[2:end]-gridX[1:end-1])/2
        thresholdY = maximum(gridY[2:end]-gridY[1:end-1])/2

        println(thresholdX, " ", thresholdY)
        x0 = findlast(x->x==0, collect(grid.x))
        y0 = findlast(x->x==0, collect(grid.y))
        xc = findlast(x->abs(x-corner[1]) < thresholdX, collect(gridX))
        yc = findlast(x->abs(x-corner[2]) < thresholdY, collect(gridY))
        xe = findlast(x->abs(x-edge[1]) < thresholdX, collect(gridX))
        ye = findlast(x->abs(x-edge[2]) < thresholdY, collect(gridY))

        println(grid[x0, y0, 1])
        println(grid[xe, ye, 1])
        println(grid[xc, yc, 1])

        wp0 = sim.weighting_potentials[i][x0, y0, :]
        wpe = sim.weighting_potentials[i][x0, ye, :]
        wpc = sim.weighting_potentials[i][xc, y0, :]

        plot(collect(grid.z), [wp0, wpe, wpc], labels = ["Center" "Edge" "Corner"],
        xlabel = "Z [m]", ylabel="Weighting potential", size=figSize)

        savefig(dir * prefix * "weighting_potential_vs_z_$i" * suffix * extension)
    end
end

function Plot1DWeightingPotentialRange(sim::Simulation, initPos::Real, finalPos::Real,length::Integer,contact::Integer=2, dir::String="", prefix::String="", suffix::String="")::Nothing

    @info "Plotting weighting potentials vs z"

    grid = sim.weighting_potentials[contact].grid
    gridX = collect(grid.x)
    gridY = collect(grid.y)

    edge = 6e-3
    tolerance = 5e-4

    thresholdX = maximum(gridX[2:end]-gridX[1:end-1])/2
    println(gridX[end])
    thresholdY = maximum(gridY[2:end]-gridY[1:end-1])/2
    y0 = findlast(x->x==0, collect(grid.y))
    
    wps = []
    posRange = range(initPos, stop=finalPos, length=length)
    for i in 1:size(posRange,1)
    	xi = posRange[i]
	println(xi)
	x0 = findlast(x->(abs(x-xi)<thresholdX && x<gridX[end]), collect(grid.x))
	
    	println(grid[x0, y0, 1])
        
        push!(wps,sim.weighting_potentials[contact][x0, y0, :])

    end
    
    posStr = map(x->string(x),posRange)
    poslabels = reshape(posStr, (1,size(posStr)[1])) 

    plot(collect(grid.z), wps, labels = poslabels, xlabel = "Z [m]", ylabel="Weighting potential", size=figSize)

    savefig(dir * prefix * "weighting_potential_vs_z_range$contact" * suffix * extension)
    
end

function PlotIDP(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    @info "Plotting IDP along z and top of detector"


    bulk = sim.detector.semiconductors[1]
    bulk_geom = bulk.geometry
    center = bulk_geom.translate === missing ? (0,0) : bulk_geom.translate
    R = bulk_geom.r_interval.right
    H = bulk_geom.z_interval.right

    zs = 0:1e-8:H
    charge_vecz = similar(zs)
    for iz in eachindex(zs)
        pt = CartesianPoint{T}(0,0,zs[iz])
        charge_vecz[iz] = pt in bulk ?
                            SolidStateDetectors.get_charge_density(bulk, pt) : 0
    end

    xs = -R:1e-7:R
    charge_vecx = similar(xs)
    charge_vecr = similar(xs)
    for ix in eachindex(xs)
        pt = CartesianPoint{T}(xs[ix]+center[1], 0, H-1e-6)
        charge_vecx[ix] = pt in bulk ?
                            SolidStateDetectors.get_charge_density(bulk, pt) : 0
        pt = CartesianPoint{T}(xs[ix] + center[1], 0, H/2)
        charge_vecr[ix] = pt in bulk ? SolidStateDetectors.get_charge_density(bulk, pt) : 0
    end

    plot(plot(zs, charge_vecz), plot(xs, charge_vecx), plot(xs, charge_vecr), layout=(1, 3), size=figSize)
    savefig(dir * prefix * "IDP_z_and_top_and_r" * suffix * extension)
end
