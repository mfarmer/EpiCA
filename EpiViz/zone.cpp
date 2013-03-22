//
//  zone.cpp
//  epiViz
//
//  Created by Matthew Farmer on 3/17/13.
//  Copyright (c) 2013 Matthew Farmer. All rights reserved.
//

#include "zone.h"

zone::zone(double statusAverage, int xCor, int yCor)
{
	this->statusAverage = statusAverage;
	this->xCor = xCor;
	this->yCor = yCor;
}

zone::~zone()
{
	//default destructor
}

double zone::getStatusAverage()
{
	return this->statusAverage;
}

int zone::getXCor()
{
	return this->xCor;
}

int zone::getYCor()
{
	return this->yCor;
}

void zone::setStatusAverage(double statusAverage)
{
	this->statusAverage = statusAverage;
}

void zone::setXCor(int xCor)
{
	this->xCor = xCor;
}

void zone::setYCor(int yCor)
{
	this->yCor = yCor;
}