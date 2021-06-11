import numpy as np
import matplotlib.pyplot as plt

e = 1.6e-19 # electron charge
eps = 11.68*8.85e-12 # dielectric constant in Si

def get_total_capacitance(rMin, rMax, N_function, V, t, steps=100):
    r = np.linspace(rMin, rMax, steps)
    C = 0.
    for i in range(steps):
        tDep = np.sqrt(2*eps*V/e/N_function(r[i]))
        C += 2*np.pi*r[i]*eps/np.minimum(tDep, t)
    return C*(r[1]-r[0])

if __name__ == "__main__":
    rMin = 0
    rMax = 0.2
    steps = 100
    V = np.linspace(1, 150, steps)
    N0 = 2.5e16 # number density in m^-3
    slope = 1/rMax
    t = 0.002 # 2mm thick

    A = np.pi*rMax**2

    power = 1
    minSlope = 0.5/rMax**power
    maxSlope = 1.75/rMax**power
    slopes = np.linspace(minSlope, maxSlope, 5)
    fig, ax = plt.subplots(3, 1, sharex=True)
    figN, axN = plt.subplots(1, 1)
    figWN, axWN = plt.subplots(1, 1)
    for s in slopes:
        print(s)
        C = np.array(V)
        N_function = lambda r: N0*(1+s*r**power)
        N_function = lambda r: np.where(r < rMax/2, N0*(1-s*r**power), N0*(1+s*(r-rMax)**power))
        r = np.linspace(rMin, rMax, 100)
        axN.plot(r, N_function(r), label="Slope=%.1f" % (s*rMax**power))
        for i in range(len(C)):
            C[i] = get_total_capacitance(rMin, rMax, N_function, V[i], t)

        ax[0].plot(V, C, label="Slope=%.1f" % (s*rMax**power))
        ax[1].plot(V, 1/C**2)
        ax[2].plot(V, np.gradient(1/C**2, V[1]-V[0]))
        axWN.plot(eps/C*A, 2/e/eps/(np.gradient(A**2/C**2, V[1]-V[0])))
    ax[0].legend(loc=0)
    ax[2].set_xlabel("Bias voltage [V]")
    ax[0].set_yscale("log")
    ax[0].set_ylabel("Capacitance [F]")
    ax[1].set_ylabel("1/C^2")
    ax[2].set_ylabel("d(1/C^2)/dV")

    axN.set_xlabel("Radial distance [m]")
    axN.set_ylabel("Dopant concentration [m^-3]")
    axN.legend(loc=0)

    axWN.set_yscale("log")

    plt.show(block=True)
