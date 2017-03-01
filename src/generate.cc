
// generate.cc

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



#include <stdlib.h>

#include "gp.h"



// Apply evolution strategy to the population and return a container
// which members become part of the new generation
GPContainer* GPPopulation::evolution (GPPopulationRange& range)
{
  static int treedepth=2;
  GPContainer *gpCont;
  
  // Create new tree?
  if (GPRandomPercent (GPVar.CreationProbability))
    {
      // Allocate a new GP object with the correct number of subtrees.
      // We get the number of subtrees from the ADF container set
      GP* newGP=createGP (adfNs->containerSize());

      // Creation: use simple Variable Grow method.  Increase
      // treedepth
      newGP->create (GPVariable, treedepth, *adfNs);
      if (++treedepth>GPVar.MaximumDepthForCreation) 
	treedepth=2;

      // Allocate container and put the new GP into the container
      gpCont=new GPContainer (1);
      gpCont->put (0, *newGP);
    }
  else 
    // Ok, if not a new one is created, it must be crossover or
    // reproduction.
    {
      // Choose crossover?
      if (GPRandomPercent (GPVar.CrossoverProbability))
	{
	  // Crossover: Select two or even more parents from the deme
	  // (or whole population)
	  gpCont=selectParents (range);

	  // The correct cross function must be called.  Therefore,
	  // look at the first member of the container and call its
	  // cross function.  The cross function returns a container
	  // with an unspecified number of children, usually two.
	  GP& dad=(GP&)*gpCont->Nth(0);
	  gpCont=&dad.cross (gpCont, GPVar.MaximumDepthForCrossover);
	}
      else
	// Reproduction: Select one member from the deme (or whole
	// population).
	gpCont=select (1, range);

    }

  // Return container
  return gpCont;
}



