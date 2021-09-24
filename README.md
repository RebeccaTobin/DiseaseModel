# DiseaseModel

To run program: 
- go to DiseaseModel directory in command line and run the "run.sh" script (i.e. ./run.sh)
- you will first need to install a C++ compiler and the Boost libraries
- modify the run.sh file to control the disease parameters and the number of simulated individuals

How the program works:
(see "Network model of human aging: Frailty limits and information measures" by Farrell et. al. for more details)

This program stochastically simulates the health trajectories of "individuals" in the presence of a 
(roughly) 6 parameter generic disease. First, the program generates a scale-free network, where the
nodes represent generic health indicators. Nodes can be either undamaged or damaged. All nodes start
undamaged. The accumulating of damaged nodes in the network represent the accumulation of health 
deficits with age.

Nodes damage accoring to a damage rate which increases exponentially with the fraction of the node's neighbours
that are damaged (local frailty, f). Because of this, damage events tends to be random at early ages and more attributable to 
propagation at older ages. Once a node is damaged, it is assigned a repair rate which decreases with f. What node is 
damaged/repaired and when is governed by the Gillespie algorithm (or Kinetic Monte Carlo), which allows for a
stochastic realization of the rates of all nodes. 

NOT FINISHED



Credit:
The original generic network model (GNM) code of which this project is based was written by Spencer Farrell.
The follow are the files that I wrote/changed a significant portion of:
- Files.h
- FindNodes.h
- GillespieTree.h
- HealthCalc.h
- ImportSeeds.h
- main.cpp
- SaveData.h
