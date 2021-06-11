import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

if __name__ == "__main__":
    fileList = ["2905-8", "3212-9", "3214-3", "3214-5", "3214-6"]
    radius = 0.0575 #cm

    eps = 11.68*8.85e-12 # F / cm
    q = 1.6e-19
    A = np.pi*radius**2

    fig, ax = plt.subplots(1, 1)
    for f in fileList:
        d = pd.read_csv(f + ".txt", delimiter=";", decimal=",", header=None).values
        d[:, 1] *= 1e-12 # To F
        ax.plot(eps/d[:, 1]*A*1e3, abs(-2/q/eps/np.gradient(A**2/d[:, 1]**2, d[:, 0]))*1e-6, label=f)

    ax.set_yscale("log")
    #ax.set_xscale("log")
    ax.set_xlabel("Depth [mm]")
    ax.set_ylabel("N [cm^-3]")
    ax.legend(loc=0)

    plt.show(block=True)
