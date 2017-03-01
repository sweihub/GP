
// lawn.h

/* ---------------------------------------------------------------

The Lawnmower Problem

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

#ifndef __LAWN
#define __LAWN

// The size of the lawn. Feel free to change it. Values of 64x64 are
// not impossible, but you have to wait a bit for the results...
#define LawnHorizontal 8
#define LawnVertical   8

// Identifiers for functions and terminals
const int LEFT=0;   // Turn left
const int MOW=1;    // Move forward and mow
const int RV=2;     // Random vector. Koza used the name RV8
const int ADF0=3;   // ADF function calls
const int ADF1=4;
const int FROG=5;   // Jump and mow there
const int VA=6;     // Vector addition. Koza used the name VA8 
const int PROGN=7;  // Execute both arguments, return result from second
const int ARG0=8;   // Argument for any ADF tree



// The return types in this problem are all vectors (x,y)
class Vector 
{
public:
  Vector () : x(0), y(0) {}
  Vector (int x_, int y_) : x(x_), y(y_) {}

  int x, y;
};

// Vector addition. The world is a toriodal world, so if the lawn
// moves out to the right side it comes in left again
inline Vector operator + (Vector v1, Vector v2)
{
  return Vector ((v1.x+v2.x) % LawnHorizontal, 
		 (v1.y+v2.y) % LawnVertical);
}

// Print a vector
inline ostream& operator << (ostream &os, Vector &v)
{
  return os << v.x << ',' << v.y;
}



// A class for our mower on his lawn
class Mower
{
public:
  void reset ();

  void left ();
  void frog (Vector& v);
  void mow ();

  void mowArea ();

  friend ostream& operator << (ostream &os, Mower &mower);

  // The lawn the mower has to mow
  unsigned char lawn[LawnHorizontal][LawnVertical];

  // Current position of mower
  Vector pos;

  // Direction the mower is facing (value 0-3) and how many areas are
  // already mown
  int direction, mown;
};



// We are now inheriting the classes we want to change.  These are
// usually the class GPGene, GP and GPPopulation.  Read the
// documentation for what has to be done when inheriting from the GP
// classes.  Load and save is not implemented.

class MyGP;

class MyGene : public GPGene
{
public:
  // The constructor
  MyGene (GPNode& gpo) : GPGene (gpo) { valueAssigned=0; }

  // Copy constructor and the function duplicate used by the container
  // class
  MyGene (const MyGene& gpo) : GPGene (gpo) { 
    valueAssigned=gpo.valueAssigned;
    randomConstant=gpo.randomConstant; }
  virtual GPObject& duplicate () { return *(new MyGene(*this)); }

  // We have to overwrite this function to create the objects of our
  // own class MyGene
  virtual GPGene* createChild (GPNode& gpo) {
    return new MyGene (gpo); }

  // Just an easy way to access the children of the node
  MyGene* NthMyChild (int n) {
    return (MyGene*) GPContainer::Nth (n); }

  virtual void printOn (ostream& os);

  Vector evaluate (MyGP& gp, Vector arg0);

  // If this node is a real constant, we need information what constant 
  // and whether it is already assigned a random value
  int valueAssigned; 
  Vector randomConstant;
};



class MyGP : public GP 
{
public:
  // The constructor
  MyGP (int genes) : GP (genes) { mown=0.0; }

  // Copy constructor and the function duplicate used by the container
  // class
  MyGP (MyGP& gpo) : GP (gpo) { mown=gpo.mown; }
  virtual GPObject& duplicate () { return *(new MyGP(*this)); }

  // We have to overwrite this function to create the objects of our
  // own class MyGene
  virtual GPGene* createGene (GPNode& gpo) {
    return new MyGene (gpo); }

  // Just an easy way to access the trees of the GP
  MyGene* NthMyGene (int n) {
    return (MyGene*) GPContainer::Nth (n); }

  virtual void evaluate ();

  // The percentage mown by the GP
  double mown;
};



class MyPopulation : public GPPopulation
{
public:
  // The constructor
  MyPopulation (GPVariables& GPVar_, GPAdfNodeSet& adfNs_) : 
    GPPopulation (GPVar_, adfNs_) {}

  // Copy constructor and the function duplicate used by the container
  // class
  MyPopulation (MyPopulation& gpo) : GPPopulation(gpo) {}
  virtual GPObject& duplicate () { return *(new MyPopulation(*this)); }

  // Don't check for ultimate diversity as it takes a long time.
  // Accept every created GP
  virtual int checkForValidCreation (GP&) { return 1; }

  // Just an easy way to access the GPs of the population
  MyGP* NthMyGP (int n) {
    return (MyGP*) GPContainer::Nth (n); }

  // We have to overwrite this function to create the objects of our
  // own GP class MyGP
  virtual GP* createGP (int numOfGenes) { return new MyGP (numOfGenes); }
};

#endif


