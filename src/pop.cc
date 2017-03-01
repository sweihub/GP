
// pop.cc

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

// The Population class has most of the genetic programming operator
// applied to it. 

#include <iomanip>
#include <stdio.h>
#include "gp.h"

using namespace std;

// Prints out the complete population
void GPPopulation::printOn (ostream& os)
{
  for (int n=0; n<containerSize(); n++)
    {
      GP* current=NthGP (n);
      if (current)
	os << *current << endl;
      else 
	os << "(NULL)" << endl;
    }
}



// Output all the data found in a generation....
void GPPopulation::createGenerationReport (int printLegend, int generation, 
					   ostream& fout, ostream& bout)
{
  if (printLegend)
    {
      cout << "Gen|       Fitness     |      Length       |   Depth\n"
	   << "   |  Best|Avg.|Worst  |  Best|Avg.|Worst  |  Best|Avg.|Worst\n";
      bout << "#Gen|       Fitness     |      Length       |   Depth\n"
	   << "#   |  Best|Avg.|Worst  |  Best|Avg.|Worst  |  Best|Avg.|Worst\n";
    }
  bout << generation 
       << ' ' << NthGP(bestOfPopulation)->stdFitness 
       << ' ' << avgFitness
       << ' ' << NthGP(worstOfPopulation)->stdFitness 
       << "    "
       << ' ' << NthGP(bestOfPopulation)->length ()
       << ' ' << avgLength
       << ' ' << NthGP(worstOfPopulation)->length ()
       << "    "
       << ' ' << NthGP(bestOfPopulation)->depth ()
       << ' ' << avgDepth
       << ' ' << NthGP(worstOfPopulation)->depth ()
       << endl;
  cout << generation 
       << ' ' << NthGP(bestOfPopulation)->stdFitness 
       << ' ' << avgFitness
       << ' ' << NthGP(worstOfPopulation)->stdFitness 
       << "    "
       << ' ' << NthGP(bestOfPopulation)->length ()
       << ' ' << avgLength
       << ' ' << NthGP(worstOfPopulation)->length ()
       << "    "
       << ' ' << NthGP(bestOfPopulation)->depth ()
       << ' ' << avgDepth
       << ' ' << NthGP(worstOfPopulation)->depth ()
       << endl;

  // Place the best of generation in output files
  fout << "Best of generation " << generation 
       << " (Fitness = " << NthGP (bestOfPopulation)->stdFitness 
       << ", Structural Complexity = " << NthGP (bestOfPopulation)->length () 
       << ")" << endl 
       << *NthGP (bestOfPopulation)
       << endl;
}



// Returns the total summated fitness of population.
double GPPopulation::totalFitness()
{
  // Loop through whole population
  GP* current;
  double sum = 0;
  for (int n=0; n<containerSize(); n++)
    if ((current=NthGP (n)))
      sum+=current->stdFitness;  

  // return result
  return sum;
}



// Returns the total summated structural complexity of population
long GPPopulation::totalLength()
{
  // move through population adding up lengths
  long sum = 0;
  GP* current;
  for (int n=0; n<containerSize(); n++)
    if ((current=NthGP (n)))
      sum+=current->length ();

  // return result
  return sum;
}



// Return total summated depth of population
long GPPopulation::totalDepth()
{
  // move through population adding up depths
  GP* current;
  long sum=0;
  for (int n=0; n<containerSize(); n++)
    if ((current=NthGP (n)))
      sum+=current->depth();

  // return result
  return sum;
}



// Resolve the pointers to the nodes, for each tree.  Must be done
// every time after a load()
void GPPopulation::setNodeSets (GPAdfNodeSet& adfNs_)
{
  // Set object variable
  adfNs=&adfNs_;

  // Every GP has to be informed about the new node sets.  Most
  // important: Each GP is converting the node values to pointer to
  // nodes
  GP* current;
  for (int n=0; n<containerSize(); n++)
    if ((current=NthGP (n)))
      current->resolveNodeValues (*adfNs);
}



// Load operation
char* GPPopulation::load (istream& is)
{
  // Load GPVariables
  char* errMsg=GPVar.load (is);
  if (errMsg) return errMsg;

  // Load container
  errMsg=GPContainer::load (is);
  if (errMsg) return errMsg;

  // We don't need to save all parameters, some can be calculated
  // after the population has been loaded
  calculateStatistics ();
  return NULL;
}



// Save operation
void GPPopulation::save (ostream& os)
{
  // Save GPVariables
  GPVar.save (os);

  // Save container
  GPContainer::save (os);
}
