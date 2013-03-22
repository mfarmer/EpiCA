//
//  simulation.cpp
//  epiViz
//
//  Created by Matthew Farmer on 3/17/13.
//  Copyright (c) 2013 Matthew Farmer. All rights reserved.
//

#include "simulation.h"

simulation::simulation(int maxDay)
{
    //Initialize the row and column value for each entity in the grid so that they may know their location
    initializeGrid();
    
	//Initialize Simulation Information
    this->currentDay = 0;
    this->maxDay = maxDay;
    this->susceptiblePopulation = dimension*dimension;
    this->infectedPopulation = 0;
    this->vaccinatedPopulation = 0;
    this->immunePopulation = 0;
    this->deadPopulation = 0;
	
	//Load Disease Information
	loadDiseaseList();
}

void simulation::begin()
{
	int userInitiateSimulation = 0;
	initializeGrid();
	
	//I need to provide an interface for the user to specify which disease should be simulated
	do
	{
		//Show the menu, and allow the user to choose a disease. The chosen disease is returned here.
		this->chosenDisease = printMainMenu();
		std::cout << "\n[!] You chose \'" << this->chosenDisease.getName() << "\'" << std::endl;
		chosenDisease.printInfo();
		std::cout << '\n';
		//Ask the user if they would like to continue with this disease choice
		userInitiateSimulation = getValidInteger("[?] Would you like to begin the simulation? (1=YES, 0=NO): ",0,1);
	}
	while(userInitiateSimulation == 0);//keep showing the menu until the user decides to begin a simulation for a chosen disease
    
    startSimulation();
}

void simulation::startSimulation()
{
    //Draw the CImg image to animate
    //CImg<unsigned char> world(dimension*10,dimension*10,1,3);
    //CImgDisplay main_display(world,"Cellular Automaton");
    
	//Choose an entity in the grid to become infected
	randomlyInfectFirstEntity();
	
	this->currentDay = 0;
	
	writeHtmlHeader();
    writeHtmlTable();
    
	//Loop through each day of the simulation, trying to spread infection each day until maxDays is reached or no one is infected anymore
	while(this->currentDay < this->maxDay && this->infectionQueue.size() > 0)
	{
		spreadInfection();
		//spreadVaccination();
		this->currentDay++;
		
		//Draw your daily HTML table here, record another line in your CSV, and draw the next frame in your CImg window
		//printGridInfo();
        writeHtmlTable();
        //animateImage(world);
        //world.display(main_display);
	}
    writeHtmlFooter();
}

void simulation::initializeGrid()
{
    for(int i=0; i<dimension; i++)
	{
        for(int j=0; j<dimension; j++)
		{
            this->grid[i][j].setGridRowCol(i,j);
			this->grid[i][j].setDaysInfected(0);
			this->grid[i][j].setStatus(susceptible);
			this->grid[i][j].setNewlyInfected(false);
			this->grid[i][j].setNewlySusceptible(false);
			this->grid[i][j].setNewlyVaccinated(false);
		}
	}
}

void simulation::randomlyInfectFirstEntity()
{
    int row = rand()%dimension;
    int col = rand()%dimension;
    
    //Choose entity to infect
    this->grid[row][col].setStatus(infected);
    
    //Place entity into FIFO queue
    this->infectionQueue.push_back(this->grid[row][col]);
}

