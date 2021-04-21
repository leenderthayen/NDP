"""
.. module:: loglikelihood
    :platform: Linux
    :synopsis: Implementation of calculation of the
     loglikelihood for common distributions

.. moduleauthor:: Leendert Hayen <leendert.hayen@fys.kuleuven.be>
"""

import numpy as np


def Poisson(x, l):
    """Returns the loglikelihood for a Poisson distribution.
    In this calculation, it is assumed that the parameters
    are true, and the loglikelihood that the data is drawn from
    the distribution established by the parameters is calculated

    Parameters
    ----------
    x : array_likes
        Data that has to be tested.
    l : array_like
        Parameter for the Poisson distribution.

    Returns
    -------
    float
        loglikelihood for the data."""
    return np.sum(x * np.log(l) - l)


def Gaussian(y, model, yerr):
    """Returns the loglikelihood for a Gaussian distribution.
    In this calculation, it is assumed that the parameters
    are true, and the loglikelihood that the data is drawn from
    the distribution established by the parameters is calculated

    Parameters
    ----------
    model : array_like
            theoretical model data to be compared against
    y : array_like
        data points
    yerr : standard deviations on individual data points,
           assumed to be gaussian

    Returns
    -------
    float
        loglikelihood for the data."""
    inv_sigma2 = 1.0/(yerr**2.0)
    return -0.5*(np.sum((y-model)**2*inv_sigma2 - np.log(inv_sigma2)))
