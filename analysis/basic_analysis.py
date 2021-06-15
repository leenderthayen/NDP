import numpy as np
import matplotlib.pyplot as plt
import pandas
import uproot
import awkward
from tqdm import tqdm

def makeHist(tree, var, cut=None, step_size="50 MB", bins=1000, plot=True):
    hist = np.zeros(bins)
    for batch in tqdm(tree.iterate(var, cut, step_size=step_size, library="np")):
        h, bin_edges = np.histogram(batch[var], bins=bins)
        hist += h
    if plot:
        binWidth = bin_edges[1]-bin_edges[0]
        plt.figure()
        plt.bar(bin_edges[:-1]+binWidth/2, hist, width=binWidth)
        plt.yscale("log")
    return bin_edges, hist

def plotPrimaries(tree):
    radAll = tree.arrays(["energy"], "process == \"RadioactiveDecayBase\"")["energy"]
    rade = tree.arrays(["energy"], "((process == \"RadioactiveDecayBase\") & (name == \"e-\"))")["energy"]
    radg = tree.arrays(["energy"], "((process == \"RadioactiveDecayBase\") & (name == \"gamma\"))")["energy"]

    fig, ax = plt.subplots(1, 1)
    ax.hist(radAll, bins=int(max(radAll)), label="All", alpha=0.5)
    ax.hist(rade, bins=int(max(rade)), label="R e-", alpha=0.5)
    ax.hist(radg, bins=int(max(radg)), label="R gamma", alpha=0.5)
    ax.set_xlabel("Energy [keV]")
    ax.set_ylabel("Counts [a.u.]")
    fig.suptitle("Radioactive decay products")
    ax.legend(loc=0)
    ax.set_yscale("log")

    # fig, ax = plt.subplots(1, 1)
    # for n in set(tree["process"].array()):
    #     d = tree.arrays(["energy"], "process == \"%s\"" % n)["energy"]
    #     f, a = plt.subplots(1, 1)
    #     a.hist(d, bins=max(int(max(d)), 1), label=n, lw=3)
    #     a.set_yscale("log")
    #     f.suptitle(n)
    #     ax.hist(d, bins=max(int(max(d)), 1), label=n, lw=3, alpha=0.3)
    # ax.set_xlabel("Energy [keV]")
    # ax.set_ylabel("Counts [a.u.]")
    # fig.suptitle("Generated primaries")
    # ax.legend(loc=0)
    # ax.set_yscale("log")

def buildPixelSpectrum(hitsTree, pixel, cce = lambda z: 1, bins=100, plot=True):
    if pixel == -1:
        df = hitsTree.arrays(["eventID" ,"eDep", "z"], "z < 2", library="pd")
    else:
        df = hitsTree.arrays(["eventID" ,"eDep", "z"], "((pixelNumber == %d) & (z < 2))" % pixel, library="pd")

    df["eDepCorr"] = df["eDep"]*cce(df["z"])


    gdf = df.groupby("eventID")

    eDep = gdf.sum()["eDepCorr"]

    eDepHist, bin_edges = np.histogram(eDep, bins=bins)

    if plot:
        plt.figure()
        binWidth = bin_edges[1]-bin_edges[0]
        plt.bar(bin_edges[:-1]+binWidth/2, eDepHist, width=binWidth)

    return eDepHist, bins


if __name__ == "__main__":
    print("Starting")

    fileName = "109Cd_front_UoM_0.5+1.67.root"

    file = uproot.open(fileName)

    makeHist(file["ntuple/energy"], "enSi")

    dl = 1e-4 # 100 nm, in mm

    cce = lambda z: np.where(z < dl, 0, 1)

    buildPixelSpectrum(file["ntuple/hits"], 64, cce=cce)

    plt.show(block=True)

    print("Stop")
