using SolidStateDetectors

T = Float32
sim = Simulation{T}(SSD_examples[:CGD])
calculate_electric_potential!(sim,
    max_refinements = 0,
    max_n_iterations = 10,
    init_grid_spacing =  T.((0.01, 0.01, 0.01)),
)

println("Finished first sim")
g = sim.electric_potential.grid
ax1, ax2, ax3 = g.axes
user_additional_ticks_ax1 = T[0.002123, 0.002133, 0.002143, 0.002023, 0.002167] # here add your ticks of interest
user_additional_ticks_ax2 = T[0.003123] # here add your ticks of interest
user_additional_ticks_ax3 = T[0.004123] # here add your ticks of interest
user_ax1 = typeof(ax1)(ax1.interval, sort(vcat(ax1.ticks, user_additional_ticks_ax1)))
user_ax2 = typeof(ax2)(ax2.interval, sort(vcat(ax2.ticks, user_additional_ticks_ax2)))
user_ax3 = typeof(ax3)(ax3.interval, sort(vcat(ax3.ticks, user_additional_ticks_ax3)))
user_g = typeof(g)((user_ax1, user_ax2, user_ax3))

calculate_electric_potential!(sim, grid = user_g)
