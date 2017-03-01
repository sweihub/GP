
// gp.cc

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

// Genetic Program definitions


#include <iostream>
#include "gp.h"

using namespace std;


// Simply print out the first Gene and then all ADF's.
void GP::printOn (ostream& os)
{
  // Print all ADF's, if any
  GPGene* current;
  for (int n=0; n<containerSize(); n++)
    {
      if (n==0)
	os << "GP: ";
      else
	os << "ADF" << n-1 << ": ";
      if ((current=NthGene (n)))
	os << *current;
      else
	os << " NONE";
      os << endl;
    }
}



// Calculates length of a genetic program.  Length is, of course, also
// the structural complexity of the system but length is so much
// shorter to type and reduces scientific linguistic complexity.  It
// simply add all lengths of the main program and all ADFs and saves
// the result in the object variable.
void GP::calcLength ()
{
  // Loop through main program and all ADF's, if any
  int len=0;
  GPGene* current;
  for (int n=0; n<containerSize(); n++)
    if ((current=NthGene (n)))
      len+=current->length ();

  GPlength=len;
}



// The depth is the maximum depth of all subtrees. 
void GP::calcDepth ()
{
  // Loop through main program and all ADF's, if any
  GPGene* current;
  int maxDepthAll=0;
  for (int n=0; n<containerSize(); n++)
    if ((current=NthGene (n)))
      {
	// Depth
	int imaxdepth=current->depth ();
	if (imaxdepth>maxDepthAll) 
	  maxDepthAll=imaxdepth;
      }

  GPdepth=maxDepthAll;
}



// Resolve the pointers to the nodes, for each tree
void GP::resolveNodeValues (GPAdfNodeSet& adfNs)
{
  GPGene* current;
  for (int n=0; n<containerSize(); n++)
    if ((current=NthGene (n)))
      current->resolveNodeValues (*adfNs.NthNodeSet (n));
}



// Load operation
char* GP::load (istream& is)
{
  // Load variables
  is >> fitnessValid;
  is >> stdFitness;

  // Load container
  char *errMsg=GPContainer::load (is);
  if (errMsg) return errMsg;

  // We didn't save the length and depth as we can calculate them now.
  // Anyway, it's safer.
  calcLength ();
  calcDepth ();

  // Return NULL
  return errMsg;
}



// Save operation
void GP::save (ostream& os)
{
  // Save variables.  Length and depth are not saved because it's no
  // problem to calculate them.
  os << fitnessValid << ' ';
  os << stdFitness << ' ';

  // Save container
  GPContainer::save (os);
}
