# -*- coding: utf-8 -*-
"""
Created on Wed May 5 18:12:47 2021

@author: Rebecca Tobin
"""

#%%

import numpy as np
from collections import namedtuple

#%%
   
     
class Disease:
    """
    Methods:  
        - assignAveYrLost
        - getAveYrLost
        
        - assignAveLifespan
        - getAveLifespan
        
        - assignIFR
        - getIFR
        
        - cleanDeathAges
        
        # assign/get data files 
        - assignDeathAges
        - getDeathAges
        - assignFIandQALY
        - getFIandQALY
        - assignFIcurves
        - getFIcurves
        
        # Other       
        - assignPath
    """
    
    # shared attributes
    Gamma0 = 0.00183
    path = ""
  
    # constructor
    def __init__(self, GNMpara, Dpara):
        
        # parameters
        
        self.GNMpara = GNM_parameters(GNMpara[0],GNMpara[1],GNMpara[2],\
                       GNMpara[3],GNMpara[4],GNMpara[5],GNMpara[6],\
                       GNMpara[7],GNMpara[8],GNMpara[9],GNMpara[10])
    
        self.Dpara = Disease_parameters(Dpara[0],Dpara[1],Dpara[2],Dpara[3],\
                     Dpara[4],Dpara[5],Dpara[6],Dpara[7],Dpara[8])
       
        self.file = File(self)
        self.data = Data()
        
        #self.groupAIndices = None

        
    
    # ---------------------------- Data Analysis ---------------------------- # 
     
   
    def assignAveYrLost(self):
        self.data.aveYrLost = self.getAveYrLost()
        
    
    def getAveYrLost(self):
        """
        aveYrLost: the difference in the average of the death ages distributions
        for the disease and control, where the effects of the disease beyond a
        certain age are censored. If disease impacts past, for example, 40 yr 
        are censored, than the average remaining lifespan of the 40 yr olds is 
        estimated using the control population.
             
        Output:
        aveYrLostTot sterrTot aveYrLost sterr windows (as a namedtuple)
        - aveYrLost is a list of aveYrLost values for various window, given
          by 'windows'
        - sterr is a list of corresponding sterr of the mean values
        """
        aveYrLost_obj = namedtuple('aveYrLost_obj', \
                       'aveYrLostTot sterrTot aveYrLost sterr windows')
        
        # Impacts of the disease are considered for different time windows:
        # from the start of the disease to the end of the disease plus some age 
        # in the list below
        windows = [0,5,10,20,30,40,50,60,70,80,90,100]
        
        # Get already cleaned death ages for disease and control groups
        da, daC, NAN = self.cleanDeathAges()
        
        # if not enough indiviudals are alive at the start of the disease, return nans
        n = np.nan
        if (NAN): 
            aveYrLost = np.empty(len(windows))
            sterr = np.empty(len(windows))
            aveYrLost.fill(n)
            sterr.fill(n)
            aveYrLost_obj = aveYrLost_obj(n, n, aveYrLost, sterr, windows)
            return aveYrLost_obj
        
        # if unable to upload data
        if (da is None):
            return None
        
        startAge = self.Dpara.t_on/self.Gamma0
        endAge = (self.Dpara.t_on + self.Dpara.tau)/self.Gamma0
        
        # plot death age dist. for disease and control
        import matplotlib.pyplot as plt
        da_bins = np.arange(startAge,120,1)
        fig, ax = plt.subplots(1, 1, figsize = (5,4))
        ax.hist(da,bins=da_bins,density=True,label="disease",alpha=.5,color ='#732B9E')
        ax.hist(daC,bins=da_bins,density=True,label="control",alpha=.5,color = '#d55e00')
        ax.set_xlabel("Death Age")
        ax.set_ylabel("P")
        ax.legend()
        
        # separately calculate the aveYrLost for an infinate window (no censoring)
        aveYrLostTot = []
        from scipy.stats import sem
        
        # Divide data up into 10 slices, calculate stats for each slice, determine
        # sterr of the mean using the 10 statistics
        from math import ceil
        m = ceil(len(da)/10)
        cuts = np.arange(0,11*m,m) # indices for slicing data into 10
        cuts[10] = len(da)
        for i in range(10):
            daSlice = da[cuts[i]:cuts[i+1]]
            daCSlice = daC[cuts[i]:cuts[i+1]]

            aveYrLostTot.append(np.average(daCSlice) - np.average(daSlice))
            
        sterrTot = sem(aveYrLostTot) 
        aveYrLostTot = np.average(aveYrLostTot)
        print("aveYrLostTot = {:.4f} +/- {:.4f}".format(aveYrLostTot, sterrTot))
        
        
        # Calculate the aveYrLost for different time windows. Deaths beyond the 
        # time window are censored such that only disease impacts during the 
        # time window are considered
        aveYrLost = []
        sterr = []
        
        for window in windows:
            print("window: {}".format(window))
            
            daD = da[da <= (endAge + window + 1e-8)]
            
            # If too few survive past the time window, then the following 
            # calculations can't be done (not enough people for the stats to 
            # be reliable). When very few survive, this indicates that the vast
            # majority of disease impacts, if not all, are within the window; 
            # therefore, using an infinate window is a good approximation.
            if (len(da)-len(daD)) < 10:
                print("too few survivors, just using aveYrLostTot")
                ave = aveYrLostTot
                st = sterrTot
                
            else:
                ave = []
                
                for i in range(10):
                    daSlice = da[cuts[i]:cuts[i+1]]
                    daCSlice = daC[cuts[i]:cuts[i+1]]
                    
                    # Average death age of control group
                    daC_ave = np.average(daCSlice) 
    
                    # In the disease group, get deaths during the time window
                    daD = daSlice[daSlice <= (endAge + window + 1e-8)]
        
                    # Fraction of indiv in the disease group who die within the time window
                    f = len(daD)/len(daSlice)
                    
                    # In the disease group, average death age of those who die 
                    # during the time window
                    daD_ave = np.average(daD)
                        
                    # Those in the control group that survive past time window
                    daC_S = daCSlice[daCSlice > (endAge + window + 1e-8)]
            
                    # average lifespan of disease group given censored data outside window
                    this = daD_ave*f + (np.average(daC_S))*(1-f)
                    
                    # subtract to get the average years lost due to disease
                    ave.append( daC_ave - this )
                    
                st = sem(ave)
                ave = np.average(ave)
                
                print("aveYrLost = {:.4f} +/- {:.4f}".format(ave, st))

            aveYrLost.append(ave) 
            sterr.append(st)
            
        aveYrLostObj = aveYrLost_obj(aveYrLostTot,sterrTot,aveYrLost,sterr,windows)

        return aveYrLostObj
    
        
    def assignAveLifespan(self):
        self.data.aveLifespan = self.getAveLifespan()
        
        
    def getAveLifespan(self):
        """
        aveLifespan: average lifespan of the population, with those who died 
        before the start of the disease censored
             
        Output:
        aveLifespan, sterr, aveLifespanC, sterrC, aveDiffLifespan, sterrDiff (as a namedtuple)
        """
        aveLifespan_obj = namedtuple('aveLifespan_obj', \
        'aveLifespan sterr aveLifespanC sterrC aveDiffLifespan sterrDiff')
        
        # Get already cleaned death ages for disease and control groups. Note: 
        # this removes those who die before the start of the disease
        da, daC, NAN = self.cleanDeathAges()
        
        # If not enough indiviudals are alive at the start of the disease, return nans
        if (NAN): 
            aveLifespan = aveLifespan_obj(np.nan,np.nan,np.nan,np.nan,np.nan,np.nan)
            return aveLifespan 
        
        # If unable to upload data
        if (da is None):
            return None
        
        aveLList = [[],[],[]]
        
        # Divde up data into 10 slices, calculate stats for each slice
        from math import ceil
        m = ceil(len(da)/10)
        cuts = np.arange(0,11*m,m) # indices for slicing data into 10
        cuts[10] = len(da)
        for i in range(10):
            daSlice = da[cuts[i]:cuts[i+1]]
            daCSlice = daC[cuts[i]:cuts[i+1]]
            
            # Average lifespan of those in the disease group (those who die before
            # the start of the disease are censored)
            aveLList[0].append(np.average(daSlice))
            # Average lifespan of those in the control group
            aveLList[1].append(np.average(daCSlice))
            # Difference in average lifespan: control - disease
            aveLList[2].append(aveLList[1][-1] - aveLList[0][-1])
        
        # Calculate ave and sterr of the mean based on the list of averages
        from scipy.stats import sem
        aveLifespan = aveLifespan_obj(np.average(aveLList[0]),sem(aveLList[0]),\
                      np.average(aveLList[1]), sem(aveLList[1]),\
                      np.average(aveLList[2]), sem(aveLList[2]))
            
        print("aveLifespan = {:.4f}+/-{:.4f}, aveLifespanC = {:.4f}+/-{:.4f}, "\
              "aveDiffLifespan = {:.4f}+/-{:.4f}".format(\
        aveLifespan.aveLifespan, aveLifespan.sterr, \
        aveLifespan.aveLifespanC, aveLifespan.sterrC, \
        aveLifespan.aveDiffLifespan, aveLifespan.sterrDiff))

        return aveLifespan
        

    def assignIFR(self):
        self.data.IFR = self.getIFR()


    def getIFR(self):
        """
        IFR: infection fatality rate
        100% * Number of deaths during the disease / number of people with the disease
             
        Output:
        IFR and sterr (as a namedtuple)
        """
        IFR_obj = namedtuple('IFR_object', 'IFR sterr IFR_C sterrC')
        
        # 1) get already cleaned death ages for disease and control groups
        da, daC, NAN = self.cleanDeathAges()
        
        # if not enough indiviudals are alive at the start of the disease, return nans
        if (NAN): 
            IFR = IFR_obj(np.nan, np.nan, np.nan, np.nan)
            return IFR 
        
        # if unable to upload data
        if (da is None):
            return None
        
        endAge = (self.Dpara.t_on + self.Dpara.tau)/self.Gamma0
        
        IFR_list = []
        IFR_C_list = []
        
        # 2) divde up data into 10, calculate stats for each group
        from math import ceil
        m = ceil(len(da)/10)
        cuts = np.arange(0,11*m,m) # indices for slicing data into 10
        cuts[10] = len(da)
        for i in range(10):
            daSlice = da[cuts[i]:cuts[i+1]]
            daCSlice = daC[cuts[i]:cuts[i+1]]
            
            # 3) Count the # of indiv who die during the disease
            daDuring = len(daSlice[daSlice <= endAge  + 1e-8])
            diedDuringC = len(daCSlice[daCSlice <= endAge  + 1e-8])
            
            IFR_list.append( daDuring*100/len(daSlice) )
            IFR_C_list.append( diedDuringC*100/len(daCSlice) )
            
        # 4) calculate ave and sterr of the mean based on the list of averages
        from scipy.stats import sem
        IFR = IFR_obj(np.average(IFR_list), sem(IFR_list), \
                         np.average(IFR_C_list), sem(IFR_C_list))
            
        print("IFR = {:.4f}+/-{:.4f}, IFR_C = {:.4f}+/-{:.4f}".format(\
        IFR.IFR, IFR.sterr, IFR.IFR_C, IFR.sterrC))

        return IFR
    

    def cleanDeathAges(self):
        NAN = False
        
        # 1) check if raw death ages have been assigned (are in memory)
        # if not, get the data
        if (self.data.deathAges is None):
            # 2) if file is missing, return None
            da = self.getDeathAges(False) # if file is missing, will return None
            if (da is None):
                # also check if control file is missing
                daC = self.getDeathAges(True)
                return None, None, NAN
        else:
            da = self.data.deathAges
            
        # 3) determine the fraction of the disease population that was too sick 
        # to get the disease -> percentTooSick
        nanIndices = ~np.isnan(da)
        da = da[nanIndices]
        tooSickSize = self.GNMpara.Number - len(da)
        startAge = self.Dpara.t_on/self.Gamma0
        # 4) remove those who died before the disease started
        beforeStartIndices = da >= startAge
        da = da[beforeStartIndices]
        print("{} indiv lived to the start of the disease".format(len(da)))

        # 5) assign percent tooSick
        percentTooSick = tooSickSize*100/len(da)
        self.data.percentTooSick = percentTooSick
        if (percentTooSick > 1.0):
            print("percentTooSick larger than 1%")
        
        # 6) If there are fewer than 10 people alive when the disease starts, 
        #    return nans
        if (len(da) < 10):
            print("not enough people entering the disease (< 10)")
            NAN = True
            return None, None, NAN
        
        # 7) Get control data
        if (self.data.deathAgesC is None):
            daC = self.getDeathAges(True)
            if (daC is None):
                return None,None, NAN  
        else:
            daC = self.data.deathAgesC
          
        # 9) remove tooSick and those who died before the disease from the control data
        daC = daC[nanIndices]
        daC = daC[beforeStartIndices]
        
        return da, daC, NAN
    
    
    # ------------------------ Upload Raw Data Files ------------------------ #   
    
    def assignDeathAges(self, control):
        if not control:
            self.data.deathAges = self.getDeathAges(control)
        else:
            self.data.deathAgesC = self.getDeathAges(control)
        
        
    def getDeathAges(self, control):
        from pandas import read_csv
        fn = self.file.getDeathAgesFN(control)
        try:
            data = read_csv(self.path+fn, sep='\t', header=None)
        except OSError as e:
            print(self.path+fn)
            print("FILE NOT FOUND, continuing")
            return None
        
        # check that the number of rows matches the number of individuals
        if (len(data) != self.GNMpara.Number):
            print("file should have " + str(self.GNMpara.Number) + \
                  " individuals but only has " + str(len(data)) + " rows")
            return None
        
        data = (data.values[:,0])/self.Gamma0
        return data
    
    
    def assignFIandQALY(self, control):
        if not control:
            self.data.FIandQALYdata = self.getFIandQALY(control)
        else:
            self.data.FIandQALYdataC = self.getFIandQALY(control)
        
        
    def getFIandQALY(self, control):
        from pandas import read_csv
        fn = self.file.getFIandQALYFN(control)
        try:
            data = read_csv(self.path+fn, sep='\t', header=None)
        except OSError as e:
            print(self.path+fn)       
            print("FILE NOT FOUND, continuing")
            return None
        
        # check that the number of rows matches the number of individuals
        if (len(data) != self.GNMpara.Number):
            print("file should have " + str(self.GNMpara.Number) + \
                  " individuals but only has " + str(len(data)) + " rows")
            return None
        
        # pandas data frame to numpy array
        data = data.values
        return data
        

    def assignFIcurves(self, control):
        if not control:
            self.data.FIcurvesData = self.getFIcurves(control)
        else:
            self.data.FIcurvesDataC = self.getFIcurves(control)
        
        
    def getFIcurves(self, control):
        from pandas import read_csv
        fn = self.file.getFIcurveFN(control)
        try:
            data = read_csv(self.path+fn, sep='\t', header=None)
        except OSError as e:
            print(self.path+fn)       
            print("FILE NOT FOUND, continuing")
            return None
        
        # check that the number of rows matches the number of individuals
        if (len(data) != self.GNMpara.Number):
            print("file should have " + str(self.GNMpara.Number) + \
                  " individuals but only has " + str(len(data)) + " rows")
            return None
        
        # pandas data frame to numpy array
        data = data.values
        return data        
        

    # -------------------------------- Other -------------------------------- #
        
    def assignPath(self, path):
        self.path = path              




