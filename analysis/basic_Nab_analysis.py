import os
from sys import path
path.append(os.getcwd() + "/../datafilereadingscripts/")

import pyNab as Nab
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

import logging

from utils.introspection import *
from utils.utilities import *
import utils.analysis as anal

logging.basicConfig(level=logging.INFO)

def analyseWFFiles(filenames, cuts):
	logging.info("Entering analyseWFFiles for file(s):"  + str(filenames))
	waveformFile = Nab.wf.waveformFile(filenames)

	for c in cuts:
		waveformFile.defineCut(*c)

	print(waveformFile.bcs)

	totalTime = (waveformFile.head(-1)['timestamp']-waveformFile.head(0)['timestamp'])*4e-9

	results = waveformFile.determineEnergyTiming(method='trap', params=[1250, 40, 1250], batchsize=20000)

	logging.info("Energy timing done")

	return waveformFile, totalTime, results

def loadTemperatureData(filename):
	names = ["Detector", "FET S", "FET N", "Ring", "Preamp", "Abs time", "Time stamp 12h", "Date"]

	temp = pd.read_csv(filename, parse_dates=[6, 7], infer_datetime_format=True, header=None, names=names)

	return temp

def loadBiasCurrentData(filename):
	names = ["Bias status", "Nominal bias", "Detector current", "Abs time", "Time stamp 12h", "Date"]

	if filename.split(".")[-1] == "xlsx":
		biasCurrent = pd.read_excel(filename, parse_dates=[4, 5], header=None, names=names)
	else:
		biasCurrent = pd.read_csv(filename, parse_dates=[4, 5], infer_datetime_format=True, header=None, names=names)

	return biasCurrent

def loadFilterFile(filename):
	filter = pd.read_csv(filename, header=1)

	return filter

def fitPeaks(results, bc, bins):
	logging.info("Fitting peaks for results " + str(results) + " for channel %d" % bc)
	logging.debug("Resetting cuts")
	results.resetCuts()
	if bc > 0:
		results.defineCut('bc', '=', bc)

	resultColl = []
	figColl = []

	#results.defineCut('t0', 'between', )
	data = results.data()
	if len(data) > 10:
		E = data["energy"]

		histE, _ = np.histogram(E, bins=bins)

		h = 0.3*max(histE)
		w = 10
		indices = findPeaks(histE, height=h, width=w)

		for i in range(len(indices)):
			fitRange = [bins[indices[i]-10*w], bins[indices[i]+10*w]]

			x, result, _, gmod, figs = anal.fitFunction(bins, histE, 'Longoria', fitRange)

			resultColl.append(result)
			figColl.append(figs)

			plt.show(block=True)

	return resultColl, figColl