// Generate creates a new generation from the current one.  If steady
// state genetic programming is used, the new generation overwrites
// the old one, otherwise the new generation is being put into the new
// population object given as parameter.
void GPPopulation::generate (GPPopulation& newPop)
{
  // Check whether we could get an infinite loop during crossover
  if (GPVar.MaximumDepthForCrossover<GPVar.MaximumDepthForCreation)
    GPExitSystem ("GPPopulation::generate", 
		  "MaximumDepthForCrossover is smaller than "\
		  "MaximumDepthForCreation");

  // If demetic grouping is used, the population is divided into
  // demes.  We implement it this way: Each deme undergoes the same
  // process than the whole population if no demetic grouping would be
  // applied.
  int demeSize;
  if (GPVar.DemeticGrouping)
    {
      demeSize=GPVar.DemeSize;

      // Some tests
      if (demeSize<2)
	GPExitSystem ("GPPopulation::generate", 
		      "Demetic group size is smaller than 2");
      if (demeSize>containerSize ())
	GPExitSystem ("GPPopulation::generate", "Deme size too big");
      if (containerSize ()%demeSize!=0)
	GPExitSystem ("GPPopulation::generate", "Wrong deme size: "\
		      "doesn't fit");
    }
  else
    demeSize=containerSize ();

  if (!GPVar.SteadyState)
    {
      // The user has already created a new generation object, but the
      // container should still be empty.  Allocate space for the new
      // population members.
      newPop.reserveSpace (containerSize ());

      // We take the best one so far and put it into the new
      // generation.  It makes the process of finding the best one
      // very easy (it is always member of the last generation).  The
      // best one stays exactly at the location where is was before
      // (the deme stays the same, also).  We have to take care later
      // that it is not overwritten at that position.
      if (GPVar.AddBestToNewPopulation)
	newPop.put (bestOfPopulation, 
		    NthGP (bestOfPopulation)->duplicate());
    }

  // For each deme (or the whole population, if no demetic grouping is
  // used) make reproduction and crossover and thus build up a new
  // generation
  for (int demeStart=0; demeStart<containerSize ();
       demeStart+=demeSize)
    {
      // Set up the range for which the selection is going to take
      // place
      GPPopulationRange range;
      range.firstSelectionPerDeme=1;
      range.startIx=demeStart;
      range.endIx=demeStart+demeSize;

      // Continue until the whole deme or population is full.  n is
      // incremented for every new member that comes into the new
      // population
      for (int n=0; n<demeSize; )
	{
	  // The genetic evolution returns a container with genetic
	  // programs to be put in the new generation
	  GPContainer *gpCont=evolution (range);

	  // Steady state programming: To replace bad genetic programs
	  // of the old population, they must be selected first.
	  int* selectBadGPs=NULL;
	  if (GPVar.SteadyState)
	    // Are there members in the container at all?
	    if (gpCont->containerSize ())
	      {
		int numToSelect=gpCont->containerSize ();
		selectBadGPs=new int [numToSelect];
		selectIndices (selectBadGPs, numToSelect, 1, range);
	      }

	  // Now we have a container with all the GPs that become
	  // members of the next population.  We only have to add them
	  // to the new population.  We would like to add all of them
	  // which might not be possible if the population (or current
	  // deme) is already completely occupied.  After that, we
	  // must delete the container (there might still be some
	  // members in it)
	  for (int j=0; j<gpCont->containerSize (); j++)
	    {
	      // We probably saved already the bestOfPopulation into
	      // the new generation (GPVariables parameter
	      // AddBestToNewPopulation).  Check for this GP and skip
	      // it (increase n, least the best one is overwritten)!
	      if (!GPVar.SteadyState)
		if (n<demeSize)
		  if (newPop.NthGP (demeStart+n))
		    n++;

	      if (n<demeSize)
		{
		  // Check that there is an object (the crossover
		  // function might not have been successful).  At
		  // least we hope so, otherwise an infinite loop
		  // could occur, if crossover is never successful.
		  // This is unlikely and doesn't happen in our cross
		  // function, so we don't check for that condition
		  if (gpCont->Nth (j))
		    {
		      // Get the object from the container.  Now that
		      // the container doesn't own it any more, we are
		      // responsible for its destruction ...
		      GP& newGP=(GP&)gpCont->get (j);

		      // This is the point where mutation comes in.
		      // One may wonder why not in the function
		      // evolution?  This is because a GP might have
		      // been mutated that won't be able to be put
		      // into the new generation, because this might
		      // be full (in short: it's faster).
		      newGP.mutate (GPVar, *adfNs);

		      // If SteadyState is used, evaluate the GP, then
		      // replace some bad genetic program selected
		      // before, otherwise put the object into the new
		      // population
		      if (GPVar.SteadyState)
			{
			  // Is the fitness still valid?
			  if (!newGP.fitnessValid)
			    {
			      newGP.evaluate ();
			      newGP.fitnessValid=1;
			    }
			  put (selectBadGPs[j], newGP);
			}
		      else
			newPop.put (demeStart+n, newGP);

		      // Increase index, or counter, call it as you
		      // will
		      n++;
		    }
		}
	    }

	  // Delete container and all objects that are still left in
	  // it, and the array of indices used for steady state
	  // programming
	  delete gpCont;
	  if (selectBadGPs)
	    delete [] selectBadGPs;
	}
    }

  // Now evaluate the new generation, if not steady state
  if (!GPVar.SteadyState)
    newPop.evaluate ();

  // If demetic grouping is used, let members migrate into other demes
  if (GPVar.DemeticGrouping)
    if (GPVar.SteadyState)
      demeticMigration ();
    else
      newPop.demeticMigration ();

  // Calculate statistics of the new generation
  if (GPVar.SteadyState)
    calculateStatistics ();
  else
    newPop.calculateStatistics ();
}



// What is demetic grouping ? (By Adam Fraser)

// Don't you read the gp mailing list this comes up about every two
// weeks.

// The total population undergoing the evolutionary process is
// subdivided into a number of groups (demes) which are unable to
// interact with other groups except through the use of migratory
// agents which are selected using a probabilistic measure.  This
// subdivision of the population allows the demes to evolve along
// separate paths without this path becoming tightly focused upon any
// particular area within the global search space


