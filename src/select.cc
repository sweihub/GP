
// select.cc

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

// All the common code for selections of a gp



#include "gp.h"



// Sometimes, an array of integer is needed, with unspecified size,
// where in most cases, however, the size is just a few.  We provide
// such an array here to save us the slow memory allocation, but
// allocate if necessary.  I would like to use templates here (I hate
// it to duplicate code, and a #define macro is not what I want), but
// it makes no sense just for the few lines...

const int selectionArraySize=2;
class IntegerArraySel
{
public:

  // Constructor
  IntegerArraySel (int size) 
  {
    // Is is necessary to allocate space?  If not, set pointer to
    // local array
    allocateFlag=size>selectionArraySize;
    if (allocateFlag)
      arrayPtr=new int [size];
    else 
      arrayPtr=&array[0];
  }

  // Destructor
  ~IntegerArraySel () 
  {
    // If space was allocated, delete it
    if (allocateFlag)
      delete [] arrayPtr;
  }

  // Access array via [] operator
  int& operator [] (int index) { return *(arrayPtr+index); }
private:
  // Flag whether space was allocated or not
  short allocateFlag;

  // Pointer to allocated space or local array
  int* arrayPtr;

  // Local array
  int array[selectionArraySize];
};



// Same for tournament array
const int tournamentArraySize=20;
class IntegerArrayTourn
{
public:

  // Constructor
  IntegerArrayTourn (int size) 
  {
    // Is is necessary to allocate space?  If not, set pointer to
    // local array
    allocateFlag=size>tournamentArraySize;
    if (allocateFlag)
      arrayPtr=new int [size];
    else 
      arrayPtr=&array[0];
  }

  // Destructor
  ~IntegerArrayTourn () 
  {
    // If space was allocated, delete it
    if (allocateFlag)
      delete [] arrayPtr;
  }

  // Access array via [] operator
  int& operator [] (int index) { return *(arrayPtr+index); }

private:
  // Flag whether space was allocated or not
  short allocateFlag;

  // Pointer to allocated space or local array
  int* arrayPtr;

  // Local array
  int array[tournamentArraySize];
};



// Selection routine for tournament selection

// By Adam Fraser.  What is tournament selection?  Tournament
// selection randomly selects a number of genetic programs from the
// population.  The fitness of each member of this group are compared
// with each other and the best replaces the worst.

// Why should I use tournament selection?  This method is one which
// can be easily implemented using a parallel methodology and seems to
// result in a quicker solution in a search than the fitness
// proportionate method to be used by Koza.  No idea why really it
// just seems to give better emperical results though some people
// (Howard Oakley ??) find that with certain problems (he studies
// chaotic data series of blood flow) tournament selection actually
// gives worst results.  So be careful with the selection method you
// choose and check a few methods out before doing thousands of runs.

// We have to select some members (this routine works only with 1 and
// 2 members, otherwise we would have to use some sort algorithm to
// get the n best/worst ones of the tournament), and return them in
// the array pointed to by the variable selection.
void GPPopulation::tournamentSelection (int *selection, int numToSelect,
					int selectWorst, 
					GPPopulationRange& range)
{
  int i;

#if GPINTERNALCHECK
  if (numToSelect!=1 && numToSelect!=2)
    GPExitSystem ("GPPopulation::tournamentSelection", 
		  "Wrong argument numToSelect");
  if (GPVar.TournamentSize<2)
    GPExitSystem ("GPPopulation::tournamentSelection", 
		  "Wrong tournament size");
  if (range.endIx-range.startIx<1)
    GPExitSystem ("GPPopulation::tournamentSelection", 
		  "Range to select from is empty");
#endif

  // The tournament is an array of integers, containing the indices to
  // the GPs
  IntegerArrayTourn tourn (GPVar.TournamentSize);

  // We randomly select a few members from the given index range and
  // put the indices into the tournament array.
  for (i=0; i<GPVar.TournamentSize; i++)
    tourn[i]=range.startIx + GPrand () % (range.endIx-range.startIx);

  // Now we look for the two best/worst ones, which become the
  // parents.  Well, perhaps we need only one
  int bestOrWorst=0, second=1;
  if (selectWorst)
    // Find the two worst ones
    {
      if (NthGP (tourn[0])->stdFitness < NthGP (tourn[1])->stdFitness)
	{
	  bestOrWorst=1; second=0;
	}
      for (i=2; i<GPVar.TournamentSize; i++)
	if (NthGP (tourn[i])->stdFitness 
	    > NthGP (tourn[bestOrWorst])->stdFitness)
	  {
	    second=bestOrWorst;
	    bestOrWorst=i;
	  }
	else
	  if (NthGP (tourn[i])->stdFitness 
	    > NthGP (tourn[second])->stdFitness)
	    second=i;
    }
  else
    // Find the two best ones
    {
      if (NthGP (tourn[0])->stdFitness > NthGP (tourn[1])->stdFitness)
	{
	  bestOrWorst=1; second=0;
	}
      for (i=2; i<GPVar.TournamentSize; i++)
	if (NthGP (tourn[i])->stdFitness 
	    < NthGP (tourn[bestOrWorst])->stdFitness)
	  {
	    second=bestOrWorst;
	    bestOrWorst=i;
	  }
	else
	  if (NthGP (tourn[i])->stdFitness 
	      < NthGP (tourn[second])->stdFitness)
	    second=i;
    }

  // Put the index of the best/worst (maximum: two) in the array
  selection[0]=tourn[bestOrWorst];
  if (numToSelect==2)
    selection[1]=tourn[second];
}



