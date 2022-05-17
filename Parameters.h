#ifndef PARAMETERS_H
#define PARAMETERS_H


// All global parameters used.
// Initialized with cmdline arguments by SetParameters(argc, argv)
namespace Parameters {
	
    // Original GNM parameters
    double R; // Ratio of damage to repair
    int N; // Number of nodes in the network
    double gammap; // Damage rate constant
    double gamman; // Repair rate constant
    int nd; // Number of mortality nodes
    int Number; // Number simulated individuals
    double alpha; // Scale free distribution exponent
    int avgdeg; // Average node degree
    std::string MortalityCondition; // "OR" or "AND"
    std::string Folder; // "Local" uses the Data/ folder. Otherwise use scratch space on a computing cluster.
    int TopologyID; // index of Topology seed file to use
    int TopologySeed; // Seed used to make the topology
    double p_assortativity; //if > 0, will modify the assortativity of the network. https://en.wikipedia.org/wiki/Assortativity
    
    int IndivSeedFile; // Which file to use to seed each individual

    // Disease Parameters
    double t_on;  // Age at which the disease starts
    double tau;  // length of disease
    int k_min;  // Minimum connectivity of nodes to be damaged at a different rate
    int k_max;  // Maximum connectivity of nodes to be damaged at a different rate
    int knn_min;  // Minimum ave neighbour connectivity of nodes to be damaged at a different rate
    int knn_max;  // Maximum ave neighbour connectivity of nodes to be damaged at a different rate
    double m; // Fraction of N nodes to be damaged at a different rate; m*N must be <= number of eligable nodes
    double r; // Fraction of p nodes repaired at the end of the disease
    std::string control; // If "C", no disease
    
    int M; // M = m*N
    double EndTime; // StartTime + tau
}	


// Get parameters from cmd line and set them
void SetParameters(int argc, char *argv[]) {

	using namespace Parameters;

	Number = atoi(argv[1]); std::cout << "Number: " << Number;
	R = atof(argv[2]); std::cout << ", R: " << R;
	gammap = atof(argv[3]); std::cout << ", gammap: " << gammap;
	gamman = atof(argv[4]); std::cout << ", gamman: " << gamman;
	N = atoi(argv[5]); std::cout << ", N: " << N;
	nd = atoi(argv[6]); std::cout << ", nd: " << nd;
	alpha = atof(argv[7]); std::cout << ", alpha: " << alpha;
	avgdeg = atoi(argv[8]); std::cout << ", avgdeg: " << avgdeg;
	MortalityCondition = argv[9]; std::cout << ", MortalityCondition: " << MortalityCondition;
	Folder = argv[10];
	TopologyID = atoi(argv[11]); std::cout << ", TopologyID: " << TopologyID;
    p_assortativity = atof(argv[12]);  std::cout << ", p_assortativity: " << p_assortativity;
    
    IndivSeedFile = atoi(argv[13]); std::cout << ", IndivSeedFile: " << IndivSeedFile;

    t_on = atof(argv[14]); std::cout << ", t_on: " << t_on;
    tau = atof(argv[15]); std::cout << ", tau: " << tau;
    k_min = atoi(argv[16]); std::cout << ", k_min: " << k_min;
    k_max = atoi(argv[17]); std::cout << ", k_max: " << k_max;
    knn_min = atoi(argv[18]); std::cout << ", knn_min: " << knn_min;
    knn_max = atoi(argv[19]); std::cout << ", knn_max: " << knn_max;
    m = atof(argv[20]); std::cout << ", m: " << m;
    r = atof(argv[21]); std::cout << ", r: " << r;
	
    M = round(m*N); std::cout << ", m*N: " << M;
    
    control = argv[22];
    if (!control.compare("C")) {
    	std::cout << ", control true";
    }

    EndTime = t_on + tau;
   
	std::cout << std::endl;
	std::cout << std::endl;
	

	std::ifstream SeedFile;
	SeedFile.open("TopologySeedFile");
	int lineCounter = 0;
	int Seed;
	SeedFile >> Seed;
	while(lineCounter < TopologyID) {

	    SeedFile >> Seed;
	    lineCounter++;
	}
	
	//std::cout << "TopologySeed: " << Seed << std::endl;
	TopologySeed = Seed;

	SeedFile.close();
}


namespace SimulateVar {
    bool MortalityOnly_bool = false;
    bool MortalityAndQALYOnly_bool = false;
}


// If you don't want any health output, add "MortalityOnly" to "CommandFile_Simulate.txt".
// If you don't want the FIcurve file, add "MortalityAndQALYOnly" to "CommandFile_Simulate.txt".
void SetSimulate() {

    std::ifstream File;
    File.open("CommandFile_Simulate.txt");
    std::string line;

    while(std::getline(File, line)){

	std::cout << line << std::endl;

	if(line == "MortalityOnly") SimulateVar::MortalityOnly_bool = true;
	else if(line == "MortalityAndQALYOnly") SimulateVar::MortalityAndQALYOnly_bool = true;

    }
		
    File.close();

}


#endif