void simulation::spreadVaccination()
{
	//Are we allowed to vaccinate
	if(this->chosenDisease.getDaysBeforeVaccinationAvailable() >= this->currentDay)
	{
		//Are there any vaccinated entities in the grid yet?
		if(this->vaccinationQueue.size() == 0)
		{
			//Try to vaccinate four zones which need the vaccinations the most!
			/*if(dispatchVaccinationPods())
             {
             //I successfully dispatched at least one pod
             }
             else
             {
             //There was not enough room to dispatch a pod, so I shouldn't ever try again
             //DO SOMETHING TO PREVENT TRYING TO SPREAD VACCINATIONS AGAIN
             }
             */
		}
		//Ok, I already have vaccinated entities in my queue. I will allow them to start vaccinating!
		else
		{
			for(std::list<entity>::iterator it=this->vaccinationQueue.begin(); it != this->vaccinationQueue.end(); ++it)
			{
				// Get a copy of the first vaccinated entity at the front of the queue
				
				//if the entity is newly vaccinated, I don't want to allow this person to vaccinate anyone yet. I'll flip their status, though, for next time
				if((*it).isNewlyVaccinated())
				{
					(*it).flipNewlyVaccinated();
				}
				else
				{
					//start attempting to vaccinate
					attemptVaccinationAt((*it).getRow()-1,(*it).getCol());//top
					attemptVaccinationAt((*it).getRow()-1,(*it).getCol()+1);//top right
					attemptVaccinationAt((*it).getRow(),(*it).getCol()+1);//right
					attemptVaccinationAt((*it).getRow()+1,(*it).getCol()+1);//bottom right
					attemptVaccinationAt((*it).getRow()+1,(*it).getCol());//bottom
					attemptVaccinationAt((*it).getRow()+1,(*it).getCol()-1);//bottom left
					attemptVaccinationAt((*it).getRow(),(*it).getCol()-1);//left
					attemptVaccinationAt((*it).getRow()-1,(*it).getCol()-1);//top left
				}
			}
		}
	}
}

void simulation::spreadInfection()
{
	//Loop through all entities in the infection queue
    for(std::list<entity>::iterator it=this->infectionQueue.begin(); it != this->infectionQueue.end(); ++it)
    {
        //Get a copy of the first infection in the front
        
        //Is this entity newly infected? If so, I will NOT attempt infecting others and will instead set the remaining entities in this list as NOT newly infected
        if((*it).isNewlyInfected())
        {
			//Set this entity to be NOT newly infected so that the next day this entity will be able to attempt infecting others
            (*it).flipNewlyInfected();
        }
		//Should this infected entity enter the removed state?
        else if((*it).getDaysInfected() > this->chosenDisease.getDaysInfectionLasts())
		{
			//This infected entity should enter the removed state.
			determineRemovedState((*it).getRow(),(*it).getCol());
			
			//Remove this entity from the queue
			it = this->infectionQueue.erase(it);
		}
		//Entity is ready to attempt infecting other entities!
		else
        {
            //start attempting to infect
			attemptInfectionAt((*it).getRow()-1,(*it).getCol());//top
			attemptInfectionAt((*it).getRow()-1,(*it).getCol()+1);//top right
			attemptInfectionAt((*it).getRow(),(*it).getCol()+1);//right
			attemptInfectionAt((*it).getRow()+1,(*it).getCol()+1);//bottom right
			attemptInfectionAt((*it).getRow()+1,(*it).getCol());//bottom
			attemptInfectionAt((*it).getRow()+1,(*it).getCol()-1);//bottom left
			attemptInfectionAt((*it).getRow(),(*it).getCol()-1);//left
			attemptInfectionAt((*it).getRow()-1,(*it).getCol()-1);//top left
			
			//TRAVEL???
			int diceRoll = rand()%100;
			if(diceRoll < this->chosenDisease.getTravelProbability())
				attemptInfectionAt(rand()%dimension,rand()%dimension);//anywhere
			
			//Increment this entities days infected since it is an actively infecting entity
			(*it).incrementDaysInfected();
        }
    }
}

void simulation::determineRemovedState(int row, int col)
{
	if(this->chosenDisease.getImmunizationAllowed())
	{
		int diceRoll = rand()%100;
		
		worldWrap(row,col);
		
		if(diceRoll < this->chosenDisease.getDeathProbability())
		{
			//The entity is now deceased, as opposed to immune
			this->grid[row][col].setStatus(dead);
		}
		else
		{
			this->grid[row][col].setStatus(immune);
		}
	}
	else
	{
		this->grid[row][col].setStatus(susceptible);
	}
}

void simulation::attemptVaccinationAt(int row, int col)
{
	//If the row or column is out of bounds, then I need to wrap it
	worldWrap(row,col);
	
	//We'll only try to infect the entity if the entity is currently in the susceptible state
	if(this->grid[row][col].getStatus() == susceptible)
	{
		int diceRoll = rand()%100;
		
		//Should entity get vaccinated?
		if(diceRoll < this->chosenDisease.getVaccinationProbability())
		{
			this->grid[row][col].flipNewlyVaccinated();
			this->vaccinationQueue.push_back(this->grid[row][col]);
		}
	}
}