class GNM_parameters:
    def __init__(self, Gammap, Gamman, N, Number, R, nd, Alpha, AveDeg, pA, \
                 TopologyID, Mortality):
        self.Gammap = Gammap # damage rate constant
        self.Gamman = Gamman # repair rate constant
        self.N = N # number of nodes
        self.Number = Number # number of individuals
        self.R = R # Gamma+/Gamma-
        self.nd = nd # number of mortality nodes
        self.Alpha = Alpha #scale free exponent
        self.AveDeg = AveDeg # average node degree
        self.pA = pA # if > 0, will modify the assortativity of the network
        self.TopologyID = TopologyID # ID representing seed for generating network
        self.Mortality = Mortality # Mortality condition, either "AND" or "OR", 
 
       
class Disease_parameters:
    def __init__(self, IndivSeedFile, t_on, tau, k_min, k_max, knn_min, knn_max, m, r):
        self.IndivSeedFile=IndivSeedFile # Num of IndivSeedFile used to seed each individual
        self.t_on = t_on # onset age of disease, unitless (must be scaled by Gamma0)
        self.tau = tau # length of disease, unitless
        self.k_min = k_min # min degree of nodes targed by the disease
        self.k_max = k_max # max degree of nodes targed
        self.knn_min = knn_min # min nearest neighbour degree of nodes targed
        self.knn_max = knn_max # max nearest neighbour degree of nodes targed
        self.m = m # fraction of N nodes targed by the disease
        self.r = r # fraction of damaged nodes repaired
    
    
