import os
from sys import path
path.append(os.getcwd() + "/../datafilereadingscripts/")

import pyNab as Nab
import numpy as np
#import numpy.polynomial.polynomial.Polynomial as poly
import matplotlib.pyplot as plt

from utils.introspection import *

if __name__ == "__main__":
	print("Starting")
	fileDir = "../data/UoM/calibration/"
	filenames109 = []
	filenames109.append(fileDir + "109Cd/ScopeRun398_1-002.scope")

	filenames113 = []
	filenames113.append(fileDir + "113Sn/ScopeRun400_1.scope")

	bkgFileNames = []
	bkgFileNames.append(fileDir + "dark_chamber/ScopeRun402_1.scope")

	waveformFile109 = Nab.wf.waveformFile(filenames109)
	waveformFile113 = Nab.wf.waveformFile(filenames113)
	bkgWaveformFile = Nab.wf.waveformFile(bkgFileNames)

	# waveformFile109.defineCut('custom', np.arange(0, 5000))
	# waveformFile113.defineCut('custom', np.arange(0, 5000))
	# bkgWaveformFile.defineCut('custom', np.arange(0, 5000))

	totalTime109 = (waveformFile109.head(-1)['timestamp']-waveformFile109.head(0)['timestamp'])*4e-9 #s
	totalTime113 = (waveformFile113.head(-1)['timestamp']-waveformFile113.head(0)['timestamp'])*4e-9
	totalTimeBG = (bkgWaveformFile.head(-1)['timestamp']-bkgWaveformFile.head(0)['timestamp'])*4e-9 #s

	print(totalTime109, totalTime113, totalTimeBG)

	results109 = waveformFile109.determineEnergyTiming(method='trap', params=[1250, 40, 1250], batchsize=20000)
	results113 = waveformFile113.determineEnergyTiming(method='trap', params=[1250, 40, 1250], batchsize=20000)
	resultsBG = bkgWaveformFile.determineEnergyTiming(method='trap', params=[1250, 40, 1250], batchsize=20000)

	bins = np.arange(0, 700)
	peakE = [22.35, 60.8, 84., 24.5, 363., 389.]
	peak0window = np.arange(25, 55)
	peak1window = np.arange(60, 105)
	peak2window = np.arange(110, 150)
	peak3window = np.arange(580, 620)
	peak4window = np.arange(615, 650)
	fitresults = []
	for i in range(127):
		results109.resetCuts()
		results113.resetCuts()
		resultsBG.resetCuts()
		results109.defineCut('bc', '=', i)
		results113.defineCut('bc', '=', i)
		resultsBG.defineCut('bc', '=', i)
		#results.defineCut('energy', '>', 20)
		results109.defineCut('t0', 'between', waveformFile109.pretrigger-500, waveformFile109.pretrigger+500)
		results113.defineCut('t0', 'between', waveformFile109.pretrigger-500, waveformFile113.pretrigger+500)
		resultsBG.defineCut('t0', 'between', bkgWaveformFile.pretrigger-500, bkgWaveformFile.pretrigger+500)
		if len(results109.data()) > 10 and len(results113.data()) > 10:
			data109 = results109.data()['energy']
			data113 = results113.data()['energy']
			dataBG = resultsBG.data()['energy']
			hist109, _ = np.histogram(data109, bins=bins)
			hist113, _ = np.histogram(data113, bins=bins)
			histBG, bin_edges = np.histogram(dataBG, bins=bins)

			peak0_p0 = [np.max(peak0window), np.argmax(hist109[peak0window])+np.min(peak0window), 2, 0]
			peak1_p0 = [np.max(peak1window), np.argmax(hist109[peak1window])+np.min(peak1window), 2, 0]
			peak2_p0 = [np.max(peak2window), np.argmax(hist109[peak2window])+np.min(peak2window), 2, 0]
			peak3_p0 = [np.max(peak3window), np.argmax(hist113[peak3window])+np.min(peak3window), 2, 0]
			peak4_p0 = [np.max(peak4window), np.argmax(hist113[peak4window])+np.min(peak4window), 2, 0]

			width = bin_edges[1]-bin_edges[0]
			plt.figure()
			plt.bar(bin_edges[:-1]+width/2, hist109, width=width, label="109Cd")
			plt.bar(bin_edges[:-1]+width/2, totalTime109/totalTimeBG*histBG, width=width, alpha=0.3, label="BG")
			res0_109 = results109.fitHist('energy', bins= peak0window, rounding=1, p0=peak0_p0, color='green', linewidth=2)
			res1_109 = results109.fitHist('energy', bins = peak1window, rounding=1, p0=peak1_p0, color='orange', linewidth=2)
			res2_109 = results109.fitHist('energy', bins = peak2window, rounding=1, p0=peak2_p0, color='red', linewidth=2)
			plt.yscale("log")
			plt.legend()
			plt.figure()
			plt.bar(bin_edges[:-1]+width/2, hist113, width=width, label="113Sn")
			plt.bar(bin_edges[:-1]+width/2, totalTime113/totalTimeBG*histBG, width=width, alpha=0.3, label="BG")
			res0_113 = results113.fitHist('energy', bins= peak0window, rounding=1, p0=peak0_p0, color='green', linewidth=2)
			res3_113 = results113.fitHist('energy', bins = peak3window, rounding=1, p0=peak3_p0, color='blue', linewidth=2)
			res4_113 = results113.fitHist('energy', bins = peak4window, rounding=1, p0=peak4_p0, color='black', linewidth=2)
			plt.yscale("log")
			plt.legend()
			try:
				if res0_109[0][0] > 10 and res1_109[0][0] > 3 and res2_109[0][0] > 5 and res3_113[0][0] > 5 and res4_113[0][0] > 1:
					centroids = [res0_109[0][1], res1_109[0][1], res2_109[0][1], res0_113[0][1], res3_113[0][1], res4_113[0][1]]
					w = 1./np.array([res0_109[0][2], res1_109[0][2], res2_109[0][2], res0_113[0][2], res3_113[0][2], res4_113[0][2]])
					polyres = np.polyfit(peakE, centroids, 1, w=w)
					print(polyres)
					plt.figure()
					plt.errorbar(peakE, centroids, yerr=w**2, fmt='bo')
					x = np.linspace(0, 1.1*max(peakE))
					fit = np.poly1d(polyres)
					fitresults.append(fit)
					plt.plot(x, fit(x))
					plt.xlabel("Energy [keV]")
					plt.ylabel("Channels")
					plt.title("BC %d" % i)
			except:
				pass
			#results.savefig('bc'+str(i)+'histFitResults.png')
			#plt.show(block=True)

	plt.figure()
	x = np.linspace(0, 1.1*max(peakE))
	for i in range(len(fitresults)):
		plt.plot(x, (fitresults[i])(x), 'r', alpha=0.5)
	plt.xlabel("Energy [keV]")
	plt.ylabel("Channel")
	plt.title("Pixel calibrations")

	plt.show(block=True)
