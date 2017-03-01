
// config.cc

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

/*

This code is provided just for your convenience. 

It provides an easy and simple to use way to define constants in a
.ini-file, which are read in by the constructor of this class.

Simply call the constructor with the stream to put any messages on
(only for error messages, in case the file is malformed), the
initialisation file name, and an array of structures describing each
variable.  The array must end with a NULL pointer in the structure
variable varPtr.  The constructor reads the file and puts all the
values of the variables he finds in the denoted memory location.
Strings are duplicated; space is allocated with new char[] and should
be freed, if no longer used.  In the case of a configuration variable
of type string, the structure variable varPtr points to a (char *),
that's better than providing space for yourself, because you needn't
worry you reserved enough space for your variable.  The structure must
be global, because a pointer to the structure is saved to be used for
other routines as well.

Example:

  char *measuredOutputDataFile="pt2.dat";
  double startTemperature=1.0;
  int numberOfIterations=10;

  // Define configuration parameters
  struct GPConfigVarInformation configArray[]=
  {
    {"numberOfIterations", DATAINT, &numberOfIterations},
    {"startTemperature", DATADOUBLE, &startTemperature},
    {"measuredOutputDataFile", DATASTRING, &measuredOutputDataFile},
    {"", 0, NULL}
  };

  // Read in configuration file
  configuration config (cout, "main.ini", configArray);

Thomas Weinbrenner 1996, 1997

*/

#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gpconfig.h"


using namespace std;


GPConfiguration::GPConfiguration (ostream &out, char *fname,
				  struct GPConfigVarInformation cfg[])
  // Constructor: Read in configuration file.
{
  char line[1000];
  int lineNum=0;
  char *s;

  // We save a pointer to the describing structure, if we need it
  // later in other routines. This is quick&dirty, but well in this
  // case I say use your own code if you think so
  saveStruc=cfg;

  // Open file
  ifstream f (fname);
  if (!f)
    {
      out << "Can't read configuration file " << fname 
	  << ", writing default values to same file" << endl;
      ofstream fo (fname);
      fo << *this;
      return;
    }

  // Process lines of the file
  while (!f==0)
    {
      // Get line from the file 
      f.getline (line, sizeof(line)-1);
      line[sizeof(line)-1]='\0';
      lineNum++;

      // Kill whitespaces at end of line
      char *last=line+strlen (line)-1;
      while (last>=line && isspace (*last))
	{
	  *last='\0';
	  last--;
	}

      // Check for comment line or empty line
      if (line[0]=='#' || strlen (line)==0)
	continue;

      // Search for '='
      char *search=strchr (line, '=');
      if (!search)
	{
	  out << "Missing = in line " << lineNum << endl;
	  continue;
	}
      *search='\0';
      
      // Kill whitespaces and check for correct value
      last=search-1;
      search++;
      while (isspace (*search))
	search++;
      while (last>=line && isspace (*last))
	{
	  *last='\0';
	  last--;
	}
      if (strlen (line)==0)
	{
	  out << "No variable name given in line " << lineNum << endl;
	  continue;
	}
      if (strlen (search)==0)
	{
	  out << "No value given in line " << lineNum << endl;
	  continue;
	}
      
      // Search for variable name in configuration array. Don't report
      // an error, if not found there.  This is probably done by
      // purpose.
      int found=0;
      for (int i=0; cfg[i].varPtr!=NULL; i++)
	{
	  if (strcmp (line, cfg[i].name)==0)
	    {
	      found=1;
	      switch (cfg[i].typ)
		{
		case DATAFLOAT:
		  *(float *)cfg[i].varPtr=(float) atof (search);
		  break;
		case DATADOUBLE:
		  *(double *)cfg[i].varPtr=(double) atof (search);
		  break;
		case DATAINT:
		  *(int *)cfg[i].varPtr=(int) atoi (search);
		  break;
		case DATASTRING:
		  s=new char [strlen(search)+1];
		  strcpy (s, search);
		  * ((char **)cfg[i].varPtr)=s;
		  break;
		default:
		  out << "Unknown data type in internal structure" 
		      << endl;
		}
	      break;
	    }
	}
    }
}



GPConfiguration::~GPConfiguration ()
  // Destructor
{
}



void GPConfiguration::printOn (ostream& o) const
  // Print all configuration variables to o
{
  o << "# Configuration (default values), created by class configuration)\n";

  for (int i=0; saveStruc[i].varPtr!=NULL; i++)
    {
      o << saveStruc[i].name;
      for (int j=strlen (saveStruc[i].name); j<31; j++)
	o << ' ';
      o << " = ";
      switch (saveStruc[i].typ)
	{
	case DATAFLOAT:
	  o << *(float *)saveStruc[i].varPtr;
	  break;
	case DATADOUBLE:
	  o << *(double *)saveStruc[i].varPtr;
	  break;
	case DATAINT:
	  o << *(int *)saveStruc[i].varPtr;
	  break;
	case DATASTRING:
	  o << *(char **)saveStruc[i].varPtr;
	  break;
	default:
	  o << "Unknown data type in internal structure\n";
	}
      o << endl;
    }
}

