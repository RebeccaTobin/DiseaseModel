/*
 * Methods for identifying nodes; related to the disease or calculating health measures
 */

// Returns the id's of top 30 most connected nodes excluding the mortality nodes
std::vector<int> getTop30Nodes(std::vector<Node> Network) {

	std::vector<int> top30Nodes;

	// Sort in descending order
	std::sort(Network.begin(), Network.end(), [](const Node &x, const Node &y){

		return x.k > y.k;

	});

	// Get the id's of the top 30 most connected nodes (excluding the mortality nodes)
	for(int i = Parameters::nd; i < Parameters::nd + 30; i++)

		top30Nodes.push_back(Network[i].id);

	return top30Nodes;
}

// Find the id's of all the nodes of degree within k and knn inputs
std::vector<int> FindNodestoDamage(std::vector<Node> &Network, const std::vector<int> &MortalityNodes) {
    
    std::vector<int> IDs;
    
    //std::cout << "id's of nodes with degree less than " << Parameters::k_max << " are: ";

    // Loop over all nodes
    for(int i = 0; i < Network.size(); i++) {

    	Node X = Network[i];

    	// exclude mortality nodes
    	if (i == MortalityNodes[0] or i == MortalityNodes[1]) continue;

        // Add id's to vector if node has degree k within range and has nearest neighbour average degree knn within range
        if (X.k <= Parameters::k_max && X.k >= Parameters::k_min && X.knn < Parameters::knn_max && X.knn >= Parameters::knn_min){

        	IDs.push_back(X.id);
            //std::cout << X.id << ", ";
        }
    }
    //std::cout << std::endl;
    
    if (IDs.empty() == true){
        std::cout << "There are no nodes with the k and knn ranges entered" << std::endl;
        std::cout << "Program aborted! Try different k and knn ranges" << std::endl;
        abort();
    }
    //std::cout << "Number of nodes in k and knn ranges entered: " << IDs.size() << std::endl;
    //std::cout << std::endl;

    return IDs;
}

// returns IDs vector without nodes that are damaged
std::vector<int> RemoveDamaged(std::vector<Node> &Network, std::vector<int> &IDs_all) {

	std::vector<int> IDs;

	//std::cout << "Undamaged IDs: ";

	for(int ID: IDs_all){

		//std::cout << X.d << ", ";

		if (Network[ID].d == 0) {

			IDs.push_back(ID);
			//std::cout << X.id << ", ";
		}
	}

	//std::cout << std::endl;
	//std::cout << "Number of nodes in IDs after removing damaged nodes: " << IDs.size() << std::endl;

	return IDs;
}

// pick M=m*N random nodes from IDs vector, returns IDs vector with nodes removed
void PickRandomIDs(std::vector<int> &IDs) {
    
    if (IDs.size() != Parameters::M) { // don't need to erase anything if there are M nodes in IDs

	if (IDs.size() > 1) {
        // Randomly shuffle vector
        std::shuffle(IDs.begin(), IDs.end(), g2);
        
        //std::cout << "The suffled node ids are:" << std::endl;
        //for(int a = 0; a < 10; a = a + 1) {
        //    std::cout << IDs[a] << ", ";
        //}
        //std::cout << "..." << std::endl;
    }
        
    IDs.erase( IDs.begin() + Parameters::M, IDs.end() );
    
    //std::cout << "IDs size is after erase is : " << IDs.size() << std::endl;
    
    //std::cout << "IDs after erase are:" << std::endl;
    //for(int a = 0; a < 10; a = a + 1) {
    //    std::cout << IDs[a] << ", ";
    //}
    //std::cout << "..." << std::endl;
    }
}

// picks a random fraction r of the nodes in the IDs vector, returns IDs vector with nodes erased
void PickRandomRepairIDs(std::vector<int> &IDs) {

	//std::cout << "IDs size is before erase is : " << IDs.size() << std::endl;

	// don't need to erase anything if r is 1 (repair all nodes)
	if (Parameters::r + 2*std::numeric_limits<double>::epsilon() > 1.0){

		return;
	}

	if (IDs.size() > 1) {
        // Randomly shuffle vector
        std::shuffle(IDs.begin(), IDs.end(), g2);
    }

	double r = Parameters::r*IDs.size();
	r = r + 0.5 - (r<0);
	int p = (int)r;

    IDs.erase(IDs.begin()+p, IDs.end());

    //std::cout << "IDs size is after erase is : " << IDs.size() << std::endl;


}
