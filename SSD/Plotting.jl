using Printf
using Plots; pyplot(fmt = :png)
using SolidStateDetectors
using Unitful

figSize = (1400, 1000)
extension = ".png"

function PlotGeometry(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    @info "Plotting detector geometry"
    plot(sim.detector)

    savefig(dir * prefix * "detector" * suffix * extension)
end

function PlotFields(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    @info "Plotting fields"

    thickness = 2.0e-3
    if !ismissing(sim.electric_potential)
        plot(plot(sim.electric_potential, y=0), plot(sim.electric_potential, x=0),
        plot(sim.electric_potential, z=0), plot(sim.electric_potential, z=thickness),
        layout=(2, 2), size=figSize)

        savefig(dir * prefix * "electric_potential" * suffix * extension)
    end

    for i in 1:size(sim.weighting_potentials, 1)
        if !ismissing(sim.weighting_potentials[i])
            plot(plot(sim.weighting_potentials[i], y=0), plot(sim.weighting_potentials[i], x=0),
            plot(sim.weighting_potentials[i], z=0), plot(sim.weighting_potentials[i], z=thickness),
            layout=(2, 2), size=figSize)

            p = @sprintf "weighting_potential_%d" i

            savefig(dir * prefix * p * suffix * extension)
        end
    end

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

function PlotMaterialProperties(sim::Simulation, dir::String="", prefix::String="", suffix::String="")::Nothing
    @info "Plotting material properties"

    thickness = 2.0e-3
    if !ismissing(sim.ρ)
        plot(plot(sim.ρ, x=0), plot(sim.ρ, y=0), plot(sim.ρ, z=0), plot(sim.ρ, z=thickness), layout=(2,2), size=figSize)
        savefig(dir * prefix * "rho" * suffix * extension)
    end
    if !ismissing(sim.ϵ)
        plot(plot(sim.ϵ, x=0), plot(sim.ϵ, y=0), plot(sim.ϵ, z=0), plot(sim.ϵ, z=thickness), layout=(2,2), size=figSize)
        savefig(dir * prefix * "epsilon" * suffix * extension)
    end
    if !ismissing(sim.point_types)
        plot(plot(sim.point_types, x=0), plot(sim.point_types, y=0), plot(sim.point_types, z=0), plot(sim.point_types, z=thickness), layout=(2,2), size=figSize)
        savefig(dir * prefix * "point_types" * suffix * extension)
    end
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
