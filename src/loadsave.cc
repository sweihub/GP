
// loadsave.cc

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



#define MAXIMUMCLASSNUM 30   // Hope it won't be more. Otherwise change it
static int ids[MAXIMUMCLASSNUM];
static GPObject* loadSaveObjects[MAXIMUMCLASSNUM];
static int registered=0;



// Create an object and return it.  If the ID is not registered,
// report an error
GPObject* GPCreateRegisteredClassObject (int ID)
{
  // Loop through all registered objects
  for (int i=0; i<registered; i++)
    {
      if (ids[i]==ID)
	// Found!  Now call the virtual function createObject() to
	// create an object of same class.
	return loadSaveObjects[i]->createObject ();
    }

#if GPINTERNALCHECK
  GPExitSystem ("GPCreateRegisteredClassObject", 
		"Can't create object of class not registered");
#endif
  return NULL;
}



// Register a single class
void GPRegisterClass (GPObject* gpo)
{
#if GPINTERNALCHECK
  if (registered==MAXIMUMCLASSNUM)
    GPExitSystem ("GPRegisterClass", 
		  "No more classes allowed. Increase MAXIMUMCLASSNUM");
#endif

  // Save ID and pointer to object
  ids[registered]=gpo->isA();
  loadSaveObjects[registered++]=gpo;
}



// Register all classes of GP kernel
void GPRegisterKernelClasses ()
{
  GPRegisterClass (new GPContainer());
  GPRegisterClass (new GPNode());
  GPRegisterClass (new GPNodeSet());
  GPRegisterClass (new GPAdfNodeSet());
  GPRegisterClass (new GPVariables());
  GPRegisterClass (new GPGene());
  GPRegisterClass (new GP());
  GPRegisterClass (new GPPopulation());
}
