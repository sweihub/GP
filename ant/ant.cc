
// ant.cc

/* ---------------------------------------------------------------

The Artificial Ant Problem

An example for how to use gpc++ - The Genetic Programming Kernel

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

Copyright 1996, 1997 Thomas Weinbrenner

For comments, improvements, additions (or even money) contact:

Thomas Weinbrenner
Grauensteinstr. 26
35789 Laimbach
Germany
E-mail: thomasw@emk.e-technik.th-darmstadt.de
WWW:    http://www.emk.e-technik.th-darmstadt/~thomasw

--------------------------------------------------------------- */

// Credits also go to Adam Fraser

#include <iostream>
#include <fstream>
#include <strstream>

#include <stdlib.h>
#include <new>    // For the new-handler

// Include header file of genetic programming system.
#include "gp.h" 
#include "gpconfig.h"

#include "ant.h" 

using namespace std;


// Factor to multiply the length with (for the fitness calculation)
double lengthFactor=1/1000.0;

// Define configuration parameters and the neccessary array to
// read/write the configuration to a file. If you need more variables,
// just add them below and insert an entry in the configArray.
GPVariables cfg;
char *InfoFileName="data";
int energy;
struct GPConfigVarInformation configArray[]=
{
  {"PopulationSize", DATAINT, &cfg.PopulationSize},
  {"NumberOfGenerations", DATAINT, &cfg.NumberOfGenerations},
  {"CrossoverProbability", DATADOUBLE, &cfg.CrossoverProbability},
  {"CreationrProbability", DATADOUBLE, &cfg.CreationProbability},
  {"CreationType", DATAINT, &cfg.CreationType},
  {"MaximumDepthForCreation", DATAINT, &cfg.MaximumDepthForCreation},
  {"MaximumDepthForCrossover", DATAINT, &cfg.MaximumDepthForCrossover},
  {"SelectionType", DATAINT, &cfg.SelectionType},
  {"TournamentSize", DATAINT, &cfg.TournamentSize},
  {"DemeticGrouping", DATAINT, &cfg.DemeticGrouping},
  {"DemeSize", DATAINT, &cfg.DemeSize},
  {"DemeticMigProbability", DATADOUBLE, &cfg.DemeticMigProbability},
  {"SwapMutationProbability", DATADOUBLE, &cfg.SwapMutationProbability},
  {"ShrinkMutationProbability", DATADOUBLE, &cfg.ShrinkMutationProbability},
  {"AddBestToNewPopulation", DATAINT, &cfg.AddBestToNewPopulation},
  {"SteadyState", DATAINT, &cfg.SteadyState},
  {"InfoFileName", DATASTRING, &InfoFileName},
  {"LengthFactor", DATADOUBLE, &lengthFactor},
  {"Energy", DATAINT, &energy},
  {"", DATAINT, NULL}
};

// The ant on the trail
Ant ant;

// Used for printing the trail to a file during evaluation
int printAnt=0;
ofstream antFile;



// We have the freedom to define this function in any way we like.
// This function evaluates the fitness of a genetic tree and is
// recursive of course.
int MyGene::evaluate ()
{
  int result, sum;
  switch (node->value ())
    {

    case LEFT: 
      // Turn left and return 0
      ant.left ();
      return 0;
      
    case RIGHT:
      // Turn right and return 0
      ant.right ();
      return 0;
      
    case FORWARD:
      // Move forward
      result=ant.forward ();

      // Print the ant?
      if (printAnt)
	antFile << ant << '\n' << '\n';

      // Return whether there was food
      return result;
      
    case IFFOODAHEAD:
      // If thereis a food in the direction you are facing evaluate
      // the first branch otherwise evolve the second branch
      if (ant.isFoodAhead ())    
	return NthMyChild(0)->evaluate ();
      else
	return NthMyChild(1)->evaluate ();
      
    case PROG2:
      // Evaluate both children
      sum=NthMyChild(0)->evaluate ();
      sum+=NthMyChild(1)->evaluate ();
      return sum;
      
    case PROG3:
      // Evaluate all three children
      sum=NthMyChild(0)->evaluate ();
      sum+=NthMyChild(1)->evaluate ();
      sum+=NthMyChild(2)->evaluate ();
      return sum;
      
    default: 
      GPExitSystem ("MyGene::evaluate", "Undefined function value");
    }
  
  // We should never reach this point
  return 0;
}