void simulation::attemptInfectionAt(int row, int col)
{
	//If the row or column is out of bounds, then I need to wrap it
	worldWrap(row,col);
	
	//We'll only try to infect the entity if the entity is currently in the susceptible state
	if(this->grid[row][col].getStatus() == susceptible)
	{
		int diceRoll = rand()%100;
		
		//Should entity get infected?
		if(diceRoll < this->chosenDisease.getInfectionProbability())
		{
			this->grid[row][col].flipNewlyInfected();
			this->grid[row][col].setStatus(infected);
			this->infectionQueue.push_back(this->grid[row][col]);
		}
	}
}

void simulation::worldWrap(int &row, int &col)
{
	if(row < 0)
		row = dimension - 1;
	else if(row >= dimension)
		row = 0;
	
	if(col < 0)
		col = dimension - 1;
	else if(col >= dimension)
		col = 0;
}

void simulation::printSimulationInfo()
{
    std::cout << std::left << "\n+----------------------------------------------+" << std::endl;
    std::cout << "| " << std::setw(40) << "Simulation Data" << std::setw(4) << " " << " |" << std::endl;
    std::cout << std::setw(42) << "| Current Day: " << std::setw(4) << std::right << this->currentDay << " |" << std::endl;
    std::cout << std::setw(42) << std::left << "| Max Day: " << std::setw(4) << std::right << this->maxDay << " |" << std::endl;
    std::cout << std::setw(42) << std::left << "| Susceptible Population: " << std::setw(4) << std::right << this->susceptiblePopulation << " |" << std::endl;
    std::cout << std::setw(42) << std::left << "| Infected Population: " << std::setw(4) << std::right << this->infectedPopulation << " |" << std::endl;
    std::cout << std::setw(42) << std::left << "| Vaccinated Population: " << std::setw(4) << std::right << this->vaccinatedPopulation << " |" << std::endl;
    std::cout << std::setw(42) << std::left << "| Immune Population: " << std::setw(4) << std::right << this->immunePopulation << " |" << std::endl;
    std::cout << std::setw(42) << std::left << "| Deceased Population: " << std::setw(4) << std::right << this->deadPopulation << " |" << std::endl;
    std::cout << "+----------------------------------------------+" << std::endl;
}

void simulation::printGridInfo()
{
	std::cout << std::endl;
	for(int i=0; i<dimension; i++)
	{
		for(int j=0; j<dimension; j++)
			std::cout << this->grid[i][j].getStatus();
		std::cout << std::endl;
	}
}

disease simulation::printMainMenu()
{
	int menuChoice;
	
	//Display a list of diseaes options for the user to choose from
	std::cout << "\n+-------------------EpiViz v" << epiVizVersionNumber << "-------------------+" << std::endl;
	int numberOfOptionsPrinted;

	std::cout << "\n[?] Please choose a disease to simulate below:" << std::endl;
	numberOfOptionsPrinted = printDiseaseOptions();
	std::cout << numberOfOptionsPrinted << ". Create New Disease" << std::endl << std::endl;
	menuChoice = getValidInteger("==> ",1,numberOfOptionsPrinted);
	
	//Figure out which disease the user chose, and return it
	return determineMenuChoice(menuChoice, numberOfOptionsPrinted);
}

disease simulation::determineMenuChoice(int n, int numberOfOptionsPrinted)
{
	//User wants to create a new disease
	if(n == numberOfOptionsPrinted)
		return createNewDisease();
	else
	{
		//Look through the disease queue until the chosen disease is found
		std::list<disease>::iterator it = this->diseaseQueue.begin();
		int count = 1;
		while(count != n)
		{
			it++;
			count++;
		}
		
		//at this point, I have found my disease. I will reutrn the diseaes my queue iterator 'it' is pointing at
		return (*it);
	}
}

