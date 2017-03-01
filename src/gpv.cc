
// gpv(ariables).cc

/* -------------------------------------------------------------------

gpc++ - The Genetic Programming Kernel

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


Copyright 1993, 1994 Adam P. Fraser and 1996, 1997 Thomas Weinbrenner

For comments, improvements, additions (or even money) contact:

Thomas Weinbrenner
Grauensteinstr. 26
35789 Laimbach
Germany
E-mail: thomasw@emk.e-technik.th-darmstadt.de
WWW:    http://www.emk.e-technik.th-darmstadt/~thomasw

  or 

(Address may be out of date)
Adam Fraser, Postgraduate Section, Dept of Elec & Elec Eng,
Maxwell Building, University Of Salford, Salford, M5 4WT, United Kingdom.
E-mail: a.fraser@eee.salford.ac.uk
Tel:    (UK) 061 745 5000 x3633
Fax:    (UK) 061 745 5999

------------------------------------------------------------------- */

// A nice simple class to contain all the genetic programming
// variables



#include <stdlib.h>
#include <fstream>
#include "gp.h"

using namespace std;

// Setup default values
GPVariables::GPVariables ()
{
  PopulationSize=100;
  NumberOfGenerations=20;
  CrossoverProbability=95.0;
  CreationProbability=2.0;
  CreationType=2;
  MaximumDepthForCreation=6;
  MaximumDepthForCrossover=17;
  SelectionType=GPTournamentSelection;
  TournamentSize=10;
  DemeticGrouping=0;
  DemeSize=100;
  DemeticMigProbability=100.0;
  SwapMutationProbability=0.0;
  ShrinkMutationProbability=0.0;
  AddBestToNewPopulation=1;
  SteadyState=1;
}



GPVariables::GPVariables (const GPVariables& gpo) : GPObject(gpo) 
{
  PopulationSize=gpo.PopulationSize;
  NumberOfGenerations=gpo.NumberOfGenerations;
  CrossoverProbability=gpo.CrossoverProbability;
  CreationProbability=gpo.CreationProbability;
  CreationType=gpo.CreationType;
  MaximumDepthForCreation=gpo.MaximumDepthForCreation;
  MaximumDepthForCrossover=gpo.MaximumDepthForCrossover;
  SelectionType=gpo.SelectionType;
  TournamentSize=gpo.TournamentSize;
  DemeticGrouping=gpo.DemeticGrouping;
  DemeSize=gpo.DemeSize;
  DemeticMigProbability=gpo.DemeticMigProbability;
  SwapMutationProbability=gpo.SwapMutationProbability;
  ShrinkMutationProbability=gpo.ShrinkMutationProbability;
  AddBestToNewPopulation=gpo.AddBestToNewPopulation;
  SteadyState=gpo.SteadyState;
}



// Write variables to ostream
void GPVariables::printOn (ostream& os)
{
  os << "PopulationSize            = " << PopulationSize
     << "\nNumberOfGenerations       = " << NumberOfGenerations
     << "\nCrossoverProbability      = " << CrossoverProbability
     << "\nCreationProbability       = " << CreationProbability
     << "\nCreationType              = ";
  switch (CreationType)
    {
    case GPVariable:
      os << "Variable";
      break;
    case GPGrow:
      os << "Grow";
      break;
    case GPRampedHalf:
      os << "Ramped Half and Half";
      break;
    case GPRampedVariable:
      os << "Ramped Variable";
      break;
    case GPRampedGrow:
      os << "Ramped Grow";
      break;
    case GPUserDefinedCreation:
      os << "User defined";
      break;
    default:
      GPExitSystem ("GPVariables::printOn", "Wrong creation type");
    }
  os << "\nMaximumDepthForCreation   = " << MaximumDepthForCreation
     << "\nMaximumDepthForCrossover  = " << MaximumDepthForCrossover
     << "\nSelectionType             = ";
  switch (SelectionType)
    {
    case GPProbabilisticSelection:
      os << "Probablistic selection";
      break;
    case GPTournamentSelection:
      os << "Tournament selection";
      break;
    case GPUserDefinedSelection:
      os << "User defined";
      break;
    default:
      GPExitSystem ("GPVariables::printOn", "Wrong selection type");
    }
  os << "\nTournamentSize            = " << TournamentSize
     << "\nDemeticGrouping           = " << (DemeticGrouping?"On":"Off")
     << "\nDemeSize                  = " << DemeSize
     << "\nDemeticMigProbability     = " << DemeticMigProbability
     << "\nSwapMutationProbability   = " << SwapMutationProbability
     << "\nShrinkMutationProbability = " << ShrinkMutationProbability
     << "\nAddBestToNewPopulation    = " << AddBestToNewPopulation
     << "\nSteadyState               = " << SteadyState
     << endl;
}



// Load operation
char* GPVariables::load (istream& is)
{
  is >> PopulationSize;
  is >> NumberOfGenerations;
  is >> CrossoverProbability;
  is >> CreationProbability;
  is >> CreationType;
  is >> MaximumDepthForCreation;
  is >> MaximumDepthForCrossover;
  is >> SelectionType;
  is >> TournamentSize;
  is >> DemeticGrouping;
  is >> DemeSize;
  is >> DemeticMigProbability;
  is >> SwapMutationProbability;
  is >> ShrinkMutationProbability;
  is >> AddBestToNewPopulation;
  is >> SteadyState;
  return NULL;
}



// Save operation
void GPVariables::save (ostream& os)
{
  os << PopulationSize << ' ';
  os << NumberOfGenerations << ' ';
  os << CrossoverProbability << ' ';
  os << CreationProbability << ' ';
  os << CreationType << ' ';
  os << MaximumDepthForCreation << ' ';
  os << MaximumDepthForCrossover << ' ';
  os << SelectionType << ' ';
  os << TournamentSize << ' ';
  os << DemeticGrouping << ' ';
  os << DemeSize << ' ';
  os << DemeticMigProbability << ' ';
  os << SwapMutationProbability << ' ';
  os << ShrinkMutationProbability << ' ';
  os << AddBestToNewPopulation << ' ';
  os << SteadyState << ' ';
}
