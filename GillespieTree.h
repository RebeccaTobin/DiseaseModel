#pragma once
#include <vector>
/*
 * methods for controlling damage and repair events
 */


// Time to an event given the rates
double TimeToEvent(double TotalRate) {
    
    using namespace Parameters;
    return -1.0/TotalRate * log(RNG(g2));

}


// Repair rate given parameters and local frailty
double DamageRate(double f) {

    using namespace Parameters;
    return exp(gammap * f);

}

// Damage rate given parameters and local frailty
double RepairRate(double f) {

    using namespace Parameters;
    return 1/R * exp(-gamman * f);

}

// calculate and set the rate of a node and all of its neighbours, as well as the cumulative rate
// also update deficits counters for health measure calculations
void CalculateRates(std::vector<Node> &Network, Node &X, EventTree &tree, int j, double &TotalRate, double Time, int run) {

    using namespace H;

	for(int i: X.Neighbours) {

		//remove current from total rate
        TotalRate -= Network[i].Rate;

        //update rates on the network
        if(Network[i].d == 0) Network[i].Rate = DamageRate(Network[i].f);
       
        else if(Network[i].d == 1) Network[i].Rate = RepairRate(Network[i].f);

        //update rates on the tree
        tree.Update(i, Network[i].Rate);

        //update new total rate
        TotalRate += Network[i].Rate;
    }

    //update the X node
    TotalRate -= X.Rate;
    if(X.d == 0){
    	X.Rate = DamageRate(X.f);
    	// note the repair event in deficit count (tot-> all nodes, 30-> top 30 nodes)
    	deficits_tot--;
    	if (std::binary_search(top30Nodes.begin(), top30Nodes.end(), X.id)){
    		deficits30--;
    	}
    }
    else if(X.d == 1){
    	X.Rate = RepairRate(X.f);
    	// note the damage event in deficit count
    	deficits_tot++;
    	if (std::binary_search(top30Nodes.begin(), top30Nodes.end(), X.id)){
    		deficits30++;
    	}
    }

    tree.Update(j, Network[j].Rate);

    TotalRate += X.Rate;

}

// damage event at the start of the disease
// loop over all nodes in IDs vector and damage them, then change the local frailties and rates
void DamageNodes(std::vector<Node> &Network, EventTree &tree, double &TotalRate, double Time, int run, std::vector<int> IDs) {
    // Note: index of node is the same as its id
    
    // loop over nodes in IDs vector
    for(int ID: IDs) {

    	//std::cout << "ID: " << ID << ", d: " << Network[ID].d << std::endl;

    	// this does what UpdateNode() normally does
    	if (Network[ID].d == 0){ // if node is undamaged

		Network[ID].d = 1; // damage node

		// add to count of deficits
		H::deficits_tot++;
    	if (std::binary_search(H::top30Nodes.begin(), H::top30Nodes.end(), ID)){
    		H::deficits30++;
    	}

    	// this does what UpdateLocalFrailty() does
    	// update the local frailty of the node and its neighbours
		double sum = 0;
		for(auto i: Network[ID].Neighbours) {
			sum += Network[i].d;
			Network[i].f += 1.0/Network[i].k;
		}
		Network[ID].f = sum/Network[ID].k;

    	// this does what CalculateRates() does
		// update TotalRate, update damage/repair rate of the node and its neighbours
        for(int i: Network[ID].Neighbours) {
            TotalRate -= Network[i].Rate;
            if(Network[i].d == 0) Network[i].Rate = DamageRate(Network[i].f);
            else if(Network[i].d == 1) Network[i].Rate = RepairRate(Network[i].f);
            tree.Update(i, Network[i].Rate);
            TotalRate += Network[i].Rate;
        }
        TotalRate -= Network[ID].Rate;
        Network[ID].Rate = RepairRate(Network[ID].f);
        tree.Update(ID, Network[ID].Rate);
        TotalRate += Network[ID].Rate;

    	}
    }
}

// repair event at the end of the disease
// loop over all nodes in IDs vector and repair them, then change the local frailties and rates
void RepairNodes(std::vector<Node> &Network, EventTree &tree, double &TotalRate, double Time, int run, std::vector<int> IDs) {
    // Note: index of node is the same as its id

    // loop over nodes in IDs vector
    for(int ID: IDs) {

    	//std::cout << "ID: " << ID << ", d: " << Network[ID].d << std::endl;

    	// this does what UpdateNode() normally does
    	if (Network[ID].d == 1){ // can ignore nodes that aren't damaged

			Network[ID].d = 0;

			// subtract number of deficits
			H::deficits_tot--;
			if (std::binary_search(H::top30Nodes.begin(), H::top30Nodes.end(), ID)){
				H::deficits30--;
				//std::cout << "ID " << ID << " repaired" << std::endl;
			}

			// this does what UpdateLocalFrailty() does
			double sum = 0;
			for(auto i: Network[ID].Neighbours) {
				sum += Network[i].d;
				Network[i].f -= 1.0/Network[i].k; // subtraction assumes node was repaired
			}
			Network[ID].f = sum/Network[ID].k;

			// this is what CalculateRates() does
			for(int i: Network[ID].Neighbours) {
				TotalRate -= Network[i].Rate;
				if(Network[i].d == 0) Network[i].Rate = DamageRate(Network[i].f);
				else if(Network[i].d == 1) Network[i].Rate = RepairRate(Network[i].f);
				tree.Update(i, Network[i].Rate);
				TotalRate += Network[i].Rate;
			}
			TotalRate -= Network[ID].Rate;
			Network[ID].Rate = DamageRate(Network[ID].f);
			tree.Update(ID, Network[ID].Rate);
			TotalRate += Network[ID].Rate;

    	}
        //std::cout << "After repair d is: " << Network[ID].d << std::endl;
    }
}


// finds rate and returns index of rate to be performed
int FindRate(EventTree &tree, double NetworkRate) {

    double val = NetworkRate * RNG(g2) ; // temp

    int i = tree.Search(val);
    assert(i < Parameters::N);
    
    return i;

}
