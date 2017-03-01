
// gpconfig.h

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

#ifndef __GPCONFIG_H
#define __GPCONFIG_H

#include <iostream>

using std::ostream;

enum GPDataType {DATAINT=1, DATAFLOAT, DATADOUBLE, DATASTRING};

struct GPConfigVarInformation {
  char *name;
  enum GPDataType typ;
  void *varPtr;
};

class GPConfiguration
{
public:
  GPConfiguration (ostream &out, char *fname,
		   struct GPConfigVarInformation cfg[]);
  ~GPConfiguration ();

  // Prototype to write the configuration variables to ostream 
  friend inline ostream& operator << (ostream& o, const GPConfiguration& c)
  {
    c.printOn (o);
    return o;
  }

private:
  struct GPConfigVarInformation* saveStruc;
  void printOn (ostream& o) const;
};

#endif