disease simulation::createNewDisease()
{
	disease a;
	std::string name;
	unsigned short int infectionProbability, deathProbability, travelProbability, vaccinationProbability, daysBeforeVaccinationAvailable, daysInfectionLasts;
	int immunizationAllowed;
	
    std::cout << "\n+-------------CREATE NEW DISEASE-----------------+" << std::endl;
	std::cout << "Enter Disease Name: ";
	std::cin >> name;
	infectionProbability = getValidInteger("Enter Infection Probability: ",0,100);
	deathProbability = getValidInteger("Enter Death Probability: ",0,100);
	travelProbability = getValidInteger("Enter Travel Probability: ",0,100);
	vaccinationProbability = getValidInteger("Enter Vaccination Probability: ",0,100);
	if(vaccinationProbability > 0)
	{
		daysInfectionLasts = getValidInteger("Enter Days Infection Lasts: ",1,this->maxDay);
		daysBeforeVaccinationAvailable = getValidInteger("Enter Days Before Vaccination Available: ",0,this->maxDay);
	}
	else
	{
		daysInfectionLasts = 0;
		daysBeforeVaccinationAvailable = 0;
	}
	immunizationAllowed = getValidInteger("Is immunization allowed? (1=YES, 0=NO): ",0,1);
	std::cout << "+------------------------------------------------+" << std::endl;
	
	//Set the attributes of the disease
	a.setName(name);
	a.setInfectionProbability(infectionProbability);
	a.setDeathProbability(deathProbability);
	a.setTravelProbability(travelProbability);
	a.setVaccinationProbability(vaccinationProbability);
	a.setDaysInfectionLasts(daysInfectionLasts);
	a.setDaysBeforeVaccinationAvailable(daysBeforeVaccinationAvailable);
	if(immunizationAllowed == 1)
		a.setImmunizationAllowed(true);
	else
		a.setImmunizationAllowed(false);
	
	//Save this disease in the disease list file
	std::ofstream outfile;
	outfile.open(diseaseListFileName.c_str(), std::ios::app);
	outfile << "\r\n" << a.getName() << "\r\n";
	outfile << a.getInfectionProbability() << "\r\n";
	outfile << a.getDeathProbability() << "\r\n";
	outfile << a.getTravelProbability() << "\r\n";
	outfile << a.getVaccinationProbability() << "\r\n";
	outfile << a.getDaysInfectionLasts() << "\r\n";
	outfile << a.getDaysBeforeVaccinationAvailable() << "\r\n";
	outfile << a.getImmunizationAllowed();
	
	//Put this disease onto the queue so that it will appear in the main menu
	this->diseaseQueue.push_back(a);
	
	//return to the 'simulation::begin' function so that the simulation knows what disease we are working with.
	return a;
}

int simulation::printDiseaseOptions()
{
	int count = 1;
	for(std::list<disease>::iterator it=this->diseaseQueue.begin(); it != this->diseaseQueue.end(); ++it)
	{
		std::cout << count << ". " << (*it).getName() << std::endl;
		count++;
	}
	return count;
}

void simulation::loadDiseaseList()
{
	std::ifstream infile;
	infile.open(diseaseListFileName.c_str(),std::ifstream::in);
	
	if(infile.is_open())
	{
		std::cout << "[!] Successfully opened " << diseaseListFileName << "!" << std::endl;
		
		disease a;
		std::string name;
		unsigned short int infectionProbability, deathProbability, travelProbability, vaccinationProbability, daysBeforeVaccinationAvailable, daysInfectionLasts;
		int immunizationAllowed;
		
		//Read the appropriate data from the input file
		while(infile >> name)
		{
			infile >> infectionProbability >> deathProbability >> travelProbability >> vaccinationProbability >> daysBeforeVaccinationAvailable >> daysInfectionLasts >> immunizationAllowed;
			
			//Set the variables for my new disease based upon the data I just read from my input file
			a.setName(name);
			a.setInfectionProbability(infectionProbability);
			a.setDeathProbability(deathProbability);
			a.setTravelProbability(travelProbability);
			a.setVaccinationProbability(vaccinationProbability);
			a.setDaysBeforeVaccinationAvailable(daysBeforeVaccinationAvailable);
			a.setDaysInfectionLasts(daysInfectionLasts);
			a.setImmunizationAllowed(immunizationAllowed);
			
			//Add this disease to the end of my diseaseQueue
			this->diseaseQueue.push_back(a);
			std::cout << "[!] Loaded disease \'" << (this->diseaseQueue.back()).getName() << "\'" << std::endl;
		}
		
		infile.close();
	}
	else
		std::cout << "[X] ERROR: Could not open file name \'" << diseaseListFileName << "\'" << std::endl;
}

