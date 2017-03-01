
// eval.cc

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

#include "gp.h"



// This function is called after a new generation has been created.
// It is alsso called after each new generation has been evolved using
// crossover, creation or reproduction, but only if steady state is
// not used.  It loops throupgh the population and evaluates the
// fitness of each genetic program.
void GPPopulation::evaluate ()
{
  // loop through whole population evaluating every GP
  for (int n=0; n<containerSize(); n++)
    {
      GP* current=NthGP (n);
#if GPINTERNALCHECK
      if (!current)
	GPExitSystem ("GPPopulation::evaluate", 
		      "Member of population is NULL");
#endif

      // If the evaluation is still valid, don't evaluate it again
      if (!current->fitnessValid)
	{
	  // Evaluate genetic program
	  current->evaluate ();

	  current->fitnessValid=1;
	}
    }
}



// Evaluation for a genetic program.  The user must provide for this
// function!  We don't know how to do this.  The user should inherit
// the GP class and rewrite this function.  He should then put the
// result of the evaluation into the class variable stdFitness.
void GP::evaluate ()
{
  stdFitness=0.0;

#if GPINTERNALCHECK
  GPExitSystem ("GP::Evaluate", 
		"Can't evaluate program, user must so this"); 
#endif
}



// Evaluation for a gene.  We don't define this function in the
// kernel!  Anyway, we don't know how to evaluate a gene.  Another
// reason is that the return value may be anything from int, double,
// or a class variable.  We allow for maximum user freedom.

#ifdef PURPOSELY_NOT_DEFINED
double GPGene::evaluate ()
{
  GPExitSystem ("GPGene::Evaluate", 
		"Can't evaluate gene, user must so this"); 
}
#endif
