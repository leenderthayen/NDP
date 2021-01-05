using Printf
using Plots; pyplot(fmt = :png)
using SolidStateDetectors
using Unitful

figSize = (1400, 1000)
extension = ".png"

function PlotEvents(events::Vector{Event}, dir::String="", prefix::String="", suffix::String="")::Nothing
    for i in 1:size(events[1].waveforms, 1) # Loop over contacts
        plot(events[1].waveforms.time / 1u"ns", [x.waveforms[i].value for x in events],
        xlabel="Time [ns]", ylabel="Integrated charge [arb.]")

        savefig(dir * prefix * "integrated_charge_contact_$i" * suffix * extension)
    end
end

function PlotGeometry(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    @info "Plotting detector geometry"
    plot(sim.detector)

    savefig(dir * prefix * "detector" * suffix * extension)
end

function PlotElectricPotential(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    thickness = 2.0e-3
    if !ismissing(sim.electric_potential)
        plot(plot(sim.electric_potential, y=0), plot(sim.electric_potential, x=0),
        plot(sim.electric_potential, z=0), plot(sim.electric_potential, z=thickness),
        layout=(2, 2), size=figSize)

        savefig(dir * prefix * "electric_potential" * suffix * extension)
    end
end

function PlotWeightingPotential(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    thickness = 2.0e-3
    for i in 1:size(sim.weighting_potentials, 1)
        if !ismissing(sim.weighting_potentials[i])
            plot(plot(sim.weighting_potentials[i], y=0), plot(sim.weighting_potentials[i], x=0),
            plot(sim.weighting_potentials[i], z=0), plot(sim.weighting_potentials[i], z=thickness),
            layout=(2, 2), size=figSize)

            p = @sprintf "weighting_potential_%d" i

            savefig(dir * prefix * p * suffix * extension)
        end
    end
end

function PlotElectricField(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    thickness = 2.0e-3
    if !ismissing(sim.electric_field)
        p_xz = plot(sim.electric_field, y=0)
        plot_electric_fieldlines!(sim)
        p_yz = plot(sim.electric_field, x=0)
        plot_electric_fieldlines!(sim)
        p_xy_bottom = plot(sim.electric_field, z=0)
        p_xy_top = plot(sim.electric_field, z=thickness)
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
    thickness = 2.0e-3
    if !ismissing(sim.ρ)
        plot(plot(sim.ρ, x=0), plot(sim.ρ, y=0), plot(sim.ρ, z=0), plot(sim.ρ, z=thickness), layout=(2,2), size=figSize)
        savefig(dir * prefix * "rho" * suffix * extension)
    end
end

function PlotDielectric(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    thickness = 2.0e-3
    if !ismissing(sim.ϵ)
        plot(plot(sim.ϵ, x=0), plot(sim.ϵ, y=0), plot(sim.ϵ, z=0), plot(sim.ϵ, z=thickness), layout=(2,2), size=figSize)
        savefig(dir * prefix * "epsilon" * suffix * extension)
    end
end

function PlotPointType(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    thickness = 2.0e-3
    if !ismissing(sim.point_types)
        plot(plot(sim.point_types, x=0), plot(sim.point_types, y=0), plot(sim.point_types, z=0), plot(sim.point_types, z=thickness), layout=(2,2), size=figSize)
        savefig(dir * prefix * "point_types" * suffix * extension)
    end
end

function PlotMaterialProperties(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    @info "Plotting material properties"

    PlotChargeDensity(sim, dir, prefix, suffix)
    PlotDielectric(sim, dir, prefix, suffix)
    PlotPointType(sim, dir, prefix, suffix)

    thickness = 2.0e-3

    if !(ismissing(sim.ρ) || ismissing(sim.ϵ) || ismissing(sim.point_types))
        plot(plot(sim.ρ, x=0), plot(sim.ϵ, x=0), plot(sim.point_types, x=0), layout=(1, 3), size=figSize)
        savefig(dir * prefix * "material_properties_yz" * suffix * extension)
        plot(plot(sim.ρ, y=0), plot(sim.ϵ, y=0), plot(sim.point_types, y=0), layout=(1, 3), size=figSize)
        savefig(dir * prefix * "material_properties_xz" * suffix * extension)
        plot(plot(sim.ρ, z=0), plot(sim.ϵ, z=0), plot(sim.point_types, z=0), layout=(1, 3), size=figSize)
        savefig(dir * prefix * "material_properties_xy_bottom" * suffix * extension)
        plot(plot(sim.ρ, z=thickness), plot(sim.ϵ, z=thickness), plot(sim.point_types, z=thickness), layout=(1, 3), size=figSize)
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

    plot(cv[:, 1], cv[:, 2], xlabel="Voltage [V]", ylabel="Capacitance [pF]")

    savefig(dir * prefix * "cv" * suffix * extension)

    dCm2dV = zeros(size(sims, 1)-1, 2)
    for i in 1:size(dCm2dV, 1)
        dCm2dV[i, 1] = cv[i, 1]
        dCm2dV[i, 2] = ((cv[i+1, 2])^(-2)-cv[i, 2]^(-2))/(cv[i+1, 1] - cv[i, 1])
    end

    plot(dCm2dV[:, 1], dCm2dV[:, 2], xlabel="Voltage [V]", ylabel="d(1/C^2)/dV")

    savefig(dir * prefix * "deriv_cv" * suffix * extension)
end

function PlotWP(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    #Edge and corner plots only work for hexagons
    @info "Plotting weighting potentials vs z"
    
    WP = sim.weighting_potentials[2]
    grid = WP.grid
    Lx, Ly, Lz = size(grid)
    R = sim.detector.contacts[2].geometry.rOuter
    r = sqrt(3)*R/2
    edge = (r, 0.0)
    corner = (r, R/2)

    x0 = 0
    y0 = 0
    xe = 0
    ye = 0
    xc = 0
    yc = 0


    for i in 1:Lx
        if grid[i,1,1][1] == 0
           x0 = i
        end
    end


    for i in 1:Ly
        if grid[1,i,1][2] == 0
           y0 = i
        end
    end

    for i in 1:Lx
        if grid[i,1,1][1] -edge[1] < 0.000001
           xe = i
        end
    end
    
    ye = y0
    
    for i in 1:Lx
        if grid[i,1,1][1] -corner[1] < 0.000001
           xc = i
        end
    end
    
    for i in 1:Ly
        if grid[1,i,1][2] -corner[2] < 0.000001
           yc = i
        end
    end

    is = 1:1:Lz
    wp0 = convert(Array{T},similar(is))
    wpe = convert(Array{T},similar(is))
    wpc = convert(Array{T},similar(is))

    for i in is
        wp0[i] = WP[x0,y0,i]
        wpe[i] = WP[xe,ye,i]
        wpc[i] = WP[xc,yc,i]
    end

    zs = convert(Array{T},similar(is))

    for i in eachindex(is)
        zs[i] = grid[x0,y0,i][3]
    end
    
    plot(plot(zs, wp0, title="Center"), plot(zs, wpe, title="Edge"), plot(zs, wpc, title="Corner"), layout=(1, 3), size=figSize)
    savefig(dir * prefix * "weighting_potential_vs_z" * suffix * extension)
end
  
    
