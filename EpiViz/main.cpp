//
//  main.cpp
//  epiViz
//
//  Created by Matthew Farmer on 3/17/13.
//  Copyright (c) 2013 Matthew Farmer. All rights reserved.
//

#include <iostream>
#include "simulation.h"

int main(int argc, const char * argv[])
{
    simulation mySim(1000);

	while(1)
        mySim.begin();

    return 0;
}