// Evaluate the fitness of a GP and save it into the class variable
// fitness.
void MyGP::evaluate ()
{
  // Prepare ant for moving
  ant.reset (energy);

  // Print the ant?
  if (printAnt)
    antFile << ant << '\n' << '\n';

  // While the ant still has energy evaluate the GP accumulating the
  // fitness.  If, however, the whole trail is eaten, we can stop the
  // evaluation to speed things up, except we want to have a printout
  // of the ant's movements.
  int rawFitness=0;
  while ((ant.energy>0) && 
	 ((rawFitness<ant.maxFood) || printAnt))
    rawFitness+=NthMyGene (0)->evaluate ();

  // Save standardized fitness in object.  Add square times the tree
  // length multiplied with a factor, to go for small trees, and (most
  // important) to save memory.
  stdFitness=(ant.maxFood-rawFitness)+length()*length()*lengthFactor;
}



// Create function and terminal set
void createNodeSet (GPAdfNodeSet& adfNs)
{
  // Reserve space for the node sets.  No ADF's, e.g. only one node
  // set.
  adfNs.reserveSpace (1);
  
  // Now define the function and terminal set for each ADF and place
  // function/terminal sets into overall ADF container
  GPNodeSet& ns=*new GPNodeSet (6);
  adfNs.put (0, ns);
  
  // Define functions/terminals and place them into the appropriate
  // sets. Terminals take two arguments, functions three (the third
  // parameter is the number of arguments the function has)
  ns.putNode (*new GPNode (LEFT, "LEFT"));
  ns.putNode (*new GPNode (RIGHT, "RIGHT"));
  ns.putNode (*new GPNode (FORWARD, "FORWARD"));
  ns.putNode (*new GPNode (IFFOODAHEAD, "IFFOODAHEAD", 2));
  ns.putNode (*new GPNode (PROG2, "PROG2", 2));
  ns.putNode (*new GPNode (PROG3, "PROG3", 3));
}



void newHandler ()
{
  cerr << "\nFatal error: Out of memory." << endl;
  exit (1);
}



int main ()
{
  // We set up a new-handler, because we might need a lot of memory,
  // and we don't know it's there.
  set_new_handler (newHandler);
  
  // Init GP system.
  GPInit (1, -1);
  
  // Read configuration file. The defaults will be overwritten by the
  // configuration file when read. If it doesn't exist, the defaults
  // will be written to the file.
  GPConfiguration config (cout, "ant.ini", configArray);

  // Open the main output file for data, trail and statistics file.
  // First set up names for data file.  Remember we should delete the
  // string we got from the stream, well, just a few wasted bytes
  ostrstream strOutFile, strStatFile, strAntFile;
  strOutFile  << InfoFileName << ".dat" << ends;
  strStatFile << InfoFileName << ".stc" << ends;
  strAntFile << InfoFileName << ".ant" << ends;
  ofstream fout (strOutFile.str());
  ofstream bout (strStatFile.str());
  
  // Print the configuration
  fout << cfg << endl;
  cout << cfg << endl;
  
  // Create the adf function/terminal set and print it out.
  GPAdfNodeSet adfNs;
  createNodeSet (adfNs);
  cout << adfNs << endl; 
  fout << adfNs << endl;  
  
  // Read the trail
  ant.readTrail ("santafe.trl");

  // Create a population with this configuration
  cout << "Creating initial population ..." << endl;
  MyPopulation* pop=new MyPopulation (cfg, adfNs);
  pop->create ();
  cout << "Ok." << endl;
  pop->createGenerationReport (1, 0, fout, bout);
  
  // This next for statement is the actual genetic programming system
  // which is in essence just repeated reproduction and crossover loop
  // through all the generations ...
  MyPopulation* newPop=NULL;
  for (int gen=1; gen<=cfg.NumberOfGenerations; gen++)
    {
      // Create a new generation from the old one by applying the
      // genetic operators
      if (!cfg.SteadyState)
	newPop=new MyPopulation (cfg, adfNs);
      pop->generate (*newPop);
      
      // Delete the old generation and make the new the old one
      if (!cfg.SteadyState)
	{
	  delete pop;
	  pop=newPop;
	}

      // Create a report of this generation and how well it is doing
      pop->createGenerationReport (0, gen, fout, bout);
    }
  
  // Print trail while evaluating the best one of the last generation
  printAnt=1;
  antFile.open (strAntFile.str());
  pop->NthMyGP(pop->bestOfPopulation)->evaluate();
  antFile.close ();

  cout << "\nResults are in " 
       << InfoFileName << ".dat, " 
       << InfoFileName << ".ant, " 
       << InfoFileName << ".stc." << endl;
  
  return 0;
}