class File:
    def __init__(self, d): # d = disease object
            
        self.file_ending = "Gammap{}Gamman{}N{}Number{}R{}nd{}Alpha{}" \
        "AvgDeg{}{}TopologyID{}pA{}IndivSeedFile{}t_on{}tau{}k_min{}k_max{}" \
        "knn_min{}knn_max{}m{}r{}".format(d.GNMpara.Gammap, d.GNMpara.Gamman, \
        d.GNMpara.N, d.GNMpara.Number,d.GNMpara.R, d.GNMpara.nd, d.GNMpara.Alpha, \
        d.GNMpara.AveDeg, d.GNMpara.Mortality, d.GNMpara.TopologyID, d.GNMpara.pA, \
        d.Dpara.IndivSeedFile,d.Dpara.t_on, d.Dpara.tau, d.Dpara.k_min, \
        d.Dpara.k_max, d.Dpara.knn_min, d.Dpara.knn_max, d.Dpara.m, d.Dpara.r)
            

    def getDeathAgesFN(self, control):
        # control : bool        
        if control:
            return "RawDeathAgesC" + self.file_ending
        else:
            return "RawDeathAges" + self.file_ending
        
    def getNetDegFN(self, control):
        if control:
            return "NetworkDegreesC" + self.file_ending
        else:
            return "NetworkDegrees" + self.file_ending
    
    def getFIandQALYFN(self, control):
        if control:
            return "FIandQALYC" + self.file_ending
        else:
            return "FIandQALY" + self.file_ending
    
    def getFIcurveFN(self, control):
        if control:
            return "FIcurveC" + self.file_ending
        else:
            return "FIcurve" + self.file_ending
    
    def getAveFIcurveFN(self):
        return "aveFIcurve" + self.file_ending
    # will need to inlcude the method that averages FIcurves
        
    def getSurvivorCurveFN(self):
        return "survivorshipCurve" + self.file.file_ending
    

class Data:
    def __init__(self):
        # raw file data
        self.deathAges = None
        self.deathAgesC = None
        self.FIandQALYData = None
        self.FIandQALYDataC = None
        self.FIcurveData = None
        self.FIcurveDataC = None

        # FI vs age curve for population
        self.aveFIcurves = None # columns: age, FItot, sem, FI30, sem
        
        # survival vs age for population
        self.survivorCurve = None # columns: age, fraction alive
        
        self.IFR = None # infection fatality rate
        
        self.aveLifespan = None
        
        # average different in FIend - FIstart
        # ((ave,sem),(control ave, control sem))
        self.diffFItot = None
        self.diffFI30 = None
        
        # average years lost for the population due to disease
        self.aveYrLost = None
        
        # other
        self.percentTooSick = None
        
#%%