// Calc 1/x
inline static double inverse (double x)
{
  if (x<1e-10)
    return 1e10;
  else 
    return 1.0/x;
}



// Selects the n best or worst members using a probablistic fitness
// function in the population.  For information see John Koza 'Genetic
// Programming' or David Goldberg 'Genetic Algorithms for Search and
// Optimisation'.  I had to change the algorithm a bit because we are
// now using standardized fitness which means we have to use the
// inverse value of it.  We have to select some members, and return
// them in the array pointed to by the variable selection.
void GPPopulation::probabilisticSelection (int *selection, int numToSelect, 
					   int selectWorst, 
					   GPPopulationRange& range)
{
  int i;

#if GPINTERNALCHECK
  if (numToSelect<1)
    GPExitSystem ("GPPopulation::probablisticSelection",
		  "Wrong argument numToSelect");
#endif

  // If this is the first time the function is called for this deme,
  // loop through population or deme and summate all fitnesses.
  if (range.firstSelectionPerDeme)
    {
      range.firstSelectionPerDeme=0;
      sumFitness=0.0; invSumFitness=0.0;
      for (i=range.startIx; i<range.endIx; i++)
	{
	  GP* gp=NthGP (i);
	  invSumFitness+=inverse (gp->stdFitness);
	  sumFitness+=gp->stdFitness;
	}
    }

  // We select as much members as the user wants (usually 1 or 2)
  for (int n=0; n<numToSelect; n++)
    {
      // Get random number between 0 and a high value.  The higher the
      // value, the better the resolution.  Be careful what numbers
      // the random generator gives us! (The new random generator
      // doesn't have the problems the old one had)
      const long highValue=300000l;
      double rand=(double) (GPrand() % highValue);

      // Loop through population, summing up the fitness/inversed
      // fitness.  As we are using standardized fitness, we sum up
      // 1/stdFitness when looking for the best, which yields to the
      // maximum value of invSumFitness.  If we reach the randomized
      // value (we have to perform some scaling here), thats the
      // member we want!
      if (selectWorst)
	{
	  double sum=0.0;
	  double lookingForFit=rand / (double)(highValue-1l) 
	    * sumFitness;
	  for (i=range.startIx; i<range.endIx; i++)
	    {
	      sum+=NthGP (i)->stdFitness;
	      if (sum>=lookingForFit)
		break;
	    }
	}
      else
	{
	  double sumInv=0.0;
	  double lookingForInvFit=rand / (double)(highValue-1l) 
	    * invSumFitness;
	  for (i=range.startIx; i<range.endIx; i++)
	    {
	      sumInv+=inverse (NthGP (i)->stdFitness);
	      if (sumInv>=lookingForInvFit)
		break;
	    }
	}

      // This shouldn't happen, but just in case...
      if (i>=range.endIx) 
	i=range.endIx-1;

      // Add the selected population member to the array of indives
      selection[n]=i;
    }
}



// We select numToSelect best or worst population members from the
// population using one of the specified selection methods, and return
// the indices of the selected members as an array of integers.  We
// don't select from all population members, but only in a range
// between startIx and endIx (the last one exclusive).  This is used
// for demetic grouping.
void GPPopulation::selectIndices (int *selection, int numToSelect, 
				  int selectWorst, 
				  GPPopulationRange& range)
{
  // The selection method differs
  switch (GPVar.SelectionType)
    {

    case GPTournamentSelection:
      tournamentSelection (selection, numToSelect, selectWorst, range);
      break;

    case GPProbabilisticSelection:
      probabilisticSelection (selection, numToSelect, selectWorst, range);
      break;

    default:
#if GPINTERNALCHECK
      GPExitSystem ("GPPopulation::select", "Unknown selection method");
#endif
      break;
    }
}



// Select numToSelect best population members from the population
// using the function selectIndices, make a copy of them and return
// them in a container.  We don't select from all population members,
// but only in a range between startIx and endIx (the last one
// exclusive).  This is used for demetic grouping.
GPContainer* GPPopulation::select (int numToSelect, 
				   GPPopulationRange& range)
{
  // An array of integers with the indices of the selected GPs
  IntegerArraySel selec (numToSelect);

  // Select the best ones
  selectIndices (&selec[0], numToSelect, 0, range);

  // Allocate container
  GPContainer* cont=new GPContainer (numToSelect);

  // Add the selected population member to the container, making a
  // copy of it
  for (int n=0; n<numToSelect; n++)
    cont->put (n, NthGP(selec[n])->duplicate ());

  // Return container
  return cont;
}



// Select two parents and return them in a container.
GPContainer* GPPopulation::selectParents (GPPopulationRange& range)
{
  return 
    select (2, range);
}
