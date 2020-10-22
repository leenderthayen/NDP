# -*- coding: utf-8 -*-
"""
Created on Fri Jul 14 19:41:10 2017

@author: leendert
"""

import numpy as np
import matplotlib.pyplot as plt

def LowPassFilter(x, RC, dt):
    alpha = RC/(RC+dt)

    y = np.zeros(len(x))
    y[0] = alpha*x[0]
    for i in range(1,len(x)):
        y[i] = y[i-1] + alpha*(x[i]-y[i-1])
    return y
def HighPassFilter(x, RC, dt):
    alpha = RC/(RC+dt)

    y = np.zeros(len(x))
    y[0] = x[0]
    for i in range(1, len(x)):
        y[i] = alpha*(y[i-1] + x[i] - x[i-1])
    return y

def CustomFilter(x, dt):
    y1 = HighPassFilter(x, 1e-3, dt)
    y2 = LowPassFilter(y1, 4e-6, dt)
    y3 = HighPassFilter(y2, 8e-9, dt)
    y4 = LowPassFilter(y3, 10e-9, dt)

    return y4

def Integrate(x, dt):
    y = np.zeros(len(x))
    for i in range(1,len(x)):
        y[i] = y[i-1] + x[i]*dt
    return y

if __name__ == '__main__':
    print('Starting...')

    plt.close('all')

    x = np.zeros(4000)

    offset = 10

    x[offset:offset+10]+=1.0
    x[offset:offset+2]+=0.5

    y = Integrate(x, 1)

    plt.figure()
    plt.plot(x[:50])
    plt.plot(y[:50])

    a = CustomFilter(y, 1e-9)
    plt.figure()
    plt.plot(a[:200])

    plt.show(block=True)
