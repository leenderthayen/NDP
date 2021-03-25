import numpy as np
import matplotlib.pyplot as plt

a0 = 0.0529 # Bohr radius in nm

def as_TF(Z1, Z2):
    return a0*0.8853/np.sqrt(Z1**0.66+Z2**0.66)

def as_ZBL(Z1, Z2):
    return a0*0.8853/(Z1**0.23+Z2**0.23)

def fn(E0, Z1, Z2, M1, M2, aS):
    kL = 0.07954*(M1+M2)**1.5*Z1**0.66*np.sqrt(Z2)/(M1**1.5*np.sqrt(M2)*(Z1**0.66+Z2**0.66)**0.75)

    epsilon = 694.4*E0*M2*aS/(Z1*Z2*(M1+M2))

    g = 0.7422*epsilon+1.6812*epsilon**0.75+0.90656*epsilon**(1/6.)

    return 1/(1+kL*g)

def QE(z, t, p0, p1, l):
    if z < t:
        return p0
    return 1+(p1-1)*np.exp(-(z-t)/l)

if __name__ == "__main__":

    Z1 = 1
    Z2 = 14
    M1 = 1
    M2 = 28
    E0 = 1 # keV

    print(as_TF(Z1, Z1))
    print(as_ZBL(Z1, Z2))
    print(fn(E0, Z1, Z2, M1, M2, as_ZBL(Z1, Z2)))

    t = 20 #nm
    p0 = 0
    p1 = 0.09
    l = 60
