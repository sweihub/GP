
// gprand.cc

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

// Adapted from ``Numerical Recipes in C'' (1992) by Press, Flannery,
// Teukolsky and Vetterling.  This is a great book indeed, and you
// should have at least a look at it!

#include "gp.h"



// Seed for random generator
static long idum=-1;

#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (IM-1)

// This routine is not Multi-Thread-save because a global variable is
// used.  Returns a random number in range 0..RNMX.
long GPrand ()
{
  int j;
  long k;
  static long iy=0;
  static long iv[NTAB];

  if (idum <= 0 || !iy) 
    {
      if (-(idum) < 1) 
	idum=1;
      else 
	idum = -(idum);
      for (j=NTAB+7;j>=0;j--) 
	{
	  k=(idum)/IQ;
	  idum=IA*(idum-k*IQ)-IR*k;
	  if (idum < 0) 
	    idum += IM;
	  if (j < NTAB) 
	    iv[j] = idum;
	}
      iy=iv[0];
    }
  k=(idum)/IQ;
  idum=IA*(idum-k*IQ)-IR*k;
  if (idum < 0) 
    idum += IM;
  j=iy/NDIV;
  iy=iv[j];
  iv[j] = idum;
  if (iy > RNMX) 
    return RNMX;
  else 
    return iy;
}

#undef IA
#undef IM
#undef AM
#undef IQ
#undef IR
#undef NTAB
#undef NDIV
#undef EPS
#undef RNMX



// Seed random generator
void GPsrand (long seed)
{
  idum=-seed;
}



// Returns either 0 or 1 by random.  The probability is given by the
// argument in the range 0..100.0 (which is meant to be percent).  The
// resolution of the argument can be as low as 0.0001.
int GPRandomPercent (double percent)
{
  return (double) (GPrand () % 1000000l) < percent*10000.0;
}