void simulation::writeHtmlHeader()
{
    std::ofstream outfile;
    outfile.open((this->chosenDisease.getName()+"_simulation.html").c_str());
    
    if(outfile.is_open())
    {
        outfile << "<html><header><title>" << this->chosenDisease.getName() << "</title></header><body>";
        outfile.close();
    }
    else
        std::cout << "[X] ERROR: Could not create file \'" << this->chosenDisease.getName() << "_simulation.html\'\n";
}

void simulation::writeHtmlTable()
{
    std::ofstream outfile;
    outfile.open((this->chosenDisease.getName()+"_simulation.html").c_str(),std::ios::app);
    
    if(outfile.is_open())
    {
        outfile << "<h1 style='text-align:center'>Day " << this->currentDay << "</h1>" << std::endl;
        outfile << "<table cellspacing='0' cellpadding='0' style='border-collapse:collapse;margin-left:auto;margin-right:auto'>";
        for(int i=0; i<dimension; i++)
        {
            outfile << "<tr style='padding: 0px; margin: 0px; border: none;'>";
            for(int j=0; j<dimension; j++)
            {
                outfile << "<td ";
                if(this->grid[i][j].getStatus() == susceptible)
                    outfile << "bgcolor=#FFFF00 width='10' height='10'></td>";
                else if(this->grid[i][j].getStatus() == infected)
                    outfile << "bgcolor=#008000 width='10' height='10'></td>";
                else if(this->grid[i][j].getStatus() == immune)
                    outfile << "bgcolor=#0000FF width='10' height='10'></td>";
                else if(this->grid[i][j].getStatus() == dead)
                    outfile << "bgcolor=#000000 width='10' height='10'></td>";
                else if(this->grid[i][j].getStatus() == vaccinated)
                    outfile << "bgcolor=#FF0000 width='10' height='10'></td>";
                outfile << "</td>";
            }
            outfile << "</tr>";
        }
        outfile << "</table>";
        outfile.close();
    }
    else
        std::cout << "[X] ERROR: Could not append table to file \'" << this->chosenDisease.getName() << "_simulation.html\'\n";
}

void simulation::writeHtmlFooter()
{
    std::ofstream outfile;
    outfile.open((this->chosenDisease.getName()+"_simulation.html").c_str(),std::ios::app);
    
    if(outfile.is_open())
    {
        outfile << "</body></html>";
        outfile.close();
    }
    else
        std::cout << "[X] ERROR: Could not append footer to file \'" << this->chosenDisease.getName() << "_simulation.html\'\n";
}

void simulation::animateImage(CImg<unsigned char> &x)
{
    for(int i=0; i<dimension; i++)
    {
        for(int j=0; j<dimension; j++)
        {
            if(this->grid[i][j].getStatus() == susceptible)
            {
                unsigned char color[] = {255,255,0};//yellow
                x.draw_rectangle(i*10,j*10,(i*10)+10,(j*10)+10,color,1);
            }
            else if(this->grid[i][j].getStatus() == vaccinated)
            {
                unsigned char color[] = {255,0,0};//red
                x.draw_rectangle(i*10,j*10,(i*10)+10,(j*10)+10,color,1);
            }
            else if(this->grid[i][j].getStatus() == infected)
            {
                unsigned char color[] = {0,255,0};//green
                x.draw_rectangle(i*10,j*10,(i*10)+10,(j*10)+10,color,1);
            }
            else if(this->grid[i][j].getStatus() == immune)
            {
                unsigned char color[] = {0,0,255};//blue
                x.draw_rectangle(i*10,j*10,(i*10)+10,(j*10)+10,color,1);
            }
            else if(this->grid[i][j].getStatus() == dead)
            {
                unsigned char color[] = {0,0,0};//black
                x.draw_rectangle(i*10,j*10,(i*10)+10,(j*10)+10,color,1);
            }
        }
    }
}

int simulation::getValidInteger(std::string prompt, int inclusiveLowRange, int inclusiveHighRange)
{
	std::string userInput = "";
	char * p;
	int value;
	
	//Get the string input
	do
	{
		do
		{
			std::cout << prompt;
			cin >> userInput;
		
			//Check the characters for any non integer value
			strtol(userInput.c_str(),&p,10);
		}
		//*p should = 0 if there were no bad characters in the string
		while(*p != 0);
		
		value = atoi(userInput.c_str());
	}
	while(value < inclusiveLowRange || value > inclusiveHighRange);
	
	value = atoi(userInput.c_str());
	
	return value;
}