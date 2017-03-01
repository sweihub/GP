
// symbreg.cc

/* ---------------------------------------------------------------

Symbolic Regression

An example for how to use gpc++ - The Genetic Programming Kernel

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

Copyright 1996, 1997 Thomas Weinbrenner

For comments, improvements, additions (or even money !?) contact:

Thomas Weinbrenner
Grauensteinstr. 26
35789 Laimbach
Germany
E-mail: thomasw@emk.e-technik.th-darmstadt.de
WWW:    http://www.emk.e-technik.th-darmstadt/~thomasw

--------------------------------------------------------------- */

#include <iostream>
#include <fstream>
#include <strstream>

#include <stdlib.h>
#include <new>    // For the new-handler
#include <math.h>   // fabs()
#include <string.h>

// Include header file of genetic programming system.
#include "gp.h" 
#include "gpconfig.h"

#include "symbreg.h"

using namespace std;

// Define the function to be symbolically regressed and set up
// question and answer array with some points for the function
#define EQUATION(x)  (x*x*x*x + x*x*x + x*x + x)
#define DATAPOINTS 10
double ques[DATAPOINTS];
double answ[DATAPOINTS];

// The TeX-file
ofstream tout;
int printTexStyle=0;

// Define configuration parameters and the neccessary array to
// read/write the configuration to a file. If you need more variables,
// just add them below and insert an entry in the configArray.
GPVariables cfg;
char *InfoFileName="data";
struct GPConfigVarInformation configArray[]=
{
  {"PopulationSize", DATAINT, &cfg.PopulationSize},
  {"NumberOfGenerations", DATAINT, &cfg.NumberOfGenerations},
  {"CreationType", DATAINT, &cfg.CreationType},
  {"CrossoverProbability", DATADOUBLE, &cfg.CrossoverProbability},
  {"CreationProbability", DATADOUBLE, &cfg.CreationProbability},
  {"MaximumDepthForCreation", DATAINT, &cfg.MaximumDepthForCreation},
  {"MaximumDepthForCrossover", DATAINT, &cfg.MaximumDepthForCrossover},
  {"SelectionType", DATAINT, &cfg.SelectionType},
  {"TournamentSize", DATAINT, &cfg.TournamentSize},
  {"DemeticGrouping", DATAINT, &cfg.DemeticGrouping},
  {"DemeSize", DATAINT, &cfg.DemeSize},
  {"DemeticMigProbability", DATADOUBLE, &cfg.DemeticMigProbability},
  {"SwapMutationProbability", DATADOUBLE, &cfg.SwapMutationProbability},
  {"ShrinkMutationProbability", DATADOUBLE, &cfg.ShrinkMutationProbability},
  {"SteadyState", DATAINT, &cfg.SteadyState},
  {"AddBestToNewPopulation", DATAINT, &cfg.AddBestToNewPopulation},
  {"InfoFileName", DATASTRING, &InfoFileName},
  {"", DATAINT, NULL}
};



// Print out a gene in typical math style. Don't be confused, I don't
// make a difference whether this gene is the main program or an ADF,
// I assume the internal structure is correct.
void MyGene::printMathStyle (ostream& os, int lastPrecedence)
{
  int precedence;

  // Function or terminal?
  if (isFunction ())
    {
      // Determine operator priority
      switch (node->value ())
	{
	case '*': 
	case '%': 
	  precedence=1;
	  break;
	case '+': 
	case '-': 
	  precedence=0;
	  break;
	case 'A': 
	  precedence=2;
	  break;
	default:
	  GPExitSystem ("MyGene::printMathStyle", 
			"Undefined function value");
	}

      // Do we need brackets?
      if (lastPrecedence>precedence)
	os << "(";

      // Print out the operator and the parameters 
      switch (node->value ())
	{
	case '*': 
	  NthMyChild(0)->printMathStyle (os, precedence);
	  os << "*";
	  NthMyChild(1)->printMathStyle (os, precedence);
	  break;
	case '+': 
	  NthMyChild(0)->printMathStyle (os, precedence);
	  os << "+";
	  NthMyChild(1)->printMathStyle (os, precedence);
	  break;
	case '-': 
	  NthMyChild(0)->printMathStyle (os, precedence);
	  os << "-";
	  NthMyChild(1)->printMathStyle (os, precedence);
	  break;
	case '%': 
	  NthMyChild(0)->printMathStyle (os, precedence);
	  os << "%";
	  NthMyChild(1)->printMathStyle (os, precedence);
	  break;
	case 'A': 
	  // This is the ADF0-function. We put the parameters in
	  // brackets and start again with precedence 0.
	  os << "ADF0 (";
	  NthMyChild(0)->printMathStyle (os, 0);
	  os << ",";
	  NthMyChild(1)->printMathStyle (os, 0);
	  os << ")";
	  break;
	default: 
	  GPExitSystem ("MyGene::printMathStyle", 
			"Undefined function value");
	}

      // Do we need brackets?
      if (lastPrecedence>precedence)
	os << ")";
    }

  // Print the terminal
  if (isTerminal ())
    os << *node;
}



