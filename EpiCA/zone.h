//
//  zone.h
//  epiViz
//
//  Created by Matthew Farmer on 3/22/13.
//  Copyright (c) 2013 Matthew Farmer. All rights reserved.
//

#ifndef __epiViz__zone__
#define __epiViz__zone__

class zone
{
private:
	double statusAverage;
	int xCor;
	int yCor;
	
public:
	zone(double statusAverage, int xCor, int yCor);
	~zone();
	
	double getStatusAverage();
	int getXCor();
	int getYCor();
	
	void setStatusAverage(double statusAverage);
	void setXCor(int xCor);
	void setYCor(int yCor);
};

#endif /* defined(__epiViz__zone__) */