#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <string>
#include <assert.h>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include <chrono>

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

// Random number generator, mersenne twister
std::mt19937 g1(1); // For setting up Network
std::mt19937 g2(2); // For picking nodes to damage
std::uniform_real_distribution<double> RNG(0.0,1.0); 

#include "Parameters.h"
#include "Files.h"
#include "Node.h"
#include "DataFile.h" 
#include "OutputDataFile.h" 
#include "SaveData.h"
#include "2Dvector.h"
#include "tree.h" 
#include "GillespieTree.h"
#include "UpdateNode.h"
#include "MortalityNodes.h"
#include "ScaleFreeNetworkTree.h"
#include "MortalityRules.h"
#include "OutputNetwork.h"
#include "AssortativeMixing.h"
#include "SetUpNetwork.h"
#include "FindNodes.h"
#include "ImportSeeds.h"
#include "HealthCalc.h"

int main(int argc, char *argv[]) {

	// for comparing doubles
	const float eps = 2*std::numeric_limits<double>::epsilon();

    using namespace Parameters; //Using the parameters from cmd line
    SetParameters(argc, argv); //Set initial parameters from command line
    SetSimulate();
    std::cout << "Parameters Loaded" << std::endl;
    
    //Network
    const int OriginalN = N; //some network types remove unconnected nodes, lowering N. Original N will not change
    std::vector<Node> Network;
    std::vector<int> MortalityNodes(nd);
    
    // Each row of vector is a death age for each indiv
    std::vector<double> DeathAges; DeathAges.reserve(Parameters::Number);
    
    // Each row of vector is an object that holds the FI at the start and end of the disease and at death and the QALYtot and QALY30 of each indiv
    std::vector<FIandQALYvals> FIandQALYArr;

    // Each row holds sampled FI values at evenly spaced times for a particular indiv
    std::vector<FICurve> FIcurveArr;

    // bools control the number of output data files
    if (!SimulateVar::MortalityOnly_bool){

		FIandQALYArr.reserve(10000); // every 10000 runs are written to file, then FIandQALYArr is emptied

		if (!SimulateVar::MortalityAndQALYOnly_bool){

			H::createSampleTimes(0.00183); // time step of 1 year if Gamma0 = 0.00183
			FIcurveArr.reserve(10000);
		}
    }

    // Data file to save intermediate Data
    OutputDataFile File(OriginalN);
    if (!SimulateVar::MortalityOnly_bool) File.Open();

    
    // Set up network
	g1.seed(Parameters::TopologySeed);
	SetUpNetwork(Network, MortalityNodes, OriginalN);
    
    // Get id's of nodes with connectivity that satisfies input parameters
    std::vector<int> IDs_all = FindNodestoDamage(Network, MortalityNodes);
    // some fraction of IDs_all will be chosen to be damaged by the disease
    std::vector<int> IDs;

    // search network and identify the top 30 most connected nodes (excluding the mortality nodes)
    H::top30Nodes = getTop30Nodes(Network);
    // sort so that binary search can be used to find if a particular node id is in the list
    std::sort (H::top30Nodes.begin(), H::top30Nodes.end());
    
    // get vector of seeds to initiate each indiv
    std::vector<int> IndivSeedVector;
    getSeedVector(IndivSeedVector);

    // loop through runs -> each run represents one simulated individual
    for(int run = 1; run <= Number; run++) {
	
    g2.seed(IndivSeedVector[run-1]);

    //std::cout << "\n" << "---------- indiv " << run-1 << " ----------- \n" << std::endl;

	EventTree tree(N, 1.0); 
	double TotalRate = N; // sum of rates (which are initially 1 for all nodes)
	double Time = 0; // current Time (unscaled age)
	
	// resets health-related measures for each individual
	H::reset();
	if (!SimulateVar::MortalityAndQALYOnly_bool)
		H::resetFIcurves();
        
    bool dead = false;
	bool start_trigger = false; // goes from false to true after the disease starts
	bool end_trigger = false; // goes from false to true after the disease ends

	// evolve in time until Mortality occurs
	while(!dead) {

        UpdateTime(Time, TotalRate);

        // Start of the disease
        if (!start_trigger && (Time+eps) >= Parameters::t_on) {

        	// sample FI at the start of the disease
        	if (!SimulateVar::MortalityOnly_bool) H::setFIstart(Parameters::N);

        	// seed using the clock (disease and control do not having matching
        	// trajectories past the start of the disease)
        	unsigned Seed = std::chrono::system_clock::now().time_since_epoch().count();
        	g2.seed(Seed);

        	// If nonzero nodes to damage and not control
        	if (Parameters::p > 0 && Parameters::control.compare("C")){

        		// damage event occurs at t_on -> forget original time step
				Time = Parameters::t_on;

				// add area to QALY and sample FI if Time >= sample time
				if (!SimulateVar::MortalityOnly_bool){

					H::addArea(Time, Parameters::N);
					H::t_low = Time;

					if (!SimulateVar::MortalityAndQALYOnly_bool){

						while ((Time + eps) >= H::getSampleTime()){

							H::sampleFI(Parameters::N);
						}
					}
				}

				// remove damaged nodes from IDs
				IDs = RemoveDamaged(Network, IDs_all);
				std::sort(IDs.begin(), IDs.end());

				// If there aren't enough undamaged nodes to damage, indiviudal is too
				// sick (damaged) to get the disease
				if (IDs.size() < Parameters::p){

					//std::cout << "size < p" << std::endl;
					H::tooSick = true;
				}

				else{

				// pick p random id's from eligible list
				PickRandomIDs(IDs);

				// damage nodes, update local frailty and rates
				DamageNodes(Network, tree, TotalRate, Time, run, IDs);
				}
        	}

        	start_trigger = true;
        } // start of disease, loop ended


        // End of the disease
        else if (!end_trigger && (Time+eps) > Parameters::EndTime) {

        	// If nonzero nodes to repair and not control
        	if ((Parameters::r+eps) > 0.0 && Parameters::p > 0 && Parameters::control.compare("C")) {

        		// repair event occurs at t_on + tau -> forget original time step
				Time = Parameters::EndTime;

				// add area to QALY and sample FI if Time >= sample time
				if (!SimulateVar::MortalityOnly_bool){
					H::addArea(Time, Parameters::N);
					H::t_low = Time;

					if (!SimulateVar::MortalityAndQALYOnly_bool){
						while ((Time + eps) >= H::getSampleTime()){
							H::sampleFI(Parameters::N);
						}
					}
				}

				// pick a random fraction r of the externally damaged nodes to reapir
				PickRandomRepairIDs(IDs);

				// repair nodes, update local frailty and rates
				RepairNodes(Network, tree, TotalRate, Time, run, IDs);
        	}

        	// sample FI after repair has taken place
        	if (!SimulateVar::MortalityOnly_bool){
        		H::setFIend(Parameters::N);
        	}

        	end_trigger = true;

        } // end of disease, end of loop

        else { // not start or end of the disease, the usual

        	// add area to QALY and sample FI if time >= sample time
			if (!SimulateVar::MortalityOnly_bool){
				H::addArea(Time, Parameters::N);
				H::t_low = Time;

				if (!SimulateVar::MortalityAndQALYOnly_bool){
					while ((Time + eps) >= H::getSampleTime()){
						H::sampleFI(Parameters::N);
					}
				}
			}

			int Index = FindRate(tree, TotalRate); // Find rate to be performed
			UpdateNode(Network[Index], Time, run); // perform rate and increase time
			UpdateLocalFrailty(Network, Network[Index]); // update the local frailty after the node is modified
			CalculateRates(Network, Network[Index], tree, Index, TotalRate, Time, run); // calculate new rates for the node and its neighbours
        }

	    if(Mortality(Network, MortalityNodes) == 1) dead = true; // evaluate mortality

        if (H::tooSick) dead = true; // Prematurely end because individual is too sick for the disease
	    
	}

	// record death age data
	if (H::tooSick) {
		DeathAges.push_back(-1);
	}
	else {
		DeathAges.push_back(Time);
		H::setFIdeath(Parameters::N);
	}

	if (!SimulateVar::MortalityOnly_bool){

		// add area one last time
		H::addArea(Time, Parameters::N);

		H::assignQALY();

		//std::cout << "FI and QALY values for this individual:" << std::endl;
		//H::FIandQALY.printVals();
		FIandQALYArr.emplace_back(H::FIandQALY);

		if (!SimulateVar::MortalityAndQALYOnly_bool){
			H::fillRemianingFIcurve();
			FIcurveArr.emplace_back(H::FIcurve);
		}

		//std::cout << "\n" << std::endl;
	}
	
	// IDs resets every run, but not IDs_all
	IDs.clear();

	// Reset rates and frailty
	for(auto &x: Network) x.Reset();
      
	// Save intermediate Data every 1000 individuals
	if(!SimulateVar::MortalityOnly_bool && run%10000 == 0 ) {

		// possibly change the 10000 (depends on memory restrictions)
	    File.SaveData(FIandQALYArr);

	    if(!SimulateVar::MortalityAndQALYOnly_bool)
	    	File.SaveData(FIcurveArr);
	}
	
	if (run%10000 == 0)
	    std::cout << "Run " << run << std::endl;

    
    } // runs end, next individual

    std::cout << "Outputing final data" << std::endl;

    // Save and output final bit of raw Data
    if (!SimulateVar::MortalityOnly_bool) {

		File.SaveData(FIandQALYArr);
		File.FlushDataFile(FIandQALYArr);

		if(!SimulateVar::MortalityAndQALYOnly_bool){

			File.SaveData(FIcurveArr);
			File.FlushDataFile(FIcurveArr);
		}
    }
    
    OutputDeathAges(DeathAges, OriginalN);
    std::cout << "Finished" << std::endl;
    
}

