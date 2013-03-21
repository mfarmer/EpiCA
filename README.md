<h1>EpiViz</h1>
======

<h3><i>What is EpiViz?</i></h3>

<b>EpiViz</b> is C++ program which uses a cellular automaton to simulate the spread of disease through a susceptible population. The [CImg library](http://cimg.sourceforge.net/) was used to display the state of the cellular automaton at various phases of the simulation. Results from the simulation are collected and displayed in each of the following ways:
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
<li>Compile
<code>g++ -o epiViz main.cpp disease.cpp entity.cpp simulation.cpp -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 -fopenmp
</li></code>

Simulation Parameters:
<ul>
<li>Disease Name: What name is the disease identified as?</li>
<li>Infection Period: How long does an infection last on average for each entity?</li>
<li>Infection Probabiliy: How likely is an entity to be infected when near an infected entity?</li>
<li>Travel Probability: How likely is an infected entity to travel?</li>
<li>Vaccination Probability: How likely is an entity to be vaccinated when the opportunity arises?</li>
<li>Death Probability: How likely is an entity to die from the disease?</li>
</ul>

to be continued
