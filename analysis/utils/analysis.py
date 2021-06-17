from lmfit import Model
import numpy as np

import utils.emcee_fitting as mcmc
import utils.fitFunctions as ff

def fitFunction(xData, yData, fitType, fitRange, isHisto=True,
                errX=None, errY=None, doSampling=False, plotResults=True, gmod=None):

    binWidth = 0.
    if isHisto:
        binWidth = xData[1]-xData[0]

    iMin = (np.abs(xData-fitRange[0])).argmin()
    iMax = (np.abs(xData-fitRange[1])).argmin()
    x = xData[iMin:iMax] + binWidth/2.
    y = yData[iMin:iMax]

    n = len(x)
    mean = sum(x*y)/(sum(y))
    sigma = np.sqrt(sum(y*(x-mean)**2)/sum(y))

    print(n,mean,sigma)

    if not gmod:
        if fitType == 'Linear':
            gmod = Model(ff.linearBG)
            gmod.set_param_hint('slope', value=0.)
            gmod.set_param_hint('offset', value=0.)

        elif fitType == 'Gaussian + BG':
            gmod = Model(ff.gauss) + Model(ff.linearBG)
            maxAmpl = max(y) - sum(y[-15:])/float(len(y[-15:]))
            gmod.set_param_hint('a', value=maxAmpl, min=maxAmpl/10, max=2.0*maxAmpl)
            gmod.set_param_hint('x0', value=mean, min=0.5*mean, max=1.5*mean)
            gmod.set_param_hint('sigma', value=0.5*sigma, min=sigma/50.0, max=2.0*sigma)
            gmod.set_param_hint('slope', value=0., max=1.0, min=-1.0)
            gmod.set_param_hint('offset', value=sum(y[-15:])/float(len(y[-15:])) + 0.3*max(x), min=0., max=sum(y[-15:])/float(len(y[-15:])) + 0.5*max(x))

        # elif fitType == 'Multiple Gaussian':
        #     number, ok = QtGui.QInputDialog.getInt(None, 'No. of peaks', 'How many peaks would you like to fit simultaneously?')
        #
        #     peakEstimates = []
        #
        #     if ok:
        #         for i in range(number):
        #             peakPos, ok = QtGui.QInputDialog.getInt(None, 'Peak position estimate', 'Provide an estimate of the peak position for the %d peak' % (i+1))
        #             if ok:
        #                 peakEstimates.append(peakPos)
        #             else:
        #                 break
        #     if len(peakEstimates) == number:
        #         models = []
        #         for i in range(number):
        #             peakPrefix = 'p%d_' % (i+1)
        #             model = Model(ff.gauss, prefix=peakPrefix)
        #             mean = peakEstimates[i]
        #             maxAmpl = 0.
        #             sigma = 60.0
        #             for j in range(len(x)):
        #                 if abs(x[j]-mean) < binWidth:
        #                     maxAmpl = y[j] - sum(y[-15:])/float(len(y[-15:]))
        #             print("Peak " + str(i+1) + " amplitude estimate: ", maxAmpl, "mean est:", mean)
        #             model.set_param_hint(peakPrefix + 'a', value=0.5*maxAmpl, min=maxAmpl*0.3)
        #             model.set_param_hint(peakPrefix + 'x0', value=mean, min=mean*0.75, max=mean*1.25)
        #             if i != 0:
        #                 model.set_param_hint(peakPrefix + 'sigma', value=sigma*0.1, expr='p1_sigma')
        #             else:
        #                 model = model + Model(ff.linearBG)
        #                 model.set_param_hint(peakPrefix + 'sigma', value=sigma*0.1, max=sigma)
        #                 model.set_param_hint('slope', value=-0.01)
        #                 model.set_param_hint('offset', value=sum(y[-15:])/float(len(y[-15:])))
        #                 print('offset', sum(y[-15:])/float(len(y[-15:])))
        #
        #             models.append(model)
        #             if gmod == None:
        #                 print(i)
        #                 print(models)
        #                 gmod = models[i]
        #             else:
        #                 gmod += models[i]

        elif fitType == 'Longoria':
            gmod = Model(ff.gauss, prefix='g_') + Model(ff.stepFunc, prefix='st_') \
            + Model(ff.lowerExponential, prefix='le_') + Model(ff.linearBG)
            maxAmpl = 0.
            for j in range(len(x)):
                if abs(x[j]-mean) < binWidth:
                    maxAmpl = y[j] - sum(y[-15:])/float(len(y[-15:]))
            gmod.set_param_hint('g_a', value=0.8*maxAmpl, min=maxAmpl*0.4, max=1.5*maxAmpl)
            gmod.set_param_hint('g_x0', value=mean, min=mean*0.75, max=mean*1.25)
            gmod.set_param_hint('g_sigma', value=sigma, max=1.5*sigma, min=0.1)

            gmod.set_param_hint('st_x0', expr='g_x0')
            gmod.set_param_hint('st_sigma', expr='g_sigma')
            gmod.set_param_hint('st_stepSize', value=10, min=1.0, max=maxAmpl/2.0)

            gmod.set_param_hint('slope', value=-0.1, max=10.0, min=-10.0)
            gmod.set_param_hint('offset', value=sum(y[-15:])/float(len(y[-15:]))+0.3*max(x), min=0., max=sum(y[-15:])/float(len(y[-15:]))+0.5**max(x))

            # gmod.set_param_hint('he_x0', expr='g_x0')
            # gmod.set_param_hint('he_sigma', expr='g_sigma')
            # gmod.set_param_hint('he_highexp1', value=maxAmpl/10.0, min=0.0, max=2.0*maxAmpl)
            # gmod.set_param_hint('he_highexp2', value=-0.6, max=-0.01, min=-1.5)

            gmod.set_param_hint('le_lowexp1', value=maxAmpl/10.0, min=0.0, max=2.0*maxAmpl)
            gmod.set_param_hint('le_lowexp2', value=0.6,  min=0.01, max=1.5)
            gmod.set_param_hint('le_x0', expr='g_x0')
            gmod.set_param_hint('le_sigma', expr='g_sigma')

        # elif fitType == 'Multiple Longoria':
        #     number, ok = QtGui.QInputDialog.getInt(None, 'No. of peaks', 'How many peaks would you like to fit simultaneously?')
        #
        #     peakEstimates = []
        #     includes = []
        #
        #     if ok:
        #         for i in range(number):
        #             peakPos, ok = QtGui.QInputDialog.getInt(None, 'Peak position estimate', 'Provide an estimate of the peak position for the %d peak' % (i+1))
        #             if ok:
        #                 peakEstimates.append(peakPos)
        #                 flags = QtGui.QMessageBox.StandardButton.Yes
        #                 flags |= QtGui.QMessageBox.StandardButton.No
        #                 inclStep = QtGui.QMessageBox.question(None, "Include Step", "Include a step function?", flags)
        #                 inclHE = QtGui.QMessageBox.question(None, "Include HE", "Include a Higher Exponential?", flags)
        #                 inclLE = QtGui.QMessageBox.question(None, "Include LE", "Include a Lower Exponential?", flags)
        #                 includes.append((inclStep, inclHE, inclLE))
        #             else:
        #                 break
        #     if len(peakEstimates) == number:
        #         models = []
        #         for i in range(number):
        #             peakPrefix = 'p%d_' % (i+1)
        #
        #             mean = peakEstimates[i]
        #             maxAmpl = max(y)
        #             for j in range(len(x)):
        #                 if abs(x[j]-mean) <= binWidth:
        #                     maxAmpl = y[j] - sum(y[-10:])/float(len(y[-10:]))
        #             print("Peak " + str(i+1) + " amplitude estimate: ", maxAmpl)
        #
        #             model = Model(ff.gauss, prefix=peakPrefix + 'g_')
        #             model.set_param_hint(peakPrefix + 'g_a', value=0.8*maxAmpl, min=maxAmpl*0.5, max=maxAmpl)
        #             model.set_param_hint(peakPrefix + 'g_x0', value=mean, min=mean*0.75, max=mean*1.25)
        #
        #             if includes[i][0] == QtGui.QMessageBox.Yes:
        #                 print("Adding step")
        #                 model+= Model(ff.stepFunc, prefix=peakPrefix + 'st_')
        #                 model.set_param_hint(peakPrefix + 'st_x0', value=mean, expr=peakPrefix + 'g_x0')
        #                 model.set_param_hint(peakPrefix + 'st_sigma', value=sigma*0.5, expr=peakPrefix + 'g_sigma')
        #                 model.set_param_hint(peakPrefix + 'st_stepSize', value=30, min=0.0, max=maxAmpl/2.0)
        #             #model.set_param_hint(peakPrefix + 'st_offset', value=maxAmpl/20.0, min=0., max=maxAmpl/5.0)
        #
        #             if includes[i][1] == QtGui.QMessageBox.Yes:
        #                 print("Adding HE")
        #                 model+= Model(ff.higherExponential, prefix=peakPrefix+'he_')
        #                 model.set_param_hint(peakPrefix + 'he_x0', value=mean, expr=peakPrefix + 'g_x0')
        #                 model.set_param_hint(peakPrefix + 'he_sigma', value=sigma*0.5, expr=peakPrefix + 'g_sigma')
        #                 model.set_param_hint(peakPrefix + 'he_highexp1', value=maxAmpl/10.0, max=2.0*maxAmpl, min=0.)
        #
        #             if includes[i][2] == QtGui.QMessageBox.Yes:
        #                 print("Adding LE")
        #                 model+=Model(ff.lowerExponential, prefix=peakPrefix + 'le_')
        #                 model.set_param_hint(peakPrefix + 'le_lowexp1', value=maxAmpl/10.0, max=2.0*maxAmpl, min=0.)
        #                 model.set_param_hint(peakPrefix + 'le_x0', value=mean, expr=peakPrefix + 'g_x0')
        #                 model.set_param_hint(peakPrefix + 'le_sigma', value=sigma*0.5, expr=peakPrefix + 'g_sigma')
        #
        #
        #             if i != 0:
        #                 model.set_param_hint(peakPrefix + 'g_sigma', value=sigma, expr='p1_g_sigma')
        #                 if (includes[i][1] == QtGui.QMessageBox.Yes) and (includes[0][1] == QtGui.QMessageBox.Yes):
        #                     model.set_param_hint(peakPrefix + 'he_highexp2', value=-0.2, expr='p1_he_highexp2')
        #                 if (includes[i][2] == QtGui.QMessageBox.Yes) and (includes[0][2] == QtGui.QMessageBox.Yes):
        #                     model.set_param_hint(peakPrefix + 'le_lowexp2', value=0.2, expr='p1_le_lowexp2')
        #             else:
        #                 model = model + Model(ff.linearBG)
        #                 model.set_param_hint(peakPrefix + 'g_sigma', value=sigma)
        #
        #                 if includes[i][1] == QtGui.QMessageBox.Yes:
        #                     model.set_param_hint(peakPrefix + 'he_highexp2', value=-0.6, max=-0.1, min=-1.5)
        #                 if includes[i][2] == QtGui.QMessageBox.Yes:
        #                     model.set_param_hint(peakPrefix + 'le_lowexp2', value=0.6, min=0.1, max=1.5)
        #                 model.set_param_hint('slope', value=-0.01)
        #                 model.set_param_hint('offset', value=sum(y[-15:])/float(len(y[-15:])), min=0.)
        #
        #             models.append(model)
        #             if gmod == None:
        #                 gmod = models[i]
        #             else:
        #                 gmod += models[i]

        elif fitType == 'Fierz':
            gmod = Model(ff.fierz)
            gmod.set_param_hint('ampl', value=1)
            gmod.set_param_hint('fierz', value=0.01)
            #TODO Implement Z Value

    if gmod != None:
        print(gmod)

        if not errY:
            errY = np.sqrt(y)
            errY[np.where(errY == 0)] = 1.0

            print('Zeros in Errs', np.where(errY == 0.0))

        result, mcmc_results, figs = mcmc.fitToData(gmod, x, y, errY, doSampling=doSampling, plotResults=plotResults)
        print(result.ier)
        print(result.lmdif_message)
        print(result.message)
        print(figs)

        #ci, trace = conf_interval(result, sigmas=[0.68,0.95], trace=True, verbose=False)
        #printfuncs.report_ci(ci)
        return x, result, mcmc_results, gmod, figs
    else:
        return None
