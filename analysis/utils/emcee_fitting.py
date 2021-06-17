# -*- coding: utf-8 -*-
"""
Created on Wed May 20 09:41:41 2015

@author: leendert
"""

import lmfit
import numpy as np
import emcee
from matplotlib.ticker import MaxNLocator

from datetime import datetime

import utils.loglikelihood as llh
import utils.utilities as utils

import matplotlib.pyplot as plt

figFolder = 'Figures/'

def create_prior(params):
    """
    emccee uses a uniform prior for every variable.
    Here we create a functions which checks the bounds
    and returns np.inf if a value is outside of its
    allowed range. WARNING: A uniform prior may not be
    what you want!
    """
    none_to_inf = lambda x, sign=1: sign*np.inf if x is None else x
    lower_bounds = np.array([none_to_inf(i.min, -1) for i in params.values() if i.vary])
    upper_bounds = np.array([none_to_inf(i.max, 1) for i in params.values() if i.vary])
    def bounds_prior(values):
        values = np.asarray(values)
        is_ok = np.all((lower_bounds < values) & (values < upper_bounds))
        return 0 if is_ok else -np.inf
    return bounds_prior

def create_lnliklihood(params, model, x, y, yerr):
    """create a normal-likihood from the residuals"""
    def lnprob(vals):
        for v, p in zip(vals, [p for p in params.values() if p.vary]):
            p.value = v
        mod = model.eval(params=params, x=x)
        l = llh.Gaussian(y, mod, yerr)
        if np.isnan(l):
            l = -np.inf
        return l
    return lnprob

def starting_guess(params):
    vals = [i.value for i in params.values() if i.vary]
    return vals

def create_all(model, params, x, y, yerr=None):
    """
    creates the log-poposterior function from a minimizer.
    sigma should is either None or an array with the
    1-sigma uncertainties of each residual point. If None,
    sigma will be assumed the same for all residuals and
    is added to the sampled parameters.
    """
    lnprior = create_prior(params)
    lnprob = create_lnliklihood(params, model, x, y, yerr)
    guess = starting_guess(params)
    func = lambda x: lnprior(x) + lnprob(x)
    return func, guess

def residuals(params, model, x, y, yerr):
    return (model.eval(params=params, x=x)-y)/yerr


def fitToData(model, x, y, yerr, doSampling=True, plotResults=True, **kwargs):

    plt.close('all')
    plt.ion()

    params = model.make_params()

    print('Starting minimisation...')

    mini = lmfit.Minimizer(residuals, params, fcn_args=(model, x, y, yerr))
    try:
        mini.minimize(method='differential_evolution', **kwargs)
        result = model.fit(y, params, x=x, weights=1./yerr, method='leastsq')
        if np.isnan(result.chisqr):
            print('**** WARNING ****')
            print('Differential evolution failed, likelihood returned NaN')
            print('Trying again with LM')
            params = model.make_params()
            result = model.fit(y, params, x=x, weights=1./yerr, method='leastsq')
        params = result.params
    except (TypeError, ValueError) as e:
        print('**** WARNING ****')
        print(e.message)
        print('Failed to fit using Differential Evolution, using Levenberg-Marquadt instead')
        mini.minimize(**kwargs)
        result = model.fit(y, params, x=x, weights=1./yerr)

    lmfit.report_errors(params)
    print(result.fit_report())

    figs = []

    if plotResults:
        fitLM = plotFitResults(model, params, result, x, y)
        figs.append(fitLM)

    if doSampling:
        print('Starting Sampling...')

        lnfunc, guess = create_all(model, params, x, y, yerr)

        nwalkers, ndim = 50, len(guess)
        p0 = emcee.utils.sample_ball(guess, 2e-4*np.array(guess), nwalkers)
        sampler = emcee.EnsembleSampler(nwalkers, ndim, lnfunc)
        steps = 1000
        sampler.run_mcmc(p0, steps)

        fitMCMC = interpretSamplingResults(x, y, yerr, sampler, ndim, params, model)
        figs.append(fitMCMC)

        if plotResults:
            figEvol, trFig = plotSamplingResults(sampler, ndim, params)
            figs.append(figEvol)
            figs.append(trFig)

        print('Are residuals equal')
        print(np.array_equal(np.asarray(result.residual), np.array(residuals(params, model, x, y, yerr))))

        plt.close('all')

        return result, (sampler, ndim, params), figs
    else:
        #plt.close('all')
        return result, None, figs

