// Imports a file with a list of seeds
// Each seed is for a different run (different individual)
// Allows initial trajectories of matched individuals from the
// disease and control group to be indentical
void getSeedVector(std::vector<int> &SeedVector) {
    // get file number to use
	int Num = Parameters::IndivSeedFile;

	// Open file
    std::ifstream IndivSeedFile;
    IndivSeedFile.open("IndivSeedFile" + std::to_string(Num));
    int lineCounter = 0;
    int Seed;
    while (lineCounter < Parameters::Number){
        IndivSeedFile >> Seed;
        SeedVector.push_back(Seed);
        lineCounter++;
    }
    IndivSeedFile.close();
}
