# -*- coding: utf-8 -*-
"""
Created on Fri May 22 16:25:10 2015

@author: leendert
"""

import ROOT
import numpy as np
from array import array

def getHistogram(rootFile, tkey=None, branch=None, bins=None, drawResults=False):
    _file = ROOT.TFile(rootFile, 'READ')
    
    print(tkey, branch)
    
    if not tkey:
        _file.ls()
    else:
        t = _file.Get(tkey)
        t.Print()
        if t.ClassName() == 'TTree':
            if branch:
                nentries = t.GetEntries()
                
                print('Nr Entries:', nentries)
                    
                dummy = array('d',[0])
                t.SetBranchAddress( str(branch), dummy )
                
                b = t.GetBranch(branch)
    
                alist, i = np.zeros(nentries), 0
                while b.GetEntry(i):
                    alist[i] = dummy[0]
                    i += 1
                    
                bins = max(200, int((max(alist)-min(alist))/2.0)) if not bins else bins
                    
                yHist, xHist = np.histogram(alist, bins=bins)
                
                if drawResults:
                    try:
                        import matplotlib.pyplot as plt
                        
                        plt.close('all')
                        
                        plt.ion()
                        
                        plt.hist(alist, bins=100)
                        plt.show()
                    except:
                        print('Install Matplotlib for a histogram of the data')
                return (xHist, yHist)
            else:
                return (None, None)
        elif t.ClassName() == 'TH1D':
            entries = t.FindLastBinAbove(0, 1)-t.FindFirstBinAbove(0, 1)
            print(entries)
            xHist = np.zeros(entries)
            yHist = np.zeros(entries)
            for i in range(entries):
                xHist[i] = t.GetBinCenter(i)
                yHist[i] = t.GetBinContent(i)
            return (xHist, yHist)
        else:
            return (None, None)
        
if __name__ == '__main__':
    xHist, yHist = getHistogram('Results_5_1600keV.root', tkey='Results', branch=u'currE', drawResults=True, bins=200)
    
    print(xHist)
    print(yHist)