// Print out a gene in LaTeX-style. Don't be confused, I don't make a
// difference whether this gene is the main program or an ADF, I
// assume the internal structure is correct.
void MyGene::printTeXStyle (ostream& os, int lastPrecedence)
{
  int precedence=0;

  // Function or terminal?
  if (isFunction ())
    {
      // Determine operator priority
      switch (node->value())
	{
	case '*': 
	case '%': 
	  precedence=2;
	  break;
	case '+': 
	case '-': 
	  precedence=1;
	  break;
	case 'A': 
	  precedence=3;
	  break;
	default:
	  GPExitSystem ("MyGene::printTeXStyle", 
			"Undefined function value");
	}

      // Do we need brackets?
      if (lastPrecedence>precedence)
	os << "\\left(";

      // Print out the operator and the parameters 
      switch (node->value())
	{
	case '*': 
	  NthMyChild(0)->printTeXStyle (os, precedence);
	  os << " ";
	  NthMyChild(1)->printTeXStyle (os, precedence);
	  break;
	case '+': 
	  NthMyChild(0)->printTeXStyle (os, precedence);
	  os << "+";
	  NthMyChild(1)->printTeXStyle (os, precedence);
	  break;
	case '-': 
	  NthMyChild(0)->printTeXStyle (os, precedence);
	  os << "-";
	  NthMyChild(1)->printTeXStyle (os, precedence);
	  break;
	case '%': 
	  // As we use \frac, we start again with precedence 0
	  os << "\\frac{";
	  NthMyChild(0)->printTeXStyle (os, 0);
	  os << "}{";
	  NthMyChild(1)->printTeXStyle (os, 0);
	  os << "}";
	  break;
	case 'A': 
	  // This is the ADF0-function. We put the parameters in
	  // brackets and start again with precedence 0.
	  os << "f_2(";
	  NthMyChild(0)->printTeXStyle (os, 0);
	  os << ",";
	  NthMyChild(1)->printTeXStyle (os, 0);
	  os << ")";
	  break;
	default: 
	  GPExitSystem ("MyGene::printTeXStyle", 
			"Undefined function value");
	}

      // Do we need brackets?
      if (lastPrecedence>precedence)
	os << "\\right)";
    }

  // We can't let the terminal print itself, because we want to modify
  // it a little bit
  if (isTerminal ())
    switch (node->value ())
      {
      case 'X': 
	os << "x";
	break;
      case 1:
	os << "x_1";
	break;
      case 2:
	os << "x_2";
	break;
      default: 
	GPExitSystem ("MyGene::printTeXStyle", 
		      "Undefined terminal value");
      }
}



// Print a Gene.
void MyGene::printOn (ostream& os)
{
  if (printTexStyle)
    printTeXStyle (os);
  else
    printMathStyle (os);
}



// Print a GP. If we want a LaTeX-output, we must provide for the
// equation environment, otherwise we simply call the print function
// of our base class.
void MyGP::printOn (ostream& os)
{
  // If we use LaTeX-style, we provide here for the right equation
  // overhead used for LaTeX. 
  if (printTexStyle)
    {
      tout << "\\begin{eqnarray}" << endl;

      // Print all ADF's, if any
      GPGene* current;
      for (int n=0; n<containerSize(); n++)
	{
	  if (n!=0)
	    os << "\\\\" << endl;
	  os << "f_" << n+1 << " & = & ";
	  if ((current=NthGene (n)))
	    os << *current;
	  else
	    os << " NONE";
	  os << "\\nonumber ";
	}
      tout << endl << "\\end{eqnarray}" << endl << endl;
      
    }
  else
    // Just call the print function of our base class to do the
    // standard job.
    GP::printOn (os);
}



