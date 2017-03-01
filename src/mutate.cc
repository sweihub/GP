
// mutate.cc

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

#include <stdio.h>

#include "gp.h"



// Swap mutation: Number of trials to find a node with same number of
// arguments, but different node value.
const int swapAttempts=5;



// This block of code perform allele or swap mutation on a genetic
// program.  This comprised of code being swapped with other code with
// certain constraints.  Any terminal can be swapped with any other
// but functions can only be swapped with other functions with the
// same arguments.  This means that the mutation does not have to
// create new branches when different function types are swapped which
// seems implicitly wrong
void GP::swapMutation (GPAdfNodeSet& adfNs)
{
#if GPINTERNALCHECK
  if (containerSize()==0)
    GPExitSystem ("GP::swapMutation", "GP contains no trees");
#endif

  // Point to a particular adf genetic tree and get that node set for
  // that tree.  
  int randtree = GPrand() % containerSize ();
  GPNodeSet& ns=*adfNs.NthNodeSet (randtree);

  GPGene* rootGene=NthGene (randtree);
  if (rootGene)
    {
      // Select a gene on that branch, e.g. get the address of the
      // pointer that points to the gene
      GPGene& g=**(rootGene->
		   choose ((GPGene**)getPointerAddress (randtree)));
  
      // First of all we have to work out how many arguments this
      // function (if it is one) has.  There are two ways: Using
      // containerSize() of the Gene or have a look at the node.  The
      // user could have modified the number of children of a gene, so
      // we don't use the node information
      int args=g.containerSize ();

      // Try a certain number of times to find a node with different
      // node identification value than before.
      for (int i=0; i<swapAttempts; i++)
	{
  
	  // Choose a node at random from node set with the same
	  // number of arguments than the gene we chose
	  GPNode* node=ns.chooseNodeWithArgs (args);
	  if (node)
	    {
	      // Check that the number of arguments match the number
	      // of arguments of the function and also check that it
	      // has a different identification value, little point
	      // otherwise.
	      if (node->value() != g.geneNode().value())
		{
		  // replace old function with new one and stop loop
		  g.node=node;
		  break;
		}
	    }
	}
    }
}



// Count the number of function nodes a tree has
int GPGene::countFunctions ()
{
  // We determine whether the current gene is a function or a terminal
  // by the number of children the node has
  int count=(containerSize()>0) ? 1 : 0;

  // Loop through the children
  GPGene* gene;
  for (int n=0; n<containerSize(); n++)
    if ((gene=NthChild (n)))
      count+=gene->countFunctions ();

  return count;
}



// Returns a random function(!) gene selected by random number
// generator.  Obviously it is quite important to know the number of
// functions, which is evaluated using countFunctions().  This
// function here is useful for shrink mutation.  The parameter is the
// adress of a pointer that refers to the root gene.  We also return
// the adress of a pointer to the selected gene.  This makes the
// shrink mutation very simple.  The object this function is called
// for is of no importance, but is usually the root gene.  The
// function returns NULL if no function nodes exists.
GPGene** GPGene::chooseFunctionNode (GPGene** rootPtr)
{
  // Calculate the length of the subtree starting at the given gene
  // and calculate a random number between 1 -> totalFunctions
  int totalFunctions=(**rootPtr).countFunctions ();
  if (totalFunctions>0)
    {
      int randomFunc = (GPrand() % totalFunctions) + 1;

      // Find function gene
      GPGene **pg=findNthNode (rootPtr, 1, randomFunc);
#if GPINTERNALCHECK
      if (!pg)
	GPExitSystem ("GPGene::chooseFunctionNode", 
		      "Didn't find tree node");
#endif

      // Return function node
      return pg;
    }
  else 
    return NULL;
}



// This block of code performs shrink mutation on a genetic program.
// A function node is chosen by random from a random GP tree, and one
// of the children of the function takes the position of the parent.
void GP::shrinkMutation ()
{
#if GPINTERNALCHECK
  if (containerSize()==0)
    GPExitSystem ("GP::swapMutation", "GP contains no trees");
#endif

  // Select a particular adf genetic tree by random
  int randtree = GPrand() % containerSize ();

  // Get root gene
  GPGene* rootGene=NthGene (randtree);
  if (rootGene)
    {
      // Select a function gene on that branch, e.g. get the address
      // of the pointer that points to the gene.
      GPGene** rootGenePtr=(GPGene**)getPointerAddress (randtree);
      GPGene** g=rootGene->chooseFunctionNode (rootGenePtr);

      // If function node exists (it may happen that there is no
      // function node at all)
      if (g)
	{
	  // Choose one subtree (or child) of the chosen function gene
	  int subTree=GPrand() % (**g).containerSize();
	  GPGene* child=(**g).NthChild (subTree);

	  // Set the pointer the parent uses to point to the child to
	  // NULL, so that the child won't be deleted if the parent is
	  // killed.
	  GPGene** childPtr=(GPGene**) (**g).getPointerAddress (subTree);
	  *childPtr=NULL;

	  // Delete parent.
	  delete *g;
      
	  // Put the child on the position of the former parent
	  *g=child;

	  // Recalculate length and depth
	  calcLength ();
	  calcDepth ();
	}
    }
}



// Mutate the GP with the probability given by parameter in the
// GPVariables.  
void GP::mutate (GPVariables& GPVar, GPAdfNodeSet& adfNs)
{
  if (GPRandomPercent (GPVar.SwapMutationProbability))
    {
      swapMutation (adfNs);
      fitnessValid=0;
    }

  if (GPRandomPercent (GPVar.ShrinkMutationProbability))
    {
      shrinkMutation ();
      fitnessValid=0;
    }
}


