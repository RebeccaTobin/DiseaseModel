#pragma once
#include "HealthCalc.h"

class OutputDataFile : public DataFile {

public:

    using DataFile::DataFile; //Same constructor for names

    // save current data to temporary data files for later analysis
    void SaveData(std::vector<FIandQALYvals> &FIandQALYArr);
    void SaveData(std::vector<FICurve> &FIcurveArr);

    // close output file and remove memory used by vectors
    void FlushDataFile(std::vector<FIandQALYvals> &FIandQALYArr) {
    	FIandQALYOut.close();
    };

    void FlushDataFile(std::vector<FICurve> &FIcurveArr) {
    	FICurveOut.close();
    };

    void Open() { // Open the output file
        FIandQALYOut.open(FIandQALYName.c_str());
        if(SimulateVar::MortalityAndQALYOnly_bool == false)
        	FICurveOut.open(FIcurveName.c_str());

    }

    std::ofstream FIandQALYOut;
    std::ofstream FICurveOut;

};
