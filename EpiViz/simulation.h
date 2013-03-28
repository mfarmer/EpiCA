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
#include "zone.h"

#include <omp.h>
#include "CImg.h"

using namespace cimg_library;
using namespace std;

const int dimension = 300;
const int cImgSquareSizeInPixels = 2;
const float epiVizVersionNumber = 1.0;
//const int animationSpeed = 125;
const std::string diseaseListFileName = "diseaseList.txt";
const std::string configFileName = "config.txt";

class simulation
{
private:
    int currentDay;
    int maxDay;
	
	//Population Totals
    int susceptiblePopulation;
    int infectedPopulation;
    int vaccinatedPopulation;
    int immunePopulation;
    int deadPopulation;
	
	//Simulation Options Flags
	bool cImgAnimationFlag;
	bool htmlFlag;
	bool csvFlag;
	int cImgAnimationSpeed;
    
	//2D Array of Entities
    entity grid[dimension][dimension];
    //entity **grid;
    
	//Queues
	std::list<entity> infectionQueue;
	std::list<disease> diseaseQueue;
	std::list<entity> vaccinationQueue;
	std::list<zone> zoneQueue;
	
	//Disease
	disease chosenDisease;
	
	//Private Methods
    void startSimulation();
	void initializeSim();
	
	//File Management
	void writeHtmlHeader();
	void writeHtmlTable();
	void writeHtmlFooter();
	void writeCSVUpdate();
	void createCSVFile();
	void updateDiseaseList();
	void loadDiseaseList();
	void loadConfigFile();
	void saveConfigFile();
	void createNewDisease();
	void createConfigFile();
	void animateImage(CImg<unsigned char> &x);
	
	//Menus
	void printMainMenu();
	int printDiseaseOptions();
	void determineMenuChoice(int n, int numberOfOptionsPrinted);
	void showDiseaseEditMenu();
	void showSimulationOptionsMenu();
	void askForDiseaseParameters(disease &b);
	void confirmSimulationChoice();
	int getValidInteger(std::string prompt, int inclusiveLowRange, int inclusiveHighRange);
	
	//Utility
	void worldWrap(int &row, int &col);
	void backupNcharacters(int n);
	
	//Infection
	void randomlyInfectFirstEntity();
    void spreadInfection();
	void attemptInfectionAt(int row, int col);
	void infectEntity(int row, int col);
	void vaccinateEntity(int row, int col);
	
	//Vaccination
	void placeInitialVaccinations();
	void dispatchVaccinationPods();
	void spreadVaccination();
	void attemptVaccinationAt(int row, int col);
	
	//Removed States
	void determineRemovedState(int row, int col);
	void removeEntity(int row, int col, int newStatus);
	
public:
    simulation(int maxDay);
    void begin();
	void printSimulationInfo();
	void printGridInfo();
};

#endif /* defined(__epiViz__simulation__) */
