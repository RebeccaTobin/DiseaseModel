# -*- coding: utf-8 -*-
"""
Created on Wed May 5 18:12:47 2021

@author: Rebecca Tobin
"""

#%%
import DataAnalysis_classFile as D

# location of disease data files
path = ""

# disease parameters

Number = 1000
# Gammap, Gamman, N, Number, R, nd, Alpha, AveDeg, pA, TopologyID, Mortality
GNMpara = [7.5,6.5,10000,Number,3.0,2,2.27,4,0.0,1,"AND"]

# Control, IndivSeedFile, t_on, tau, k_min, k_max, knn_min, knn_max, m, r
Dvals = []

IndivSeedFile = 0
r = 1.0
k = [0,10000]
knn = [0,10000]

for t_on in [0.0915]:
    for tau in [0.00183]:
        for m in [0.02]:
            Dvals.append((IndivSeedFile, t_on, tau, k[0], k[1], knn[0], knn[1], m, r))

# create disease objects

diseases = []

for Dval in Dvals:  
    diseases.append(D.Disease(GNMpara, Dval))

#%% get ave lifespan data from file

for d in diseases:
    d.assignPath(path)
    #aveYrLost = d.getAveYrLost()
    aveLifespan = d.getAveLifespan()
    #IFR = d.getIFR()

    