// This function is the divide with closure. Basically if you divide
// anything by zero you get an error so we have to stop this
// process. We check for a very small denominator and return a very
// high value.
inline double divide (double x, double y)
{
  if (fabs (y)<1e-6)
    {
      if (x*y<0.0) 
	return -1e6;
      else
	return 1e6;
    }
  else
    return x/y;
}



// We have the freedom to define this function in any way we like. In
// this case, it takes the parameter x that represents the terminal X,
// and returns the value of the expression. It's recursive of course.
double MyGene::evaluate (double x, MyGP& gp, double arg0, double arg1)
{
  double ret, a0, a1;

  if (isFunction ())
    switch (node->value ())
      {
      case '*': 
	ret=NthMyChild(0)->evaluate (x, gp, arg0, arg1)
	  * NthMyChild(1)->evaluate (x, gp, arg0, arg1);
	break;
      case '+': 
	ret=NthMyChild(0)->evaluate (x, gp, arg0, arg1)
	  + NthMyChild(1)->evaluate (x, gp, arg0, arg1);
	break;
      case '-': 
	ret=NthMyChild(0)->evaluate (x, gp, arg0, arg1)
	  - NthMyChild(1)->evaluate (x, gp, arg0, arg1);
	break;
      case '%': 
	// We use the function divide rather than "/" to ensure the
	// closure property
	ret=divide (NthMyChild(0)->evaluate (x, gp, arg0, arg1), 
		    NthMyChild(1)->evaluate (x, gp, arg0, arg1));
	break;
      case 'A': 
	// This is the ADF0 function call.  We have access to that
	// subtree, as the GP gave us a reference to itself as
	// parameter.  We first evaluate the subtrees, and then call
	// the adf with the parameters
	a0=NthMyChild(0)->evaluate (x, gp, arg0, arg1);
	a1=NthMyChild(1)->evaluate (x, gp, arg0, arg1);
	ret=gp.NthMyGene(1)->evaluate (x, gp, a0, a1);
	break;
      default: 
	GPExitSystem ("MyGene::evaluate", "Undefined function value");
      }
  if (isTerminal ())
    switch (node->value ())
      {
      case 'X': 
	ret=x;
	break;
      case 1:
	ret=arg0;
	break;
      case 2:
	ret=arg1;
	break;
      default: 
	GPExitSystem ("MyGene::evaluate", "Undefined terminal value");
      }

  // Restrict the return value (it may become really large, especially
  // for large trees)
  const double maxValue=1e6;
  if (ret>maxValue)
    return maxValue;
  if (ret<-maxValue)
    return -maxValue;

  return ret;
}



// Evaluate the fitness of a GP and save it into the GP class variable
// stdFitness.
void MyGP::evaluate ()
{
  double rawfitness=0.0, diff=0.0;

  // The evaluation function checks with some values of the
  // mathematical function
  for (int i=0; i<DATAPOINTS; i++)
    {
      // Calculate genetic programs answer.  ques[i] is the current x
      // value of the function.
      double result=NthMyGene(0)->evaluate (ques[i], *this, 0, 0);

      // Calculate square error between the genetic program and the
      // actual answer.
      diff=pow (answ[i]-result, 2);

      // Add this difference to total rawfitness
      rawfitness+=diff;
    }
  rawfitness=sqrt (rawfitness);

  // We want a short solution and put the total GP length to the
  // fitness.  As we know that the best solution has the length 12, we
  // create such a formula that the fitness will be 1.0 in this case.
  // This standardized fitness is saved in the GP object.
  stdFitness=rawfitness+(double)length ()/12.0;
}



