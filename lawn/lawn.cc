
// lawn.cc

/* ---------------------------------------------------------------

The Lawnmower Problem

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

/*

The definitions of code to map the LawnMower problem developed by John
Koza into gpc++.

Imagine, if you will, a lawn upon which we have placed a robot who
wishes to evolve the ability to mow the lawn.  The lawn is a grid
toriodal world of user defined size (see lawn.h).  Using standard GP
the robot would have to a lot of moves but with new improved ADF GP
its a different story ... and ADF GP washes whiter and is lighter than
ordinary chocolate

The approach I use to solve the problem is a little bit modified:

1. Koza uses an energy variable for whatever reason.  With every move,
the mower decreases the energy, and if nothing is left, it stops
moving.  I got bad results if the amount of energy the mower has is
too low.  I assume Koza used it to reduce calculation power (poor guy
to have to work with a LISP-machine.  He should have had the GP
kernel!).

2. Instead, I use a different fitness calculation, and a very good one
also.  It has this formula:

  area=LawnHorizontal*LawnVertical

  tmp=(area-mower.mown)/area

  stdFitness=tmp + length()*lengthFactor;

area is the area of the lawn.  tmp is a temporary variable with range
[0..1] and can by used as a fitness measure.  To reduce the length of
the GP's, a different measure is used: stdFitness is the standardized
fitness and has a range between 0 and usually 2 (depending on the
factor the length has).  The calculation time gets better and better
as the GP trees get smaller.  The lengthFactor determines whether its
better to decrease tree length or to mow all the grass.

*/

#include <iostream>
#include <fstream>
#include <strstream>

#include <stdlib.h>
#include <new>    // For the new-handler

// Include header file of genetic programming system.
#include "gp.h" 
#include "gpconfig.h"

#include "lawn.h" 

using namespace std;

// Factor to multiply the length with (for the fitness calculation)
double lengthFactor=1/100.0;

// Define configuration parameters and the neccessary array to
// read/write the configuration to a file. If you need more variables,
// just add them below and insert an entry in the configArray.
GPVariables cfg;
char *InfoFileName="data";
struct GPConfigVarInformation configArray[]=
{
  {"PopulationSize", DATAINT, &cfg.PopulationSize},
  {"NumberOfGenerations", DATAINT, &cfg.NumberOfGenerations},
  {"CreationType", DATAINT, &cfg.CreationType},
  {"CrossoverProbability", DATADOUBLE, &cfg.CrossoverProbability},
  {"CreationProbability", DATADOUBLE, &cfg.CreationProbability},
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
  {"InfoFileName", DATASTRING, &InfoFileName},
  {"SteadyState", DATAINT, &cfg.SteadyState},
  {"LengthFactor", DATADOUBLE, &lengthFactor},
  {"", DATAINT, NULL}
};

// The mower on the lawn
Mower mower;

// A zero vector
Vector zeroVector (0, 0);

// Used for printing the lawn to a file during evaluation
int printLawn=0;
ofstream lawnFile;



// We overwrite the printOn function to print a random vector not as
// "RV" but rather the numbers.
void MyGene::printOn (ostream& os)
{
  if (node->value()==RV)
    // Print out random vector
    os << randomConstant;
  else
    // Use the normal way to print
    GPGene::printOn (os);
}



// We have the freedom to define this function in any way we like.
// Variable gp is used to be able to evaluate the ADF trees of the GP.
// arg0 is the argument for ADF1.  It's recursive of course.
Vector MyGene::evaluate (MyGP& gp, Vector arg0=zeroVector)
{
  // Print the lawn?
//     {
//       static int print=1;
//       if (print)
// 	lawnFile << mower << '\n' << '\n';
//       print=node->value ()==MOW || node->value ()==FROG;
//     }
//   else

  // Stop if all is mown (Speeds things up), but only, if the
  // printmode is not active.
  if (!printLawn && mower.mown==LawnHorizontal*LawnVertical)
    return zeroVector;

  Vector arg;
  switch (node->value ())
    {

    case LEFT: 
      // Turn left and return (0,0) according to Koza
      mower.left ();
      return zeroVector;
      
    case MOW:
      // Mow and return (0,0) according to Koza
      mower.mow ();
      // Print the lawn after the mow.
      if (printLawn)
	lawnFile << mower << '\n' << '\n';
      return zeroVector;
      
    case RV:
      // Random position. If a value has not yet been assigned, do it
      // now
      if (!valueAssigned)
	{
	  valueAssigned=1;
	  randomConstant.x=GPrand () % LawnHorizontal;
	  randomConstant.y=GPrand () % LawnVertical;
	}
      return randomConstant;
      
    case ADF0:
      // No argument for ADF0
      return gp.NthMyGene (1)->evaluate (gp);
      
    case ADF1:
      // One argument for ADF1
      return gp.NthMyGene(2)->
	evaluate (gp, NthMyChild(0)->evaluate (gp));
      
    case ARG0:
      // Argument for ADF1
      return arg0;
      
    case FROG:
      // Jump to new position and mow
      arg=NthMyChild(0)->evaluate (gp);
      mower.frog (arg);
      // Print the lawn after the jump.
      if (printLawn)
	lawnFile << mower << '\n' << '\n';
      // Identity operator: returns the same
      return arg;
      
    case PROGN:
      // Evaluate first child
      NthMyChild(0)->evaluate (gp);
      
      // Return evaluation from second child
      return NthMyChild(1)->evaluate (gp);
      
    case VA:
      // Vector addition
      return NthMyChild(0)->evaluate (gp)
	+ NthMyChild(1)->evaluate (gp);

    default: 
      GPExitSystem ("MyGene::evaluate", "Undefined function value");
    }
  
  // We should never reach this point
  return zeroVector;
}



