#include <sstream>
/*
 * methods for output data files
 */

//turn doubles into strings without the trailing zeros (used for generating file names)
std::string to_string(double x) {

	// want to string to be in scientific notation if it is a small enough order of magnitude
	if ((log10(x) < (-4.0 + 1e-7)) && (x > 1e-7)){
		// Create an output string stream
		std::ostringstream streamObj;
		//Add double to stream
		streamObj << x;
		// Get string from output string stream
		std::string strObj = streamObj.str();
		return strObj;
	}

	std::string temp = std::to_string(x);
    temp.erase ( temp.find_last_not_of('0') + 1, std::string::npos );
    if(*temp.rbegin() == '.') temp = temp + '0';
    return temp;

}

// turn ints into strings
std::string to_string(int x) { return std::to_string(x);}


// tag to append to filenames to indicate parameters
std::string SetRawName(int NNodes = Parameters::N) {

    std::string str;
	using namespace Parameters;

	// indicate whether disease was "given" or not
	if (control.compare("C"))
		str = "";
	else
		str = "C"; // no disease

    return str + "Gammap" + to_string(gammap) + "Gamman" + to_string(gamman) + "N"
    + std::to_string(NNodes)  + "Number" + std::to_string(int(Number)) + "R"
	+ to_string(R) + "nd" + std::to_string(nd) + "Alpha" + to_string(alpha) + "AvgDeg"
	+ std::to_string(avgdeg) + MortalityCondition + "TopologyID"
	+ std::to_string(TopologyID) + "pA" + to_string(p_assortativity)
	+ "IndivSeedFile" + std::to_string(IndivSeedFile) + "t_on" + to_string(t_on)+ "tau" + to_string(tau)
	+ "k_min" + std::to_string(k_min) + "k_max" + std::to_string(k_max)
	+ "knn_min" + std::to_string(knn_min) + "knn_max" + std::to_string(knn_max) + "m"
	+ to_string(m) + "r" + to_string(r);
}


// set the folder to output Data to
std::string SetFolder() {
	
    if (Parameters::Folder == "Local")
	return "Data/";
    else
	return "../scratch/RawData/" + Parameters::Folder + "/";
	
}

// folder for temporary raw Data
std::string TempFolder() {

    if (Parameters::Folder == "Local")
	return "Data/";
    else
	return "../scratch/TempData/" + Parameters::Folder + "/";

}

//check if a file exists
inline bool FileExists (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