// Create function and terminal set
void createNodeSet (GPAdfNodeSet& adfNs)
{
  // Reserve space for the node sets
  adfNs.reserveSpace (2);

  // Now define the function and terminal set for each ADF and place
  // function/terminal sets into overall ADF container
  GPNodeSet& ns0=*new GPNodeSet (6);
  GPNodeSet& ns1=*new GPNodeSet (4);
  adfNs.put (0, ns0);
  adfNs.put (1, ns1);

  // Define functions/terminals and place them into the appropriate
  // sets. Terminals take two arguments, functions three (the third
  // parameter is the number of arguments the function has)
  ns0.putNode (*new GPNode ('+', "+", 2));
  ns0.putNode (*new GPNode ('-', "-", 2));
  ns0.putNode (*new GPNode ('*', "*", 2));
  ns0.putNode (*new GPNode ('%', "%", 2));
  ns0.putNode (*new GPNode ('A', "ADF0", 2));
  ns0.putNode (*new GPNode ('X', "x"));

  ns1.putNode (*new GPNode ('+', "+", 2));
  ns1.putNode (*new GPNode ('*', "*", 2));
  ns1.putNode (*new GPNode (1, "x1"));
  ns1.putNode (*new GPNode (2, "x2"));
}



void newHandler ()
{
  cerr << "\nFatal error: Out of memory." << endl;
  exit (1);
}



int main ()
{
  // We set up a new-handler, because we might need a lot of memory,
  // and we don't know it's there.
  set_new_handler (newHandler);

  // Set up the array with data points. This is for the evaluation
  // process.
  for (int i=0; i<DATAPOINTS; i++)
    {
      ques[i]=i;
      answ[i]=EQUATION(ques[i]);
    }

  // Init GP system.
  GPInit (1, -1);

  // Declare the GP Variables, set defaults and read configuration
  // file.  The defaults will be overwritten by the configuration file
  // when read.  If it doesn't exist, the defaults will be written to
  // the file.
  GPConfiguration config (cout, "symbreg.ini", configArray);

  // Open the main output file for data and statistics file. First set
  // up names for data file. We use also a TeX-file where the
  // equations are written to in TeX-style. Very nice to look at!
  // Remember we should delete the string from the stream, well just a
  // few bytes
  ostrstream strOutFile, strStatFile, strTeXFile;
  strOutFile  << InfoFileName << ".dat" << ends;
  strStatFile << InfoFileName << ".stc" << ends;
  strTeXFile  << InfoFileName << ".tex" << ends;
  ofstream fout (strOutFile.str());
  ofstream bout (strStatFile.str());
  tout.open (strTeXFile.str(), ios::out);
  tout << endl
       << "\\documentstyle[a4]{article}" << endl
       << "\\begin{document}" << endl;
  
  // Print the configuration to the files just opened
  fout << cfg << endl;
  cout << cfg << endl;
  tout << "\\begin{verbatim}\n" << cfg << "\\end{verbatim}\n" << endl;
  
  // Create the adf function/terminal set and print it out.
  GPAdfNodeSet adfNs;
  createNodeSet (adfNs);
  cout << adfNs << endl; 
  fout << adfNs << endl;  

  // Create a population with this configuration
  cout << "Creating initial population ..." << endl;
  MyPopulation* pop=new MyPopulation (cfg, adfNs);
  pop->create ();
  cout << "Ok." << endl;
  pop->createGenerationReport (1, 0, fout, bout);

  // Print the best of generation to the LaTeX-file.
  printTexStyle=1;
  tout << *pop->NthGP (pop->bestOfPopulation);
  printTexStyle=0;

  // This next for statement is the actual genetic programming system
  // which is in essence just repeated reproduction and crossover loop
  // through all the generations .....
  MyPopulation* newPop=NULL;
  for (int gen=1; gen<=cfg.NumberOfGenerations; gen++)
    {
      // Create a new generation from the old one by applying the
      // genetic operators
      if (!cfg.SteadyState)
	newPop=new MyPopulation (cfg, adfNs);
      pop->generate (*newPop);
      
      // Delete the old generation and make the new the old one
      if (!cfg.SteadyState)
	{
	  delete pop;
	  pop=newPop;
	}

      // Print the best of generation to the LaTeX-file.
      printTexStyle=1;
      tout << "Generation " << gen << ", fitness " 
	   << pop->NthGP (pop->bestOfPopulation)->getFitness()
	   << endl;
      tout << *pop->NthGP (pop->bestOfPopulation);
      printTexStyle=0;

      // Create a report of this generation and how well it is doing
      pop->createGenerationReport (0, gen, fout, bout);
    }

  // TeX-file: end of document
  tout << endl
       << "\\end{document}"
       << endl;
  tout.close ();

  cout << "\nResults are in " 
       << InfoFileName << ".dat," 
       << InfoFileName << ".tex,"
       << InfoFileName << ".stc." << endl;

  return 0;
}
