
// cross.cc

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

// Crosses two gps together to produce two resulting children

#include <stdlib.h>

#include "gp.h"



// Returns a random gene selected by random number generator.
// Obviously it is quite important to know the length of the GP before
// calling this function.  This also adds a component laid down by
// Koza that there will be a 90% chance of getting a function.  This
// is produced by going through a loop 10 times and returning ONLY if
// a function is found.  This should give a greater chance of getting
// a function.  Note that if you have a single node this function will
// not hang.

// This function is useful for crossover.  The parameter is the adress
// of a pointer that refers to the root gene.  We also return the
// adress of a pointer to the selected gene.  This makes crossover
// very simple, as only pointers are swapped.  The object this
// function is called for is of no importance.
GPGene** GPGene::choose (GPGene** rootPtr)
{
  GPGene **pg=NULL;

  // Calculate the length of the subtree starting at the given gene
  int totalLength=(**rootPtr).length ();

#if GPINTERNALCHECK
  if (totalLength==0)
    GPExitSystem ("GPGene::choose", 
		  "Tree length is 0: Can't select crossover point");
#endif

  // loop 10 times
  for (int i=0; i<10; i++)
    {
      // Calculate a random number between 1..totalLength
      int iLengthCount = (GPrand() % totalLength) + 1;

      // Find gene with this value
      pg=findNthNode (rootPtr, 0, iLengthCount);
#if GPINTERNALCHECK
      if (!pg)
	GPExitSystem ("GPGene::choose", "Didn't find tree node");
#endif

      // If this pointer points to a function rather than terminal
      // return this value else keep going around the loop
      if ((**pg).isFunction ()) 
	return pg;
    }

  // If after 10 loops still don't have function well return terminal
  return pg;
}



// Cross the objects contained in the given container.  The function
// is responsible for the given container and has to delete it, if a
// completely new container is returned.  We use only the objects in
// the container for the crossover operation, and not the object for
// which we were called (though this is in our case a member of the
// container).  We can use all container objects and do with them what
// we want.  We only have to return another container.  The returned
// container needn't contain any objects, but definitely should at
// least sometimes, otherwise an infinite loop will occur, because the
// generate-function tries to fill the new population with new
// members, and if we return none, well...
GPContainer& GP::cross (GPContainer* parents, 
			int maxdepthforcrossover)
{
#if GPINTERNALCHECK
  // We are conservative: Only two sexes allowed
  if (parents->containerSize()!=2)
    GPExitSystem ("GP::cross", "Only two parents allowed for crossover");
#endif

  // Get pointers of mum and dad from container
  GP& dad=*(GP*)parents->Nth (0);
  GP& mum=*(GP*)parents->Nth (1);

#if GPINTERNALCHECK
  // GP's certainly must have the same number of trees
  if (dad.containerSize()!=mum.containerSize())
    GPExitSystem ("GP::cross", 
		  "Mum and Dad must have same number of trees");
  if (dad.containerSize()==0)
    GPExitSystem ("GP::cross", "Parents contain no trees");
#endif

  // Work out which adf branch we are going to cut from
  int randTree=GPrand() % dad.containerSize();

  // Get the adresses of the pointers to the root genes of the ADF
  // branch we are going to cut from
  GPGene** rootGene1=(GPGene**) dad.getPointerAddress (randTree);
  GPGene** rootGene2=(GPGene**) mum.getPointerAddress (randTree);

#if GPINTERNALCHECK
  if (!*rootGene1 || !*rootGene2)
    GPExitSystem ("GP::cross", 
		  "Genetic tree of Mum or Dad is NULL");
#endif

  // Loop around this as long as we find two points on the trees so
  // that the maxdepthforcrossover is not exceeded
  int maxDepth1, maxDepth2;
  do
    {
      // Determine the cut points by choosing a node within mum and
      // dad
      GPGene** cutPoint1=(**rootGene1).choose (rootGene1);
      GPGene** cutPoint2=(**rootGene2).choose (rootGene2);

      // Swap the whole subtrees.  Easy, isn't it? And so fast...
      GPGene* tmp=*cutPoint1;
      *cutPoint1=*cutPoint2;
      *cutPoint2=tmp;

      // Here the maximum depth of the new trees and only that trees
      // is calculated as other trees we assume to be under the
      // maximum depth of crossover
      maxDepth1=(**rootGene1).depth ();
      maxDepth2=(**rootGene2).depth ();

      // Make sure that maximum depth is not too high.  If so, swap
      // the subtrees back
      if (maxDepth1>maxdepthforcrossover || maxDepth2>maxdepthforcrossover)
	{ 
	  tmp=*cutPoint1;
	  *cutPoint1=*cutPoint2;
	  *cutPoint2=tmp;
	}
    }
  while (maxDepth1>maxdepthforcrossover || maxDepth2>maxdepthforcrossover);

  // After crossover, the fitness of the GP is no longer valid, so we
  // set the corresponding flag.  The length and depth has to be
  // recalculated as well.
  dad.fitnessValid=0;
  mum.fitnessValid=0;
  dad.calcLength();
  dad.calcDepth();
  mum.calcLength();
  mum.calcDepth();

  // We return the same container, so we don't have to allocate a new
  // one and delete the parents container
  return *parents;
}
