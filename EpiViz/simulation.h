//
//  simulation.h
//  epiViz
//
//  Created by Matthew Farmer on 3/17/13.
//  Copyright (c) 2013 Matthew Farmer. All rights reserved.
//

#ifndef __epiViz__simulation__
#define __epiViz__simulation__

#include <iostream>
#include <list>
#include <cstdlib>
#include <fstream>
#include "entity.h"
#include "disease.h"

#include <omp.h>
#include "CImg.h"

using namespace cimg_library;
using namespace std;

const int dimension = 50;
const float epiVizVersionNumber = 1.0;
const std::string diseaseListFileName = "diseaseList.txt";

class simulation
{
private:
    int currentDay;
    int maxDay;
    int susceptiblePopulation;
    int infectedPopulation;
    int vaccinatedPopulation;
    int immunePopulation;
    int deadPopulation;
    
    entity grid[dimension][dimension];
    
	std::list<entity> infectionQueue;
	std::list<disease> diseaseQueue;
	std::list<entity> vaccinationQueue;
	
	disease chosenDisease;
	
	//Private Methods
    void startSimulation();
	void initializeGrid();
	
	//Output
	void writeHtmlHeader();
	void writeHtmlTable();
	void writeHtmlFooter();
    void animateImage(CImg<unsigned char> &x);
	void randomlyInfectFirstEntity();
    void spreadInfection();
	void spreadVaccination();
	void dispatchVaccinationPods();
	int printDiseaseOptions();
	void worldWrap(int &row, int &col);
	disease printMainMenu();
	disease determineMenuChoice(int n, int numberOfOptionsPrinted);
	disease createNewDisease();
	void loadDiseaseList();
	void attemptInfectionAt(int row, int col);
	void attemptVaccinationAt(int row, int col);
	void determineRemovedState(int row, int col);
	
	//Input Validation
	int getValidInteger(std::string prompt, int inclusiveLowRange, int inclusiveHighRange);
	
public:
    simulation(int maxDay);
    void begin();
	void printSimulationInfo();
	void printGridInfo();
};

#endif /* defined(__epiViz__simulation__) */