// Evaluate the fitness of a GP and save it into the class variable
// fitness.
void MyGP::evaluate ()
{
  // Prepare lawn for mowing
  mower.reset ();
  
  // Print the unmown lawn
  if (printLawn)
    lawnFile << mower << '\n' << '\n';

  // Evaluate main tree
  NthMyGene (0)->evaluate (*this);
  
  // Save standardized fitness in object
  double fitness=(double)(LawnHorizontal*LawnVertical-mower.mown)
    / (double)(LawnHorizontal*LawnVertical);
  stdFitness=fitness + (double)length()*lengthFactor;

  // Percentage mown by current GP
  mown=(1.0-fitness)*100.0;
}



// Create function and terminal set
void createNodeSet (GPAdfNodeSet& adfNs)
{
  // Reserve space for the node sets
  adfNs.reserveSpace (3);
  
  // Now define the function and terminal set for each ADF and place
  // function/terminal sets into overall ADF container
  GPNodeSet& ns0=*new GPNodeSet (8);
  GPNodeSet& ns1=*new GPNodeSet (5);
  GPNodeSet& ns2=*new GPNodeSet (8);
  adfNs.put (0, ns0);
  adfNs.put (1, ns1);
  adfNs.put (2, ns2);
  
  // Define functions/terminals and place them into the appropriate
  // sets. Terminals take two arguments, functions three (the third
  // parameter is the number of arguments the function has)
  ns0.putNode (*new GPNode (LEFT, "LEFT"));
  ns0.putNode (*new GPNode (MOW, "MOW"));
  ns0.putNode (*new GPNode (RV, "RV"));
  ns0.putNode (*new GPNode (ADF0, "ADF0"));
  ns0.putNode (*new GPNode (ADF1, "ADF1", 1));
  ns0.putNode (*new GPNode (FROG, "FROG", 1));
  ns0.putNode (*new GPNode (VA, "VA", 2));
  ns0.putNode (*new GPNode (PROGN, "PROGN", 2));
  
  ns1.putNode (*new GPNode (LEFT, "LEFT"));
  ns1.putNode (*new GPNode (MOW, "MOW"));
  ns1.putNode (*new GPNode (RV, "RV"));
  ns1.putNode (*new GPNode (VA, "VA", 2));
  ns1.putNode (*new GPNode (PROGN, "PROGN", 2));
  
  ns2.putNode (*new GPNode (ARG0, "ARG0"));
  ns2.putNode (*new GPNode (LEFT, "LEFT"));
  ns2.putNode (*new GPNode (MOW, "MOW"));
  ns2.putNode (*new GPNode (RV, "RV"));
  ns2.putNode (*new GPNode (ADF0, "ADF0"));
  ns2.putNode (*new GPNode (FROG, "FROG", 1));
  ns2.putNode (*new GPNode (VA, "VA", 2));
  ns2.putNode (*new GPNode (PROGN, "PROGN", 2));
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
  GPConfiguration config (cout, "lawn.ini", configArray);
  
  // Open the main output file for data, lawn and statistics file.
  // First set up names for data file.  Remember we should delete the
  // string from the stream, well just a few bytes
  ostrstream strOutFile, strStatFile, strLawnFile;
  strOutFile  << InfoFileName << ".dat" << ends;
  strStatFile << InfoFileName << ".stc" << ends;
  strLawnFile << InfoFileName << ".lwn" << ends;
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
  
  // Create a population with this configuration
  cout << "Creating initial population ..." << endl;
  MyPopulation* pop=new MyPopulation (cfg, adfNs);
  pop->create ();
  cout << "Ok." << endl;
  pop->createGenerationReport (1, 0, fout, bout);
  cout << "Mown: " 
       << pop->NthMyGP(pop->bestOfPopulation)->mown << "% " 
       << endl;
  
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
      cout << "Mown: " 
	   << pop->NthMyGP(pop->bestOfPopulation)->mown << "% " 
	   << endl;
    }

  // Print lawn to a text file while evaluating the best one of the
  // last generation
  printLawn=1;
  lawnFile.open (strLawnFile.str());
  pop->NthMyGP(pop->bestOfPopulation)->evaluate();
  lawnFile.close ();

  cout << "\nResults are in " 
       << InfoFileName << ".dat, " 
       << InfoFileName << ".lwn, " 
       << InfoFileName << ".stc." << endl;
  
  return 0;
}