def interpretSamplingResults(x, y, yerr, sampler, ndim, params, model):
    burnin = 150

    samples = sampler.chain[:, burnin:, :].reshape((-1, ndim))

    val = []
    err = []
    q = [16.0, 50.0, 84.0]
    for i, samp in enumerate(samples.T):
        q16, q50, q84 = np.percentile(samp, q)
        val.append(q50)
        err.append(max([q50 - q16, q84 - q50]))
        print(q50, '+-', err[-1], '(', utils.round2SignifFigs(err[-1]/q50*100.0, 4), '%)')

    print("Mean acceptance fraction:", np.mean(sampler.acceptance_fraction))
    print("Autocorrelation time:", sampler.get_autocorr_time())

    chisq = sum(residuals(params, model, x, y, yerr)**2.0)
    dof = float(len(x)-ndim)

    print('Reduced chi-squared:', chisq, '/', dof, '=', str(chisq/dof))

    for v, p in zip(val, [p for p in params.values() if p.vary]):
        p.value = v


    fig, axes = plt.subplots(2, 1, sharex=True, figsize=(16,9), squeeze=True)
    axes[0].plot(x, y, label=r'$\chi^2/\nu$ = %.2f/%d' % (chisq, dof))
    axes[0].plot(x, model.eval(params=params, x=x), linewidth=2)
    axes[0].legend(fontsize=17)
    axes[1].plot(x, residuals(params, model, x, y, yerr))

    comps = model.eval_components(params=params, x=x)
    for key in comps:
        axes[0].plot(x, comps[key])

    axes[0].set_ylabel('Counts')
    axes[1].set_ylabel('Residuals')
    axes[1].set_xlabel('Channel')

    fig.suptitle('MCMC Results', y=0.96, fontsize=20)

    time = datetime.strftime(datetime.now(), '%d-%m-%Y_%H_%M')

    plt.savefig(figFolder + 'mcmc_fit_results_' + time + '.pdf', bbox_inches='tight')

    plt.close()

    return fig


def plotFitResults(model, params, result, x, y):

    fig, axes = plt.subplots(2, 1, sharex=True, figsize=(16,9))
    axes[0].plot(x, y)
    axes[0].plot(x, result.best_fit, linewidth=2)
    axes[1].plot(x, result.residual)

    axes[0].set_ylabel('Counts')
    axes[1].set_ylabel('Residuals')
    axes[1].set_xlabel('Channel')

    fig.suptitle('LMFit Results', y=0.96, fontsize=20)


    comps = result.eval_components(x=x)
    for key in comps:
        axes[0].plot(x, comps[key])

    #plt.close()

    return fig


