"""
.. module:: loglikelihood
    :platform: Linux
    :synopsis: Implementation of calculation of the
     loglikelihood for common distributions

.. moduleauthor:: Leendert Hayen <leendert.hayen@fys.kuleuven.be>
"""

import numpy as np

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
