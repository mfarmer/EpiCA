//
//  entity.h
//  epiViz
//
//  Created by Matthew Farmer on 3/17/13.
//  Copyright (c) 2013 Matthew Farmer. All rights reserved.
//

#ifndef __epiViz__entity__
#define __epiViz__entity__

#include <iostream>

const int susceptible = 0;
const int infected = 1;
const int immune = 2;
const int dead = 3;
const int vaccinated = 4;

class entity
{
private:
    int gridRow;
    int gridCol;
    int status;
    int daysInfected;
    bool newlyInfected;
    bool newlySusceptible;
	bool newlyVaccinated;
    
public:
    entity();
    ~entity();
    void setStatus(int status);
    void setGridRowCol(int row, int col);
	void setDaysInfected(int daysInfected);
	void setNewlyInfected(bool newlyInfected);
	void setNewlySusceptible(bool newlySusceptible);
	void setNewlyVaccinated(bool newlyVaccinated);
	void incrementDaysInfected();
	int getDaysInfected();
    int getStatus();
	int getRow();
	int getCol();
    bool isNewlyInfected();
    bool isNewlySusceptible();
	bool isNewlyVaccinated();
    void flipNewlyInfected();
    void flipNewlySusceptible();
	void flipNewlyVaccinated();
};

#endif /* defined(__epiViz__entity__) */
