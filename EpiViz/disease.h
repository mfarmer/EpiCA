//
//  disease.h
//  epiViz
//
//  Created by Matthew Farmer on 3/17/13.
//  Copyright (c) 2013 Matthew Farmer. All rights reserved.
//

#ifndef __epiViz__disease__
#define __epiViz__disease__

#include <iostream>
#include <string>
#include <iomanip>

class disease
{
private:
    std::string name;
    unsigned short int infectionProbability;
    unsigned short int deathProbability;
    unsigned short int travelProbability;
    unsigned short int vaccinationProbability;
    unsigned short int daysInfectionLasts;
    unsigned short int daysBeforeVaccinationAvailable;
    
public:
    disease();
    ~disease();
    void setName(std::string n);
    void setInfectionProbability(unsigned short int n);
    void setDeathProbability(unsigned short int n);
    void setTravelProbability(unsigned short int n);
    void setVaccinationProbability(unsigned short int n);
    void setDaysInfectionLasts(unsigned short int n);
    void setDaysBeforeVaccinationAvailable(unsigned short int n);
    
	std::string getName();
    unsigned short int getInfectionProbability();
    unsigned short int getDeathProbability();
    unsigned short int getTravelProbability();
    unsigned short int getVaccinationProbability();
    unsigned short int getDaysInfectionLasts();
    unsigned short int getDaysBeforeVaccinationAvailable();
    
    void printInfo();
    void setup();
};

#endif /* defined(__epiViz__disease__) */
