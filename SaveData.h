#ifndef SAVEDATA_H
#define SAVEDATA_H

// write FI and QALY (health) measures to a file that begins with "FIandQALY..."
void OutputDataFile::SaveData(std::vector<FIandQALYvals> &FIandQALYArr) {
    
    std::ios::sync_with_stdio(false);

    for(const auto &x: FIandQALYArr) {

    	if (x.getFItotstart() < 0) FIandQALYOut << "nan" << "\t" << "nan" << "\t";
    	else FIandQALYOut << x.getFItotstart() << "\t" << x.getFI30start() << "\t";

    	if (x.getFItotend() < 0) FIandQALYOut << "nan" << "\t" << "nan" << "\t";
    	else FIandQALYOut << x.getFItotend() << "\t" << x.getFI30end() << "\t";

    	if (x.getFItotdeath() < 0) FIandQALYOut << "nan" << "\t" << "nan" << "\t";
    	else FIandQALYOut << x.getFItotdeath() << "\t" << x.getFI30death() << "\t";

    	FIandQALYOut << x.getQALYtot() << "\t" << x.getQALY30() << "\n";
    }
    
    FIandQALYArr.clear();
    
}

// write regularly sampled FI values to a file that begins with "FIcurve..."
void OutputDataFile::SaveData(std::vector<FICurve> &FIcurveArr) {

	//std::cout << "size of FIcurve vector before saving is " << FIcurveArr.size() << std::endl;

    std::ios::sync_with_stdio(false);

    for(const FICurve &FIcurve: FIcurveArr) {

        for (float FI : FIcurve.getFIcurvetot()){
        	if (FI < 0.0) FICurveOut << "nan" << "\t";
        	else FICurveOut << FI << "\t";
        }
        FICurveOut << "\n";

        for (float FI : FIcurve.getFIcurve30()){
        	if (FI < 0.0) FICurveOut << "nan" << "\t";
        	else FICurveOut << FI << "\t";
        }
        FICurveOut << "\n";
    }

    FIcurveArr.clear();
}

// write death ages to a file that begins with "RawDeathAges..."
void OutputDeathAges(const std::vector<double> &DeathAges, int OriginalN) {

    std::string name = TempFolder() + "RawDeathAges" + SetRawName(OriginalN);
    std::ofstream Output;
    Output.open(name.c_str());

    for(double x: DeathAges){
    	// those too sick for the disease are identified by a "nan" death age
    	if (x < 0.0) Output << "nan" << "\n";
    	else Output << x << "\n";
    }

    Output.close();
    
}

#endif
