"""
.. module:: loglikelihood
    :platform: Linux
    :synopsis: Implementation of calculation of the
     loglikelihood for common distributions

.. moduleauthor:: Leendert Hayen <leendert.hayen@fys.kuleuven.be>
"""

import numpy as np

m_e = 510.998910
alpha =  7.2973525698e-3

def gauss(x, a, x0, sigma):
    tt = (x-x0)/sigma
    return a*np.exp(-0.5*(tt**2.0))
    
def fierz(E, ampl, fierz, Z=20.0):
    """Returns the function for a Fierz term present in the nuclear beta decay
    rate

    Parameters
    ----------
    E : array_like
        Input data for the experimental energy of the beta particle
    ampl : float
           Rescaling factor
    fierz : float
            Value for Fierz parameter
    Z : float
        Proton number of the decay

    Returns
    -------
    float
        loglikelihood for the data."""
    return ampl*(1+fierz*np.sqrt(1.0-(alpha*Z)**2.0)*m_e/(m_e+E))
    
def linearBG(x, slope, offset):
    return x*slope+offset
    
def stepFunc(x, x0, sigma, stepSize):
    tt = (x-x0)/sigma
    return stepSize/(1.0+np.exp(tt))**2.0

def lowerExponential(x, x0, sigma, lowexp1, lowexp2):
    tt = (x-x0)/sigma
    return lowexp1*np.exp(lowexp2*tt)/(1+np.exp(tt))**4.0
    
def higherExponential(x, x0, sigma, highexp1, highexp2):
    tt = (x-x0)/sigma
    return highexp1*np.exp(highexp2*tt)/(1+np.exp(-tt))**4.0
def fullFunction(theta, x):
    a, sigma, x0, le1, le2, he1, he2, step, offset = theta
    return gauss(x, a, x0, sigma) + lowerExponential(x, x0, sigma, le1, le2) \
    + higherExponential(x, x0, sigma, he1, he2) + stepFunc(x, x0, sigma, step)\
    + linearBG(x, 0.0, offset)
    
def round2SignifFigs(vals, n):
    """
    Code copied from
    http://stackoverflow.com/questions/18915378/rounding-to-significant-figures-in-numpy
    Goes over the list or array of vals given, and rounds
    them to the number of significant digits (n) given.

    Parameters
    ----------
    vals : array_like
        Values to be rounded.
    n : integer
        Number of significant digits to round to.

    Note
    ----
    Does not accept: inf, nan, complex

    Example
    -------
    >>> m = [0.0, -1.2366e22, 1.2544444e-15, 0.001222]
    >>> round2SignifFigs(m,2)
    array([  0.00e+00,  -1.24e+22,   1.25e-15,   1.22e-03])
    """
    if np.all(np.isfinite(vals)) and np.all(np.isreal((vals))):
        eset = np.seterr(all='ignore')
        mags = 10.0 ** np.floor(np.log10(np.abs(vals)))  # omag's
        vals = np.around(vals / mags, n - 1) * mags  # round(val/omag)*omag
        np.seterr(**eset)
        try:
            vals[np.where(np.isnan(vals))] = 0.0  # 0.0 -> nan -> 0.0
        except TypeError as e:
            print(e.message)
            if np.isnan(vals):
                vals = 0.0
    else:
        raise IOError('Input must be real and finite')
    return vals


def weightedAverage(x, sigma, axis=None):
    r"""Takes the weighted average of an array of values and the associated
    errors. Calculates the scatter and statistical error, and returns
    the greater of these two values.
    Parameters
    ----------
    x: array_like
        Array-like assortment of measured values, is transformed into a
        1D-array.
    sigma: array_like
        Array-like assortment of errors on the measured values, is transformed
        into a 1D-array.
    Returns
    -------
    tuple
        Returns a tuple (weighted_average, uncertainty), with the uncertainty
        being the greater of the uncertainty calculated from the statistical
        uncertainty and the scattering uncertainty.
    Note
    ----
    The formulas used are
    .. math::
        \left\langle x\right\rangle_{weighted} &= \frac{\sum_{i=1}^N \frac{x_i}
                                                                 {\sigma_i^2}}
                                                      {\sum_{i=1}^N \frac{1}
                                                                {\sigma_i^2}}
        \sigma_{stat}^2 &= \frac{1}{\sum_{i=1}^N \frac{1}{\sigma_i^2}}
        \sigma_{scatter}^2 &= \frac{\sum_{i=1}^N \left(\frac{x_i-\left\langle
                                                    x\right\rangle_{weighted}}
                                                      {\sigma_i}\right)^2}
               {\left(N-1\right)\sum_{i=1}^N \frac{1}{\sigma_i^2}}"""
    x = np.ravel(x)
    sigma = np.ravel(sigma)
    Xstat = (1 / sigma**2).sum(axis=axis)
    Xm = (x / sigma**2).sum(axis=axis) / Xstat
    # Xscatt = (((x - Xm) / sigma)**2).sum() / ((1 - 1.0 / len(x)) * Xstat)
    Xscatt = (((x - Xm) / sigma)**2).sum(axis=axis) / ((len(x) - 1) * Xstat)
    Xstat = 1 / Xstat
    return Xm, np.maximum.reduce([Xstat, Xscatt], axis=axis) ** 0.5