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
	
	//Flags
	this->cImgAnimationFlag = false;
	this->htmlFlag = true;
	this->csvFlag = false;
	this->cImgAnimationSpeed = 3;
	
	//Load Disease Information
	loadDiseaseList();
}

void simulation::begin()
{
	initializeGrid();
	printMainMenu();
}

void simulation::confirmSimulationChoice()
{
	int userInitiateSimulation = 0;
	
	//I need to provide an interface for the user to specify which disease should be simulated
	std::cout << "\n[!] You chose \'" << this->chosenDisease.getName() << "\'" << std::endl;
	
	this->chosenDisease.printInfo();
	
	std::cout << '\n';
	
	//Ask the user if they would like to continue with this disease choice
	userInitiateSimulation = getValidInteger("[?] Would you like to begin the simulation? (1=YES, 0=NO): ",0,1);
    
	if(userInitiateSimulation == 1)
		startSimulation();
}

void simulation::startSimulation()
{
	if(!this->cImgAnimationFlag && !this->csvFlag && !this->htmlFlag)
	{
		std::cout << "[!] You have no output modes enabled. Please enable at least one output before beginning the simulation.\n";
		showSimulationOptionsMenu();
		return;
	}
	
    //Draw the CImg image to animate
	if(this->cImgAnimationFlag)
	{
		//CImg<unsigned char> world(dimension*10,dimension*10,1,3);
		//CImgDisplay main_display(world,(this->chosenDisease.getName()+" Simulation").c_str());
	}
    
	//Choose an entity in the grid to become infected
	randomlyInfectFirstEntity();
	
	//Reset the current day counter to 0
	this->currentDay = 0;
	
	//Begin a new HTML file, and draw the first table for day 0
	if(this->htmlFlag)
	{
		writeHtmlHeader();
		writeHtmlTable();
	}
    
	//Loop through each day of the simulation, trying to spread infection each day until maxDays is reached or no one is infected anymore
	while(this->currentDay < this->maxDay && this->infectionQueue.size() > 0)
	{
		spreadInfection();
		spreadVaccination();
		this->currentDay++;
		
		//Draw your daily HTML table here, record another line in your CSV, and draw the next frame in your CImg window
		//printGridInfo();
        if(this->htmlFlag)
			writeHtmlTable();
        
		if(this->cImgAnimationFlag)
			for(int frame=0; frame<animationSpeed; frame++)
			{
				//animateImage(world);
				//world.display(main_display);
			}
	}
	if(this->htmlFlag)
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
    
    //Empty the vaccination queue
    this->vaccinationQueue.clear();
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
	if(this->currentDay >= this->chosenDisease.getDaysBeforeVaccinationAvailable())
	{
		//Are there any vaccinated entities in the grid yet?
		if(this->vaccinationQueue.size() == 0)
		{
			placeInitialVaccinations();
            //pause();
		}
		//Ok, I already have vaccinated entities in my queue. I will allow them to start vaccinating!
		else
		{
			for(std::list<entity>::iterator it=this->vaccinationQueue.begin(); it != this->vaccinationQueue.end(); ++it)
			{
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

void simulation::placeInitialVaccinations()
{	
	int maxAttempts = dimension*dimension;
    int attempts = 0;
    int placedHospitals = 0;
    int x = 0, y = 0;
    while(attempts < maxAttempts && placedHospitals < 4)
    {
        x = rand()%dimension;
        y = rand()%dimension;
        
        if(this->grid[x][y].getStatus() == susceptible)
        {
            placedHospitals++;
            this->grid[x][y].flipNewlyVaccinated();
            this->grid[x][y].setStatus(vaccinated);
            this->vaccinationQueue.push_back(this->grid[x][y]);
        }
        attempts++;
    }
    
    //Attempt finding a susceptible square nearest the optimal cell
    //this->grid[x][y].flipNewlyVaccinated();
    //this->grid[x][y].setStatus(vaccinated);
    //this->vaccinationQueue.push_back(this->grid[x][y]);
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
    int diceRoll = rand()%100;
    
    worldWrap(row,col);
    
    if(diceRoll < this->chosenDisease.getDeathProbability())
    {
        //The entity is now deceased, as opposed to immune
        this->grid[row][col].setStatus(dead);
    }
    else if(this->chosenDisease.getImmunizationAllowed())
    {
        this->grid[row][col].setStatus(immune);
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
	
	//We'll only try to vaccinate the entity if the entity is currently in the susceptible state
	if(this->grid[row][col].getStatus() == susceptible)
	{
		int diceRoll = rand()%100;
		
		//Should entity get vaccinated?
		if(diceRoll < this->chosenDisease.getVaccinationProbability())
		{
			this->grid[row][col].flipNewlyVaccinated();
            this->grid[row][col].setStatus(vaccinated);
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

void simulation::printMainMenu()
{
	int menuChoice;
	
	//Display a list of diseaes options for the user to choose from
	std::cout << "\n+-------------------EpiViz v" << epiVizVersionNumber << "-------------------+" << std::endl;
	int numberOfOptionsPrinted;

	std::cout << "\n[?] Please choose a disease to simulate below:" << std::endl;
	numberOfOptionsPrinted = printDiseaseOptions();
	std::cout << std::setw(2) << numberOfOptionsPrinted << ". *Create New Disease" << std::endl;
	std::cout << std::setw(2) << numberOfOptionsPrinted+1 << ". *Edit Disease List" << std::endl;
	std::cout << std::setw(2) << numberOfOptionsPrinted+2 << ". *Simulation Options" << std::endl << std::endl;
	menuChoice = getValidInteger("==> ",1,numberOfOptionsPrinted+2);
	
	//Figure out which disease the user chose, and return it
	//return determineMenuChoice(menuChoice, numberOfOptionsPrinted);
	determineMenuChoice(menuChoice, numberOfOptionsPrinted);
}

void simulation::determineMenuChoice(int n, int numberOfOptionsPrinted)
{
	//User wants to create a new disease
	if(n == numberOfOptionsPrinted)
		createNewDisease();
	else if(n == numberOfOptionsPrinted+1)
		showDiseaseEditMenu();
	else if(n == numberOfOptionsPrinted+2)
		showSimulationOptionsMenu();
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
		
		//confirm menu choice with user
		this->chosenDisease = (*it);
		
		confirmSimulationChoice();
	}
}

void simulation::showSimulationOptionsMenu()
{
	std::cout << "[?] Which option would you like to modify?\n\n";
	
	//Should HTML output files be produced?
	if(this->htmlFlag)
		std::cout << " 1.    HTML WRITER: [ON] OFF" << std::endl;
	else
		std::cout << " 1.    HTML WRITER:  ON [OFF]" << std::endl;
	
	//Should a CSV data file be produced?
	if(this->csvFlag)
		std::cout << " 2.     CSV WRITER: [ON] OFF" << std::endl;
	else
		std::cout << " 2.     CSV WRITER:  ON [OFF]" << std::endl;
	
	//Should a CImg animation be produced?
	if(this->cImgAnimationFlag)
	{
        std::cout << " 3. CImg Animation: [ON] OFF" << std::endl;
		switch(this->cImgAnimationSpeed)
		{
			case 1:{std::cout << " 4.          Speed: [1] 2 3 4 5" << std::endl;break;}
			case 2:{std::cout << " 4.          Speed: 1 [2] 3 4 5" << std::endl;break;}
			case 3:{std::cout << " 4.          Speed: 1 2 [3] 4 5" << std::endl;break;}
			case 4:{std::cout << " 4.          Speed: 1 2 3 [4] 5" << std::endl;break;}
			case 5:{std::cout << " 4.          Speed: 1 2 3 4 [5]" << std::endl;break;}
			default:{std::cout << " 4???.       Speed: 1 2 [3] 4 5" << std::endl;break;}
		}
        std::cout << " 5. *Back" << std::endl;
	}
    else
    {
        std::cout << " 3. CImg Animation:  ON [OFF]" << std::endl;
        std::cout << " 4. *Back" << std::endl;
    }
    
	int menuChoice = getValidInteger("==> ",1,5);
	
	switch(menuChoice)
	{
		case 1:{this->htmlFlag = !this->htmlFlag;showSimulationOptionsMenu();break;}
		case 2:{this->csvFlag = !this->csvFlag;showSimulationOptionsMenu();break;}
		case 3:{this->cImgAnimationFlag = !this->cImgAnimationFlag;showSimulationOptionsMenu();break;}
		case 4:
		{
			if(this->cImgAnimationFlag)
			{
				std::cout << "[?] What speed should the animation operate at? (1-5, 1 being fastest)\n";
				this->cImgAnimationSpeed = getValidInteger("==> ",1,5);
				showSimulationOptionsMenu();
			}
			else
				begin();
			break;
		}
		case 5:
		{
			if(this->cImgAnimationFlag)
				begin();
			break;
		}
		default:{std::cout << "[X] ERROR: Didn't recognize menu choice in the Simulation Options menu...\n";break;}
	}
}

void simulation::showDiseaseEditMenu()
{
	std::cout << "\n[?] Which disease would you like to edit?\n\n";
	int numberOfOptions = printDiseaseOptions();
	std::cout << std::setw(2) << numberOfOptions << ". *Go Back" << std::endl;
	int diseaseChoice = getValidInteger("\n==> ",1,numberOfOptions+1);
	
	//If the user wants to go back to the main menu, return.
	if(diseaseChoice == numberOfOptions)
		return;
	
	//The user chose a valid disease choice if we are at this point of the function
	std::list<disease>::iterator it = this->diseaseQueue.begin();
	int count = 1;
	while(count != diseaseChoice)
	{
		it++;
		count++;
	}
	
	std::cout << "\n[?] Which option would you like to edit?\n\n";
	std::cout << "    +----------------------------------------------+" << std::endl;
    std::cout << std::setw(2) << right << 1 << ". " << std::setw(8) << std::left << "| Name: " << std::setw(38) << std::right << (*it).getName() << " |" << std::endl;
	std::cout << std::setw(2) << 2 << ". " << std::setw(42) << std::left << "| Infection Probability: " << std::setw(3) << std::right << (*it).getInfectionProbability() << "% |" << std::endl;
    std::cout << std::setw(2) << 3 << ". " << std::setw(42) << std::left << "| Days Infection Lasts: " << std::setw(4) << std::right << (*it).getDaysInfectionLasts() << " |" << std::endl;
	std::cout << std::setw(2) << 4 << ". " << std::setw(42) << std::left << "| Death Probability: " << std::setw(3) << std::right << (*it).getDeathProbability() << "% |" << std::endl;
    std::cout << std::setw(2) << 5 << ". " << std::setw(42) << std::left << "| Travel Probability: " << std::setw(3) << std::right << (*it).getTravelProbability() << "% |" << std::endl;
    std::cout << std::setw(2) << 6 << ". " << std::setw(42) << std::left << "| Vaccination Probability: " << std::setw(3) << std::right << (*it).getVaccinationProbability() << "% |" << std::endl;
	std::cout << std::setw(2) << 7 << ". " << std::setw(42) << std::left << "| Day when Vaccination Available: " << std::setw(4) << std::right << (*it).getDaysBeforeVaccinationAvailable() << " |" << std::endl;
    std::cout << std::setw(2) << 8 << ". " << std::setw(42) << std::left << "| Immunization Allowed: " << std::setw(4) << std::right << (*it).getImmunizationAllowed() << " |" << std::endl;
	std::cout << "    +----------------------------------------------+" << std::endl;
	std::cout << std::setw(2) << 9 << ". " << std::setw(42) << std::left << " *Delete " << std::endl;
	std::cout << std::setw(2) << 10 << ". " << std::setw(42) << std::left << " *Go Back " << std::endl;
	
	int choice = getValidInteger("\n==> ",1,10);
	std::cout << '\n';
	
	//choice == 10 means the user wants to return to the disease edit menu
	switch(choice)
	{
		case 1:
		{
			std::string newName = "";
			std::cout << "[?] Enter a new name: ";
			std::cin >> newName;
			(*it).setName(newName);
			break;
		}
		case 2:
		{
			(*it).setInfectionProbability(getValidInteger("[?] Enter new infection probability: ",0,100));
			break;
		}
		case 3:
		{
			(*it).setDaysInfectionLasts(getValidInteger("[?] Enter a new value for days infection lasts: ",0,this->maxDay));
			break;
		}
		case 4:
		{
			(*it).setDeathProbability(getValidInteger("[?] Enter new death probability: ",0,100));
			break;
		}
		case 5:
		{
			(*it).setTravelProbability(getValidInteger("[?] Enter new travel probability: ",0,100));
			break;
		}
		case 6:
		{
			(*it).setVaccinationProbability(getValidInteger("[?] Enter new vaccination probability: ",0,100));
			break;
		}
		case 7:
		{
			(*it).setDaysBeforeVaccinationAvailable(getValidInteger("[?] Enter a new value for days before vaccination available: ",0,this->maxDay));
			break;
		}
		case 8:
		{
			(*it).setImmunizationAllowed(getValidInteger("[?] Enter a new value for immunization allowed: ",0,1));
			break;
		}
		case 9:
		{
			//delete
			std::cout << "[?] Are you sure you want to delete \'" << (*it).getName() << "\'? (1=YES, 0=NO)" << std::endl;
			int response = getValidInteger("\n==> ",0,1);
			if(response == 1)
			{
				it = this->diseaseQueue.erase(it);
				std::cout << "\n[!] Deletion successful!" << std::endl;
			}
			else
			{	
				std::cout << "\n[!] Cancelled deletion." << std::endl;
			}
			break;
		}
		default://shouldn't happen due to 'getValidInteger' enforcing a 1-8 selection, but this is here just to be safe
		{
			std::cout << "\n[X] ERROR: Did not recognize disease edit choice. Ignoring request...\n";
			break;
		}
	}
	std::cout << "\n[!] Successful edit!" << std::endl;
	
	//I'd like to rewrite the diseaseList.txt
	updateDiseaseList();
	
	showDiseaseEditMenu();
}

void simulation::updateDiseaseList()
{
	std::ofstream outfile;
	outfile.open(diseaseListFileName.c_str());
	for(std::list<disease>::iterator it=this->diseaseQueue.begin(); it != this->diseaseQueue.end(); ++it)
	{
		outfile << "\r\n" << (*it).getName() << "\r\n";
		outfile << (*it).getInfectionProbability() << "\r\n";
		outfile << (*it).getDeathProbability() << "\r\n";
		outfile << (*it).getTravelProbability() << "\r\n";
		outfile << (*it).getVaccinationProbability() << "\r\n";
		outfile << (*it).getDaysInfectionLasts() << "\r\n";
		outfile << (*it).getDaysBeforeVaccinationAvailable() << "\r\n";
		outfile << (*it).getImmunizationAllowed();
	}
}

void simulation::createNewDisease()
{	
	disease a;
	
	askForDiseaseParameters(a);
	
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
	
	this->chosenDisease = a;
}

void simulation::askForDiseaseParameters(disease &b)
{
	std::string name;
	unsigned short int infectionProbability, deathProbability, travelProbability, vaccinationProbability, daysBeforeVaccinationAvailable, daysInfectionLasts;
	int immunizationAllowed;
	
    std::cout << "\n+-------------CREATE NEW DISEASE-----------------+" << std::endl;
	std::cout << "Enter Disease Name: ";
	std::cin >> name;
	infectionProbability = getValidInteger("Enter Infection Probability: ",0,100);
    daysInfectionLasts = getValidInteger("Enter Days Infection Lasts: ",1,this->maxDay);
	deathProbability = getValidInteger("Enter Death Probability: ",0,100);
	travelProbability = getValidInteger("Enter Travel Probability: ",0,100);
	vaccinationProbability = getValidInteger("Enter Vaccination Probability: ",0,100);
	if(vaccinationProbability > 0)
	{
		daysBeforeVaccinationAvailable = getValidInteger("Enter Days Before Vaccination Available: ",0,this->maxDay);
	}
	else
	{
		daysBeforeVaccinationAvailable = 0;
	}
	immunizationAllowed = getValidInteger("Is immunization allowed? (1=YES, 0=NO): ",0,1);
	std::cout << "+------------------------------------------------+" << std::endl;
	
	//Set the attributes of the disease
	b.setName(name);
	b.setInfectionProbability(infectionProbability);
	b.setDeathProbability(deathProbability);
	b.setTravelProbability(travelProbability);
	b.setVaccinationProbability(vaccinationProbability);
	b.setDaysInfectionLasts(daysInfectionLasts);
	b.setDaysBeforeVaccinationAvailable(daysBeforeVaccinationAvailable);
	if(immunizationAllowed == 1)
		b.setImmunizationAllowed(true);
	else
		b.setImmunizationAllowed(false);
}

int simulation::printDiseaseOptions()
{
	int count = 1;
	for(std::list<disease>::iterator it=this->diseaseQueue.begin(); it != this->diseaseQueue.end(); ++it)
	{
		std::cout << std::setw(2) << right << count << ". " << (*it).getName() << std::endl;
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
			infile >> infectionProbability >> deathProbability >> travelProbability >> vaccinationProbability >> daysInfectionLasts >> daysBeforeVaccinationAvailable >> immunizationAllowed;
			
			//Set the variables for my new disease based upon the data I just read from my input file
			a.setName(name);
			a.setInfectionProbability(infectionProbability);
			a.setDeathProbability(deathProbability);
			a.setTravelProbability(travelProbability);
			a.setVaccinationProbability(vaccinationProbability);
            a.setDaysInfectionLasts(daysInfectionLasts);
			a.setDaysBeforeVaccinationAvailable(daysBeforeVaccinationAvailable);
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