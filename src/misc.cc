
// misc.cc

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

// Simply an exit system, which outputs some error

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include "gp.h"

using namespace std;

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif



// Version number
char *GPVersion="0.5.2";



// Print copyright message
void GPPrintCopyright (ostream& os)
{
  os << "Genetic Programming Kernel in C++ (gpc++) version " 
     << GPVersion << 
    "\nCopyright (C) 1993,1994 Adam Fraser, 1996,1997 Thomas Weinbrenner\n" \
    "gpc++ comes with ABSOLUTELY NO WARRANTY. This is free software,\n" \
    "and you are welcome to redistribute it under certain conditions.\n" \
    "See the GNU General Public Licence for details."
     << endl;
}



// A nice way of exiting when an error occurs.  We don't check for out
// of memory during the cout function call!  An error occurs (or
// rather should occur) only if the user gives us wrong parameter
// values etc., but on some rare occasions an error indicates an
// internal program error.
void GPExitSystem (char *functionName, char *errorMessage)
{
  cerr << "\n\nThe GP Kernel reports an error:\n" 
       << "===> In Function \"" << functionName << "\"" << endl
       << "===> \"" << errorMessage << "\"" 
       << "\n";
  GPPrintCopyright (cerr);

#if GPCREATE_SEGMENTATIONFAULT_ON_ERROR
  cerr << "\nCreating segmentation fault for debugging..." << endl;
  int* g=0;
  cerr << *g;
#endif

  exit (EXIT_FAILURE);
}



// Initialize GP programming system.  If seedRandomGenerator==-1, seed
// with the value of time(), otherwise with the value of
// seedRandomGenerator
void GPInit (int printCopyright, long seedRandomGenerator)
{
  // Intro screen
  if (printCopyright)
    GPPrintCopyright (cout);

  // seeds random numbers
  if (seedRandomGenerator==-1)
    GPsrand (time (NULL));
  else
    GPsrand (seedRandomGenerator);

  // Register kernel classes for load/save
  GPRegisterKernelClasses ();
}
