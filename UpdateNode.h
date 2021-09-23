

// Perform damage/repair on the node
void UpdateNode(Node &X, double &Time, int run) {

    //perform event
    X.d = (X.d + 1)%2; //0->1, 1->0

}

// Increment time
void UpdateTime(double &Time, double TotalRate) {
    
    Time += TimeToEvent(TotalRate);
}

// Update local frailty of a node and its neighbours after damage/repair of that node
void UpdateLocalFrailty(std::vector<Node> &Network, Node &X) {

	double sum = 0;

	// update neighbours
	for(auto i: X.Neighbours) {

		sum += Network[i].d; 
		if(X.d == 0) Network[i].f -= 1.0/Network[i].k;
		if(X.d == 1) Network[i].f += 1.0/Network[i].k;

	} 

	// update this node
	X.f = sum/X.k;

}