def plotSamplingResults(sampler, ndim, params, truths=None):

    labels = [l for l in params.keys() if params[l].vary]

    print(labels)

    burnin = 200

    samples = sampler.chain[:, burnin:, :].reshape((-1, ndim))

    '''fig, axes = plt.subplots(int((ndim+1)/2.0), 2, sharex=True, figsize=(8, 9))
    for i in range(ndim):
        axes[i%int((ndim+1.0)/2.0)][int(i/(ndim+1.0)*2.0)].plot(sampler.chain[:, :, i].T, color='k', alpha=0.4)
        axes[i%int((ndim+1.0)/2.0)][int(i/(ndim+1.0)*2.0)].yaxis.set_major_locator(MaxNLocator(5))
        axes[i%int((ndim+1.0)/2.0)][int(i/(ndim+1.0)*2.0)].set_ylabel(labels[i])
    fig.tight_layout()'''

    fig, axes = plt.subplots(ndim, 1, sharex=True, figsize=(8, 9))
    for i in range(ndim):
        axes[i].plot(sampler.chain[:, :, i].T, color='k', alpha=0.4)
        axes[i].yaxis.set_major_locator(MaxNLocator(5))
        axes[i].set_ylabel(labels[i])
    fig.tight_layout(h_pad=0.0)

    time = datetime.strftime(datetime.now(), '%d-%m-%Y_%H_%M')
    plt.savefig(figFolder + 'mcmc_evol_' + time + '.pdf', bbox_inches='tight')

    val = []
    err = []
    q = [16.0, 50.0, 84.0]
    for i, samp in enumerate(samples.T):
        q16, q50, q84 = np.percentile(samp, q)
        val.append(q50)
        err.append(max([q50 - q16, q84 - q50]))

    trFig = None
    try:
        import triangle # use pip install triangle_plot
        samples = sampler.chain[:, burnin:, :].reshape((-1, ndim))
        labels = [l for l in params.keys() if params[l].vary]+  ['sigma']
        trFig = triangle.corner(samples, labels= labels,
                        truths=truths, show_data=False, show_titles=True, quantiles=[0.16, 0.5, 0.84])

        time = datetime.strftime(datetime.now(), '%d-%m-%Y_%H_%M')

        plt.savefig(figFolder + 'mcmc_triangle_' + time + '.pdf', bbox_inches='tight')
    except ImportError:
        print("Please install triangle_plot for a nice overview graphic")

    val = []
    err = []
    q = [16.0, 50.0, 84.0]
    for i, samp in enumerate(samples.T):
        q16, q50, q84 = np.percentile(samp, q)
        val.append(q50)
        err.append(max([q50 - q16, q84 - q50]))



    '''if __name__ == '__main__':
        plt.figure()

        plt.plot(range(ndim), np.array(truths)/truths, 'bD', label='True Values')
        plt.errorbar(range(ndim), np.array(val)/truths, yerr=np.array(err)/truths, fmt='rD', label='MCMC')
        #plt.plot(np.array(range(ndim))+0.05, valsLM/truths, 'gD', label='LMfit')
        plt.xlabel('Parameter number', fontsize=20)
        plt.ylabel('Ratio', fontsize=20)
        plt.title('Fit quality on test distributions', fontsize=20, y=1.05)
        plt.legend(bbox_to_anchor=(0.43, 0.9), fontsize=20)
        plt.tick_params(axis='both', labelsize=20)
        plt.tight_layout()'''

    return (fig, trFig)


if __name__ == '__main__':
    plt.close('all')

    x = np.linspace(0, 50, 500)
    a, x0, sigma = 200, 20, 3 # Real Values
    a2, x02, sigma2 = 300, 28, 3
    truths = [a, sigma, x0, sigma2, x02, a2]
    y_true = utils.gauss(x, a, x0, sigma) + utils.gauss(x, a2, x02, sigma2)
    yerr = np.random.randn(x.size)*np.sqrt(y_true)
    y = y_true + yerr

    yerr = np.sqrt(y_true)


    gmod = lmfit.Model(utils.gauss, prefix='p1_') + lmfit.Model(utils.gauss, prefix='p2_')
    gmod.set_param_hint('p1_a', value=150, min=20, max=500)
    gmod.set_param_hint('p1_x0', value=7, min=5, max=30)
    gmod.set_param_hint('p1_sigma', value=4, min=0, max=10)
    gmod.set_param_hint('p2_a', value=300, min=20, max=500)
    gmod.set_param_hint('p2_x0', value=28, min=10, max=45)
    gmod.set_param_hint('p2_sigma', value=3, min=0, max=5)

    _, _, figs = fitToData(gmod, x, y, yerr)

    for f in figs:
        f.show()
