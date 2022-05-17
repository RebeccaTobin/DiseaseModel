# DiseaseModel

Before you run the program:
- In the file "run.sh," the variable "Folder" is set to "Local." If this is not changed, you will need to create a folder called "Data" (for output data) in the main DiseaseModel folder
- Before running the program, you will also need to run the python scripts "MakeIndivSeedFiles.py" and "MakeTopologySeedFiles.py" to generate files containing random numbers

To run the program:
- The program is designed to be run from the command line. To do this, go to DiseaseModel directory in command line and run the "run.sh" script (i.e. ./run.sh)
- You will first need to install a C++ compiler and the Boost libraries
- Modify the run.sh file to control the disease parameters and the number of simulated individuals


Credit:
The original generic network model (GNM) code of which this project is based was written by Spencer Farrell.
The follow are the files that I wrote/changed a significant portion of:
- Files.h
- FindNodes.h
- GillespieTree.h
- HealthCalc.h
- ImportSeeds.h
- main.cpp
- SaveData.h

