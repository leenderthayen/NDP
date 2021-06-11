import numpy as np

def radiative(n0, p0, dnp):
    B = 1e-14 # cm^-3 s^-1
    return 1./B/(n0+p0+dnp)

def auger(dnp):
    # Assume High level injection, only valid for doping > 8e18 cm^-3
    Cn = 2.8e-31 # cm^6 s^-1
    Cp = 9.9e-32 # cm^6 s^-1
    Ca = Cn+Cp
    return 1/Ca/dnp**2

def coulombCorrectedAuger():
    return -1

def SRH(tp0, tn0, p0, p1, n0, n1, dp, dn):
    return (tn0*(p0+p1+dp)+tp0*(n0+n1+dn))/(p0+n0+(dp+dn)/2)

def vth(T):
    vth0 = 1e7 #cm/s
    return vth0*np.sqrt(T/300.)

if __name__ == "__main__":
    import matplotlib.pyplot as plt
