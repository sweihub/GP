
// create.cc

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

#include "gp.h"



// Number of trials to create a proper GP
const int creationAttempts=50;



// Creation of the population
void GPPopulation::create ()
{
#if GPINTERNALCHECK
  // Check whether the user has done a good job and setup the node
  // sets correctly.
  if (adfNs->containerSize()<1)
    GPExitSystem ("Population::create", 
		  "ADF Node set contains no node sets");
  for (int tree=0; tree<adfNs->containerSize(); tree++)
    {
      GPNodeSet* ns=adfNs->NthNodeSet (tree);
      if (!ns)
	GPExitSystem ("Population::create", "Node set not defined");
      if (ns->containerSize()==0)
	GPExitSystem ("Population::create", "Node set contains no nodes");
      for (int i=0; i<ns->containerSize(); i++)
	if (!ns->Nth (i))
	  GPExitSystem ("Population::create", "Node in set not defined");
    }
#endif

  const int minTreeDepth=2;
  int treedepth=2; 
  GP* newObj;

  // At this point, the population container is still empty.  Space
  // must be reserved for all the genetic programs.
  reserveSpace (GPVar.PopulationSize);

  // loop through the whole population
  for (int i=0; i<containerSize (); i++)
    {
      // set up done boolean checks whether we have been around the
      // next do ..while loop
      int Done = 0;
      do
	{
	  // Check that we haven't done this more than a few times.
	  // If so, increase tree depth.  Obviously, it's difficult to
	  // create a proper tree of that depth
	  if (Done>=creationAttempts/4 
	      && treedepth<GPVar.MaximumDepthForCreation)
	    treedepth++;

	  // Allocate a new GP object with the correct number of
	  // subtrees (ADFs+1).  The number of subtrees is called up
	  // from one of the ADF container sets.  A virtual function
	  // is used to create the object, so it may well be a users
	  // class GP we are creating here.
	  newObj=createGP (adfNs->containerSize());

	  // Basically a switch statement is used to decide which type
	  // of GP creation is wanted
	  switch (GPVar.CreationType)
	    {
	    case GPRampedHalf:
	      // if odd create ramped grow else ramped variable
	      if (i%2)
		newObj->create (GPGrow, treedepth, *adfNs);
	      else
		newObj->create (GPVariable, treedepth, *adfNs);
	      break;
	    case GPRampedVariable:
	      newObj->create (GPVariable, treedepth, *adfNs);
	      break;
	    case GPRampedGrow:
	      newObj->create (GPGrow, treedepth, *adfNs);
	      break;
	    case GPGrow:
	      newObj->create(GPGrow, GPVar.MaximumDepthForCreation, 
			     *adfNs);
	      break;
	    case GPVariable:
	      newObj->create(GPVariable, GPVar.MaximumDepthForCreation, 
			     *adfNs);
	      break;
	    default:
#if GPINTERNALCHECK
	      GPExitSystem ("Population::create", "Wrong creation type");
#endif
	      break;
	    }

	  // Now leave the object in the care of the container class.
	  // If there is already one object at that place, it will be
	  // deleted.  No more worries about memory allocations!
	  // What a drag it was before...
	  put (i, *newObj);

	  // Set up boolean done to show we have been around do{}while
	  // loop before.
	  Done++;
	}  
      // Check for a "good" GP.  This could lead to an infinite loop,
      // but the process is stopped after some attempts to create a
      // proper GP.
      while (!checkForValidCreation (*newObj) && Done < creationAttempts);

      // Now increase treedepth
      if (++treedepth>GPVar.MaximumDepthForCreation) 
	treedepth=minTreeDepth;
    }

  // Evaluate each member of the population.  This must be done
  // anyway, so why not here?
  evaluate ();

  // Calculate statistics of the new generation
  calculateStatistics ();
}



