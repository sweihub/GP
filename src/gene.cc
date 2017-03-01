
// gene.cc

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

// The base class of the genetic program is the gene which is contains
// the value of the gene and pointers to further genes within the
// program



#include <stdlib.h>

#include "gp.h"



// Print function
void GPGene::printOn (ostream& os)
{
  if (node->isFunction ())
    os << "(";
  os << *node;

  // Print all children, if any
  for (int n=0; n<containerSize(); n++)
    {
      GPGene* current=NthChild (n);

      os << ' ';
      if (current)
	os << *current;
      else
	os << "(NULL)";
    }

  if (node->isFunction ())
    os << ")";
}



// Calculates the length of a genetic program or subtree (count all
// nodes) from this point on
int GPGene::length ()
{
  int lengthSoFar=1;   // Thats me!

  // Do same for all children, if there are any, and add up length
  GPGene* current;
  for (int n=0; n<containerSize(); n++)
    if ((current=NthChild (n)))
      lengthSoFar+=current->length ();

  // Return the length
  return lengthSoFar;
}



// Calculates the depth of a genetic program or subtree from this
// point on.  Please call from outside without parameters as the
// default value of the parameter is meaningful!
int GPGene::depth (int depthSoFar)
{
  // Calc depth for all children.  If it turns out to be greater than
  // depthSoFar, save the value and return it later
  GPGene* current;
  int maxDepthOfChild=depthSoFar;
  for (int n=0; n<containerSize(); n++)
    if ((current=NthChild (n)))
      {
	int d=current->depth (depthSoFar+1);
	if (d>maxDepthOfChild)
	  maxDepthOfChild=d;
      }

  return maxDepthOfChild;
}



// Returns the address of the pointer to the Nth function gene of the
// subtree.  This function is useful for crossover and shrink
// mutation.  The flag findFunction determines whether only function
// nodes (shrink mutation) are searched or just any node type
// (crossover).
GPGene** GPGene::findNthNode (GPGene** rootPtr, int findFunction,
			      int &iLengthCount)
{
  // if we are looking for any node, or if we are looking just for
  // function nodes and this happens to be one, then ...
  if (!findFunction 
      || (findFunction && (**rootPtr).containerSize()>0))
    {
      // ... decrement the length counter and return if it is zero
      if (--iLengthCount<=0)
	return rootPtr;
    }

  // Do the same for all children
  for (int n=0; n<(**rootPtr).containerSize(); n++)
    {
      // Get pointer to pointer of n-th child
      GPGene** childPtr=(GPGene**) (**rootPtr).getPointerAddress (n);

      // If child exists
      if (*childPtr)
	{
	  // Recursive call for the children
	  GPGene** found=
	    (**childPtr).findNthNode (childPtr, findFunction, 
				      iLengthCount);

	  // If we found it, return immediately
	  if (found) 
	    return found;
	}
    }

  // We didn't find the correct one so far
  return NULL;
}



// Resolve the pointers to the nodes
void GPGene::resolveNodeValues (GPNodeSet& ns)
{
  // Transform the node value to an address to the appropriate node
  node=ns.searchForNode (nodeValue);
  if (!node)
    GPExitSystem ("GPGene::resolveNodeValues", 
		  "Node value not in node set");

  // Same for all children
  GPGene* current;
  for (int n=0; n<containerSize(); n++)
    if ((current=NthChild (n)))
      current->resolveNodeValues (ns);
}



// Load operation
char* GPGene::load (istream& is)
{
  // We can load only the node's value.  This has to be transformed to
  // a pointer to a node by function resolveNodeValues().
  is >> nodeValue;

  // Load container
  return GPContainer::load (is);
}



// Save operation
void GPGene::save (ostream& os)
{
  // Save node value
  os << node->value() << ' ';

  // Save container
  GPContainer::save (os);
}