// Huh, Demes what are they good for, say it again! [ with apologies
// to Bruce Springsteen ]

// Hmmmm, damn good question it probably needs a good answer but I am
// not going to try.  Demes have been used to halt premature
// convergence of the population as they are unlikely to find the same
// sub optimal structure.  Demes are also good when considering the
// relatedness of structures (my particular area of study so don't get
// me started on it or you will never shut me up), they could also be
// useful in using muliple population by considering them as tribes
// and electing tribal leaders who compete for the deme.

// From biology there are two theories Fisher and Wright to
// paraphrase.  Fisher feels that everything should be able to
// reproduce with every other member of the population (a bit like
// late 1960's but without the rather strange halluciogenics). Wright
// thinks populations get forced into groups due to enviromental
// constraints such as two tortoises ten miles apart. Anyway do what
// you will here is the code.

// Demetic grouping selection in which grouping is carried out through
// spatial groups on a ONE dimensional torus (therefore a hoop). Sub
// populations are placed within a valley and only random wanderers
// can get out. These must therefore be specifically found and moved
// around etc....  apf 25 February 1994

// If demetic grouping is used, the population is divided into demes.
// We make no other checks (concerning deme size), as there have been
// a lot of checks in generate ().
void GPPopulation::demeticMigration ()
{
  // For each deme select a wanderer that will be exchanged with a
  // selected member of the next deme.
  for (int demeStart=0; demeStart<containerSize ()-GPVar.DemeSize; 
       demeStart+=GPVar.DemeSize)
    {
      // There is a user-defined possibility that demetic migration
      // will take place
      if (GPRandomPercent (GPVar.DemeticMigProbability))
	{
	  // Set selection range
	  GPPopulationRange range1, range2;
	  range1.firstSelectionPerDeme=1;
	  range1.startIx=demeStart;
	  range1.endIx=demeStart+GPVar.DemeSize;
	  range2.firstSelectionPerDeme=1;
	  range2.startIx=demeStart+GPVar.DemeSize;
	  range2.endIx=demeStart+GPVar.DemeSize*2;

	  // Select the best of each deme, using the usual selection
	  // sceme
	  int r1, r2;
	  selectIndices (&r1, 1, 0, range1);
	  selectIndices (&r2, 1, 0, range2);

	  // Exchange population members
	  GPObject* p1=&get (r1);
	  GPObject* p2=&get (r2);
	  put (r2, *p1);
	  put (r1, *p2);
	}
    }
}



// This function evaluates the index of the best and worst of the
// population, and calculates average values of the fitness, length
// and depth.
void GPPopulation::calculateStatistics ()
{
  // Average values
  avgFitness=totalFitness () / (double) containerSize ();
  avgLength=(double)totalLength () / (double) containerSize ();
  avgDepth=(double)totalDepth () / (double) containerSize ();

  // loop through whole population and search for the best and worst
  GP* worst;
  GP* best;
  for (int n=0; n<containerSize(); n++)
    {
      GP* current=NthGP (n);
#if GPINTERNALCHECK
      if (!current)
	GPExitSystem ("GPPopulation::calculateStatistics", "GP is NULL");
#endif

      // Save the best and worst of the population
      if (n==0)
	{
	  worstOfPopulation=n;
	  bestOfPopulation=n;
	  worst=current;
	  best=current;
	}
      else
	{
	  // Search for worst GP.  If fitness is equal, take the one
	  // with the worst length
	  if (worst->stdFitness < current->stdFitness 
	      || (worst->stdFitness == current->stdFitness
		  && worst->length () < current->length ()))
	    {
	      worstOfPopulation=n;
	      worst=current;
	    }

	  // Search for best GP.  If fitness is equal, take the one
	  // with the best length
	  if (best->stdFitness > current->stdFitness
	      || (best->stdFitness == current->stdFitness
		  && best->length () > current->length ()))
	    {
	      bestOfPopulation=n;
	      best=current;
	    }
	}
    }
}