if __name__ == "__main__":
	logging.info("STARTING")

	figDir = "figures/"

	fileDir = "/Users/leenderthayen/git/NDP/data/UoM/bias_scan_single/"

	filenames = ["Run-420_0  Cd109_Vb=-50V/ScopeRun420_0-004.scope", "Run-416_1 Cd109_Vb=-75V/ScopeRun416_1-002.scope", "Run-417_0 Cd109_Vb=-90V/ScopeRun417_0-003.scope", "Run-418_0  Cd109_Vb=-120V/ScopeRun418_0-006.scope", "Run-419_0  Cd109_Vb=-150V/ScopeRun419_0-005.scope"]

	plt.figure()
	bins = np.arange(0, 200, 0.2)
	histDict = dict()
	bcs = np.arange(17, 23)
	for f in filenames:
		cuts = []
		#cuts.append(['custom', np.arange(0, 7000)])
		cuts.append(['bc', 'or', bcs])
		wf, time, results = analyseWFFiles([fileDir + f], cuts)
		hists = np.zeros((len(bcs), len(bins)-1))
		i = 0
		for bc in bcs:
			results.resetCuts()
			results.defineCut('bc', '=', bc)
			results.defineCut('t0', 'between', 3000, 4000)
			hist, _ = np.histogram(results.data()['energy'], bins=bins)

			hists[i, :] = hist
			i += 1
		histDict[f] = hists

	i = 0
	for bc in bcs:
		plt.figure(figsize=(16, 10))
		plt.title("BC %d" % bc)
		plt.xlabel("Channels")
		plt.ylabel("Counts")
		for f in filenames:
			hist = histDict[f][i, :]
			plt.plot(bins[:-1]+0.1, hist, label=f)
		plt.yscale("log")
		plt.legend(loc=0)
		plt.savefig(figDir + "bias_scan_bc_%d.png" % bc)
		plt.close('all')
		i += 1
	#plt.yscale("log")
		# bins = np.arange(0, 400, 0.2)
		#
		# results, figs = fitPeaks(results, 19, bins)
		#
		# print(results)

	# bins = np.arange(0, 700)
	# peakE = [22.35, 60.8, 84., 24.5, 363., 389.]
	# peak0window = np.arange(25, 55)
	# peak1window = np.arange(60, 105)
	# peak2window = np.arange(110, 150)
	# peak3window = np.arange(580, 620)
	# peak4window = np.arange(615, 650)
	# fitresults = []
	# for i in range(127):
	# 	results109.resetCuts()
	# 	results109.defineCut('bc', '=', i)
	# 	# results113.defineCut('bc', '=', i)
	# 	# resultsBG.defineCut('bc', '=', i)
	# 	#results.defineCut('energy', '>', 20)
	# 	results109.defineCut('t0', 'between', waveformFile109.pretrigger-500, waveformFile109.pretrigger+500)
	# 	# results113.defineCut('t0', 'between', waveformFile109.pretrigger-500, waveformFile113.pretrigger+500)
	# 	# resultsBG.defineCut('t0', 'between', bkgWaveformFile.pretrigger-500, bkgWaveformFile.pretrigger+500)
	# 	if len(results109.data()) > 10:
	# 		data109 = results109.data()['energy']
	# 		# data113 = results113.data()['energy']
	# 		# dataBG = resultsBG.data()['energy']
	# 		hist109, bin_edges = np.histogram(data109, bins=bins)
	# 		# hist113, _ = np.histogram(data113, bins=bins)
	# 		# histBG, bin_edges = np.histogram(dataBG, bins=bins)
	#
	# 		peak0_p0 = [np.max(peak0window), np.argmax(hist109[peak0window])+np.min(peak0window), 2, 0]
	# 		peak1_p0 = [np.max(peak1window), np.argmax(hist109[peak1window])+np.min(peak1window), 2, 0]
	# 		peak2_p0 = [np.max(peak2window), np.argmax(hist109[peak2window])+np.min(peak2window), 2, 0]
	# 		# peak3_p0 = [np.max(peak3window), np.argmax(hist113[peak3window])+np.min(peak3window), 2, 0]
	# 		# peak4_p0 = [np.max(peak4window), np.argmax(hist113[peak4window])+np.min(peak4window), 2, 0]
	#
	# 		width = bin_edges[1]-bin_edges[0]
	# 		fig109 = plt.figure()
	# 		plt.title("BC %d" % i)
	# 		plt.bar(bin_edges[:-1]+width/2, hist109, width=width, label="109Cd")
	# 		# plt.bar(bin_edges[:-1]+width/2, totalTime109/totalTimeBG*histBG, width=width, alpha=0.3, label="BG")
	# 		res0_109 = results109.fitHist('energy', bins= peak0window, rounding=1, p0=peak0_p0, color='green', linewidth=2)
	# 		res1_109 = results109.fitHist('energy', bins = peak1window, rounding=1, p0=peak1_p0, color='orange', linewidth=2)
	# 		res2_109 = results109.fitHist('energy', bins = peak2window, rounding=1, p0=peak2_p0, color='red', linewidth=2)
	# 		plt.yscale("log")
	# 		plt.legend()
	# 		# fig113 = plt.figure()
	# 		# plt.title("BC %d" % i)
	# 		# plt.bar(bin_edges[:-1]+width/2, hist113, width=width, label="113Sn")
	# 		# plt.bar(bin_edges[:-1]+width/2, totalTime113/totalTimeBG*histBG, width=width, alpha=0.3, label="BG")
	# 		# res0_113 = results113.fitHist('energy', bins= peak0window, rounding=1, p0=peak0_p0, color='green', linewidth=2)
	# 		# res3_113 = results113.fitHist('energy', bins = peak3window, rounding=1, p0=peak3_p0, color='blue', linewidth=2)
	# 		# res4_113 = results113.fitHist('energy', bins = peak4window, rounding=1, p0=peak4_p0, color='black', linewidth=2)
	# 		# plt.yscale("log")
	# 		# plt.legend()
	# 		# try:
	# 		# 	if res0_109[0][0] > 10 and res1_109[0][0] > 3 and res2_109[0][0] > 5 and res3_113[0][0] > 5 and res4_113[0][0] > 1:
	# 		# 		fig109.savefig(figDir + 'bc_%d_109Cd.png' % i)
	# 		# 		fig113.savefig(figDir + 'bc_%d_113Sn.png' % i)
	# 		# 		centroids = [res0_109[0][1], res1_109[0][1], res2_109[0][1], res0_113[0][1], res3_113[0][1], res4_113[0][1]]
	# 		# 		w = 1./np.array([res0_109[0][2], res1_109[0][2], res2_109[0][2], res0_113[0][2], res3_113[0][2], res4_113[0][2]])
	# 		# 		polyres = np.polyfit(peakE, centroids, 1, w=w)
	# 		# 		print(polyres)
	# 		# 		plt.figure()
	# 		# 		plt.errorbar(peakE, centroids, yerr=w**2, fmt='bo')
	# 		# 		x = np.linspace(0, 1.1*max(peakE))
	# 		# 		fit = np.poly1d(polyres)
	# 		# 		fitresults.append(fit)
	# 		# 		plt.plot(x, fit(x), label="%.2f E %+.2f" % (polyres[0], polyres[1]))
	# 		# 		plt.xlabel("Energy [keV]")
	# 		# 		plt.ylabel("Channels")
	# 		# 		plt.legend(loc=0)
	# 		# 		plt.title("BC %d" % i)
	# 		# 		plt.savefig(figDir + 'bc_%d_calibration.png' % i)
	# 		# 		print("GOOD!! BC %d" % i)
	# 		# except:
	# 		# 	pass
	# 		#
	# 		plt.show(block=True)
	#
	# plt.figure()
	# x = np.linspace(0, 1.1*max(peakE))
	# for i in range(len(fitresults)):
	# 	plt.plot(x, (fitresults[i])(x), 'r', alpha=0.5)
	# plt.xlabel("Energy [keV]")
	# plt.ylabel("Channel")
	# plt.title("Pixel calibrations")
	# plt.savefig(figDir + "all_calibrations.png")
	# print("%d good channels!" % len(fitresults))

	plt.show(block=True)