// Compares each member of the population with a GP up to that member
// of the population.  Used by checkForValidCreation().  Returns 0, if
// two equal members are found
int GPPopulation::checkForDiversity (GP& gp)
{
  for (int n=0; n<containerSize(); n++)
    {
      GP* cmp=NthGP (n);

      // If we reach the same GP at this point, stop
      if (cmp==&gp)
	break;

      // Compare two GPs together and return 0 if the same if not
      // continue in loop
      if (cmp->compare (gp)==0)
	return 0;
    }

  // If we get to here then all GPs must be different and we still
  // have 100% diversity in the population
  return 1;
}



// What this routine has to do is to check whether the newly created
// GP is a "good" one or not.  As default, it is checked here for
// ultimate diversity as it checks that each GP is different to every
// other created so far, but the user might as well think different
// about what is a good GP or not.  It won't lead to an infinite loop
// if the routine rejects every GP it gets, but will certainly slow
// down creation process.  Return 1 if good, 0 if we should give it
// another try.  Remember the compare function returns a 0 if it finds
// an identical GP.
int GPPopulation::checkForValidCreation (GP& gpo)
{
  return checkForDiversity (gpo)!=0;
}



// This block of code creates a GP. GP::Create is called within
// GPPopulation::create with the correct creation type and depth.
// Notes: This code makes a function the first node.  This is in line
// with Koza's LISP code and does make the trees more interesting but
// it is not necessary.  It makes this code a little shorter, as the
// decision whether to choose a function or a terminal is not done
// here, but only in GPGene::create().
void GP::create (enum GPCreationType ctype, int allowableDepth, 
		 GPAdfNodeSet& adfNs)
{
#if GPINTERNALCHECK
  // Check argument
  if (ctype!=GPGrow && ctype!=GPVariable)
    GPExitSystem ("GP::create",
		  "Argument ctype must be GPGrow or GPVariable");
#endif

  // As the first node is always a function that is created here in
  // this routine, decrease allowableDepth
  allowableDepth--;

  // loop through each adf
  for (int adf=0; adf<containerSize(); adf++)
    {
      // Set the node set pointer to the correct node set (most
      // interesting work is when these are different for each ADF)
      GPNodeSet& ns=*adfNs.NthNodeSet (adf);

      // Choose a function from function set which complies with
      // Genetic Programming book or code.  Create a new gene with that
      // function.
      GPNode &tempfunc=ns.chooseFunction ();
      GPGene& g=*createGene (tempfunc);

      // Create tree structure
      g.create (ctype, allowableDepth, ns);

      // Now we put the child into the container.  Nice, isn't it?
      put (adf, g);
    }

  // Calculate length and depth
  calcLength ();
  calcDepth ();
}



// Grow GPs according to the given parameters
void GPGene::create (enum GPCreationType ctype, int allowableDepth, 
		     GPNodeSet& ns)
{
  // Loop through the whole function arguments and create new
  // offsprings (the function parameters).  Remember: There is already
  // a Gene, namely the object this routine is called for by
  // GP::create(), and it is a function.
  for (int n=0; n<containerSize(); n++)
    {
      // Now decide whether the offspring should be a function or a
      // terminal.  If there is no more allowable depth, choose a
      // terminal in any case.
      int chooseTerm;
      if (ctype==GPGrow) 
	chooseTerm=0;
      else 
	// 50/50% chance of getting a function or a terminal
	chooseTerm=GPrand () % 2;
      if (allowableDepth<=1)
	chooseTerm=1;

      // Choose function or terminal
      GPNode* newNode;
      if (chooseTerm)
	newNode=&ns.chooseTerminal();
      else
	newNode=&ns.chooseFunction();

      // Create a new gene with the chosen node and place the new gene
      // into the container
      GPGene& g=*createChild (*newNode);
      put (n, g);
      
      // If the node was a function, call function recursively with
      // decreased allowable depth
      if (!chooseTerm)
	g.create (ctype, allowableDepth-1, ns);
    }
}





