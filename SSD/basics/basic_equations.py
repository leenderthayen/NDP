import numpy as np

#Si

Egap0 = 1.17 #eV
alpha = 4.73e-4 #eV/K
beta = 636 # K

kB = 8.617e-5 # eV/K

def Egap(T):
    # Temperature-dependent bandgap energy
    return Egap0 - alpha*T**2/(T+beta)

def me(T):
    # Temperature-dependent effective elecron mass
    C = 0.1905
    mlm0 = 0.9163
    return 6**0.66*((C*Egap0/Egap(T))**2*mlm0)**0.33

def mh(T):
    # Temperature-dependent effective hole mass
    a = 0.44
    b = 0.36e-2
    c = 0.11e-3
    d = 0.126e-5
    e = 0.303e-8
    f = 0.468e-2
    g = 0.229e-3
    h = 0.747e-6
    i = 0.173e-8

    return ((a+b*T+c*T**2+d*T**3+e*T**4)/(1+f*T+g*T**2+h*T**3+i*T**4))**0.66

def Nc(T):
    # Temperature-dependent conduction density of states
    N = 2.541e19 # cm^-3
    return N*me(T)**1.5*(T/300.)**1.5

def Nv(T):
    # Temperature-dependent valence density of states
    N = 2.541e19 # cm^-3
    return N*mh(T)**1.5*(T/300)**1.5

def f(T, N, dE, e=True):
    # Temperature-dependent ionization fraction of impurities
    C = lambda T, N, dE: 4*N*np.exp(dE/(kB*T))/(Nc(T) if e else Nv(T))

    f = (-1+np.sqrt(1+2*C(T, N, dE)))/C(T, N, dE)

    return f

def EFermi(T, N, dE, p=True):
    # Temperature and impurity dependence of Fermi energy wrt intrinsic energy
    ni2 = Nc(T)*Nv(T)*np.exp(-Egap(T)/(kB*T))
    feff = f(T, N, dE)
    Neff = 0.5*(feff*N+np.sqrt((feff*N)**2+4*ni2))
    EFrel = -Egap(T)/2+kB*T*np.log(np.sqrt(Nc(T)*Nv(T))/Neff)
    return EFrel if p else -EFrel

if __name__ == "__main__":
    import matplotlib.pyplot as plt

    T = np.linspace(1, 450, 100)

    dE = 0.04 # energy gap between dopant and band, eV

    fig, ax = plt.subplots(1, 1)

    ax.plot(T, me(T), label="Electron")
    ax.plot(T, mh(T), label="Hole")
    ax.set_xlabel("Temperature [K]")
    ax.set_ylabel("m/m0")
    ax.legend(loc=0)

    fig, ax = plt.subplots(1, 1)
    ax.plot(T, Egap(T))
    ax.set_ylabel("Egap [eV]")
    ax.set_xlabel("Temperature [K]")

    N = [1e10, 1e12, 1e15, 1e18]

    cols = ['C0', 'C1', 'C2', 'C3']

    i = 0
    fig, ax = plt.subplots(1, 1)
    for n in N:
        ax.plot(T, f(T, n, dE), cols[i], label="%.0e" % n)
        ax.plot(T, f(T, n, dE, False), "--" + cols[i])
        i += 1

    ax.legend(loc=0)
    ax.set_xlabel("Temperature [K]")
    ax.set_ylabel("Ionization")
    ax.vlines(110, -1, 2, color="k", ls="--")
    ax.set_ylim(0, 1)

    fig, ax = plt.subplots(1, 1)
    for n in N:
        ax.plot(T, EFermi(T, n, dE), label="%.0e" % n)
    ax.legend(loc=0)
    ax.set_xlabel("Temperature [K]")
    ax.set_ylabel(r"$E_F-E_I$")
    ax.set_ylim(-1.1*(Egap0/2), 1.1*Egap0/2)
    ax.plot(T, Egap(T)/2, color="k",ls="--")
    ax.plot(T, -Egap(T)/2, color="k", ls="--")
    ax.set_xlim(T[0], T[-1])

    plt.show(block=True)
