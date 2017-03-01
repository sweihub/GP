
// symbreg.h

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

// We are now inheriting the classes we want to change. These are the
// three classes GPGene, GP and GPPopulation.  Read the documentation
// for what has to be done here.  Remember it makes not a lot of sense
// to implement Load/Save here, otherwise we should definitely have to
// provide for the isA(), load(), save() and createObject() functions.

class MyGP;

class MyGene : public GPGene
{
public:
  MyGene (GPNode& gpo) : GPGene (gpo) {}

  MyGene (const MyGene& gpo) : GPGene (gpo) { }
  virtual GPObject& duplicate () { return *(new MyGene(*this)); }
  virtual GPGene* createChild (GPNode& gpo) {
    return new MyGene (gpo); }

  virtual void printOn (ostream& os);
  void printMathStyle (ostream& os, int lastPrecedence=0);
  void printTeXStyle (ostream& os, int lastPrecedence=0);

  MyGene* NthMyChild (int n) {
    return (MyGene*) GPContainer::Nth (n); }

  double evaluate (double x, MyGP& gp, double arg0, double arg1);
};



class MyGP : public GP 
{
public:
  MyGP (int genes) : GP (genes) {}

  MyGP (MyGP& gpo) : GP (gpo) { }
  virtual GPObject& duplicate () { return *(new MyGP(*this)); }
  virtual GPGene* createGene (GPNode& gpo) {
    return new MyGene (gpo); }

  virtual void printOn (ostream& os);

  MyGene* NthMyGene (int n) {
    return (MyGene*) GPContainer::Nth (n); }
  virtual void evaluate ();
};



class MyPopulation : public GPPopulation
{
public:
  MyPopulation (GPVariables& GPVar_, GPAdfNodeSet& adfNs_) : 
    GPPopulation (GPVar_, adfNs_) {}

  MyPopulation (MyPopulation& gpo) : GPPopulation(gpo) {}
  virtual GPObject& duplicate () { return *(new MyPopulation(*this)); }

  // Don't check for ultimate diversity as it takes a long time.
  // Accept every created GP
  virtual int checkForValidCreation (GP&) { return 1; }

  virtual GP* createGP (int numOfGenes) { return new MyGP (numOfGenes); }
};


