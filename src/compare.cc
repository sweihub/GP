
// compare.cc

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

// Used to guarantee 100% diversity in population in the beginning of
// a run

#include "gp.h"



// Compare two gps together.  Returning a 0 if they are the same
int GP::compare (GP& gp)
{
#if GPINTERNALCHECK
  // Well both GPs should have the same number of ADFs
  if (containerSize()!=gp.containerSize())
    GPExitSystem ("GP::compare", "Number of ADFs differ");
#endif

  // Loop through all subtrees and compare them
  for (int n=0; n<containerSize(); n++)
    {
      GPGene *c1, *c2;

      // Get pointer to genes
      c1=NthGene (n);
      c2=gp.NthGene (n);

      // Compare them
      if (c1 && c2)
	{
	  // If they are not same, return immediately
	  if (c1->compare (*c2))
	    return 1;
	}
      else 
	// If one of them is not NULL, return immediately
	if (c1 || c2)
	  return 1;
    }

  // Seem to be the same
  return 0;
}



int operator == (GPGene& g1, GPGene& g2)
{
  return g1.compare (g2)==0;
}



// Comparison of a genetic tree: Returns a 1 if different, 0 if the
// same.  If the pointers to the nodes are the same, we think they are
// same indeed
int GPGene::compare (GPGene &g)
{
  // If the pointers to the nodes are the same, we think they are same
  // indeed, but we have to check their children first.  If the
  // pointers differ, return immediately
  if (node != g.node)
    return 1;

  // Test the number of children.  It would be odd if it would differ
  // here, as the nodes are the same, but it can, if the user changed
  // the number of parameters by hand
  if (containerSize() != g.containerSize())
    return 1;

  // Now compare all children.  
  for (int n=0; n<containerSize(); n++)
    {
      // Get pointer to children
      GPGene *c1=NthChild (n);
      GPGene *c2=g.NthChild (n);

      // Compare them
      if (c1 && c2)
	{
	  // If they are not same, return immediately
	  if (c1->compare (*c2))
	    return 1;
	}
      else 
	// If one of them is not NULL, return immediately
	if (c1 || c2)
	  return 1;
    }

  // Seem to be the same
  return 0;
}


