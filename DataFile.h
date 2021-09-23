/*
 * Class to hold names of output data files
 */
class DataFile {

public:

    DataFile(int NNodes = Parameters::N) {
	
	FIandQALYName = TempFolder() + "FIandQALY" + SetRawName(NNodes);
	FIcurveName = TempFolder() + "FIcurve" + SetRawName(NNodes);
	
    }
    
    std::string FIandQALYName;
    std::string FIcurveName;

};

