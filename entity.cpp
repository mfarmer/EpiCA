//
//  entity.cpp
//  epiViz
//
//  Created by Matthew Farmer on 3/17/13.
//  Copyright (c) 2013 Matthew Farmer. All rights reserved.
//

#include "entity.h"

entity::entity()
{
    this->status = susceptible;
    this->newlyInfected = false;
    this->newlySusceptible = false;
	this->newlyVaccinated = false;
}

entity::~entity()
{
    //destructor
}

void entity::setStatus(int status)
{
    this->status = status;
}

void entity::setGridRowCol(int row, int col)
{
    this->gridRow = row;
    this->gridCol = col;
}

void entity::setDaysInfected(int daysInfected)
{
	this->daysInfected = daysInfected;
}

void entity::setNewlyInfected(bool newlyInfected)
{
	this->newlyInfected = newlyInfected;
}

void entity::setNewlySusceptible(bool newlySusceptible)
{
	this->newlySusceptible = newlySusceptible;
}

void entity::setNewlyVaccinated(bool newlyVaccinated)
{
	this->newlyVaccinated = newlyVaccinated;
}

void entity::incrementDaysInfected()
{
	this->daysInfected+=1;
}

int entity::getStatus()
{
    return this->status;
}

int entity::getRow()
{
	return this->gridRow;
}

int entity::getCol()
{
	return this->gridCol;
}

int entity::getDaysInfected()
{
	return this->daysInfected;
}

bool entity::isNewlyInfected()
{
    return this->newlyInfected;
}

bool entity::isNewlySusceptible()
{
    return this->newlySusceptible;
}

bool entity::isNewlyVaccinated()
{
	return this->newlyVaccinated;
}

void entity::flipNewlyInfected()
{
    this->newlyInfected = !this->newlyInfected;
}

void entity::flipNewlySusceptible()
{
    this->newlySusceptible = !this->newlySusceptible;
}

void entity::flipNewlyVaccinated()
{
	this->newlyVaccinated = !this->newlyVaccinated;
}