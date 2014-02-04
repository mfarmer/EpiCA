![Screenshot](https://raw.github.com/MFarmer/EpiViz/master/Screenshot/sample1.png)
<h1>EpiCA</h1>
======

<h3><i>What is EpiCA?</i></h3>

<b>EpiCA</b> (Epidemiological Cellular Automaton) is a C++ program which uses a cellular automaton to simulate the spread of disease through a susceptible population. The [CImg library](http://cimg.sourceforge.net/) was used to display the state of the cellular automaton at various phases of the simulation. Results from the simulation are collected and displayed in each of the following ways:
<ul>
<li>CImg Animation: Shows a live, animated view of the cellular automaton so that the behavior of the model can be more easily assessed
<li>HTML File: Contains tables to show the state of the cellular automaton for each simulated day of the epidemic in a graphical manner</li>
<li>CSV File: Contains the collected population data for use in a spreadsheet program</li>
</ul>

<h3><i>How to Use</i></h3>

<ol>
<li>Clone the project<br/>
<code>git clone git://github.com/MFarmer/EpiViz.git</code>
</li>
<li>Compile (Mac)
<code>g++ -o epiViz main.cpp disease.cpp entity.cpp simulation.cpp zone.cpp -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 -fopenmp
</code></li>
<li>Execute<br/><code>./epiViz</code></li>
</ol>

Simulation Parameters:
<ul>
<li>Disease Name: What name is the disease identified as?</li>
<li>Infection Period: How long does an infection last on average for each entity?</li>
<li>Infection Probabiliy: How likely is an entity to be infected when near an infected entity?</li>
<li>Travel Probability: How likely is an infected entity to travel?</li>
<li>Vaccination Probability: How likely is an entity to be vaccinated when the opportunity arises?</li>
<li>Death Probability: How likely is an entity to die from the disease?</li>
</ul>

<h3>Introduction</h3>
A cellular automaton was utilized in a computer program to simulate the spread of infectious diseases through a population of susceptible entities. By definition, a cellular automaton consists of a grid of cells in which each cell may be in varying states. There may be any number of dimensions used for the grid, and each cell has the ability to impact the states of surrounding cells (called a neighborhood) based upon established rules. The rules used for this project will be discussed in detail within the "Considerations" section of this document.
<br><br>
Performing a simulation allowed for the collection of data such as the average number of susceptible, infected, immune, deceased, and vaccinated entities each day. The collected data was then used to construct a line graph that showed the associated SIR model for the disease being simulated.
<br><br>
<h3>Considerations</h3>
The following considerations and rules were used in the creation of this project’s cellular automaton:
<ul>
<li>C++ was used as the programming language of choice.</li>
<li>The grid is a two-dimensional array with a dimension of 50x50 entities.</li>
<li>Each cell represents a single entity which is capable of being in various states. The cells are also capable of knowing how long they have been infected, if applicable.</li>
<li>Entities may be in the following states:</li>
<li>Susceptible: The default state of all entities; vulnerable to infection. Depending on the disease, it may be possible for an infected individual to return to the susceptible population if immunity is disallowed.</li>
<li>Infected: entity is harboring the disease and is able to infect other entities. After the infectious period has elapsed, the entity will either become immune, return to the susceptible population if immunity is not possible, or die; temporary state.</li>
<li>Immune: entity was once infected but is no longer infectious and can not die, receive vaccinations, or be infected again; a permanent state.</li>
<li>Dead: entity was once infected, but has died and is no longer to able to infect others or be infected; a permanent state.</li>
<li>Vaccinated: entity is immune to infection permanently; only susceptible entities may be vaccinated; a permanent state.</li>
<li>Contacts are established using the Moore Neighborhood. Only infected and vaccinated entities may affect the state of nearby entities.</li>
<li>Each infected or vaccinated entity has a probability of infecting each entity within the Moore neighborhood. An entity in either of the aforementioned states will have the opportunity to cause anywhere from 0 to 8 entities to change states in a given day. Therefore, the likelihood of a state change occurring is greater if more of the surrounding entities are either infected or vaccinated.</li>
<li>If entities are capable of traveling, each infected entity has an additional opportunity per day to infect a random entity anywhere in the grid. Vaccinated entities are not capable of travel.</li>
<li>When an entity becomes newly infected, the entity is not infectious until the following day. The same applies for newly vaccinated entities.</li>
<li>The grid allows for wrapping. In other words, entities on the borders of the grid may affect entities on the opposite side of the grid. In this way, the grid is less like a flat square and more like a sphere.</li>
<li>Simulation ends when either no entities are infected or two hundred days have elapsed.</li>
</ul>

When the program starts, the user is greeted with a menu that allows for choosing from a predefined list of diseases to simulate. The user may also choose to create a new disease. All diseases created by the user are saved locally for later retrieval. Each disease has the following characteristics: disease name, how long infection lasts, probability of infecting other entities, probability of killing an infected entity, probability of an infected person traveling, whether immunity is allowed, and which day of the simulation vaccines are made available to the population. In addition, the user may create a custom disease to simulate as one wishes by supplying the various characteristic values.
<br><br>
Once a disease has been chosen, the program will show the user all of the characteristics associated with the chosen disease and will ask for confirmation to begin the simulation. A graphical representation of the epidemic will then appear, which utilizes the CImg Library to do so. At the conclusion of the simulation, the user will be informed of the presence of two output files generated by the program: a CSV file with average population data for the various states by day, and an optional HTML file which provides a graphical interpretation of the states within the grid on each day (only available for sufficiently small grid sizes to keep file size manageable). The user is returned to the main menu from which another simulation may begin.
<br><br>
When a simulation first began, a single entity was randomly chosen to be the first infected. The infected entity had the opportunity to infect others, and the day concluded. Each day, the infected entities have the opportunity to infect other entities. Some diseases may have a day in which vaccinations are made available. Once the vaccination day has arrived, four random susceptible entities are chosen to be vaccinated. From each day forward, all vaccinated entities have the opportunity to vaccinate susceptible entities within the Moore Neighborhood only (vaccinated entities may not travel). Vaccinations can not cure an infected entity. Only on the first available day of vaccine availability are four random entities chosen for vaccination.
<br><br>
The grid consists of entities which are capable of knowing their current state and how long they have been infected, if applicable. Once the infection period has fully lapsed, the entity must change to one of three states: susceptible (only if immunity is not allowed), immune, or dead.
<br><br>
At the conclusion of each day, an HTML file is created which shows a graphical representation of the grid and the current state of all entities for each day of the simulation. A table of 50x50 cells is drawn and a colored block is placed within each cell that represents a state. Table 1 shows which status is represented by each color.
<br><br>
The CImg visual representation of the cellular automata provides real-time information regarding the behavior of the epidemic. The HTML tables created at the end of a simulation are mostly for show to visually see how the disease spread each day by scrolling vertically through the page, while the other output file (CSV) contains the average state population data for each day over the twenty simulations. The CSV data is used to create the SIR Graph.
<br><br>
Using a spreadsheet program such as Microsoft Excel or Apple Numbers, a CSV file can be opened to easily produce a table of values. Using the columns of data as input for a line graph, a graph may quickly be produced.
<br><br>

<h3>Final Thoughts</h3>
Using a cellular automaton to simulate the spread of diseases through an infected population proved to be an interesting and fun project. The real-time CImg visualization allows for better understanding the behavior of an epidemic. The HTML Tables provided a fun, graphical way to see exactly what’s going on during a simulation run. The population data gathered from the simulations are able to demonstrate the principles of an SIR Model by showing the epidemic curve, which was exciting to see.
<br><br>
The following are ideas for future additions to the project:
<ul>
<li>Enhance the artificial intelligence of the vaccinated entities to better combat the spread of diseases</li>
<li>Instead of having every cell in the grid contain an entity, perhaps there could be cells which represent flora, structures, water, or other objects which are not affected by infections but instead provide natural barriers which can alter the spread of a disease.</li>
<li>A latent period could be included to enable SLIR Model data collection</li>
</ul>
