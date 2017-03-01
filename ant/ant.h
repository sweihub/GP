
// ant.h

/* ---------------------------------------------------------------

The Artificial Ant Problem

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

For comments, improvements, additions (or even money) contact:

Thomas Weinbrenner
Grauensteinstr. 26
35789 Laimbach
Germany
E-mail: thomasw@emk.e-technik.th-darmstadt.de
WWW:    http://www.emk.e-technik.th-darmstadt/~thomasw

--------------------------------------------------------------- */

// Identifiers for functions and terminals
const int LEFT=0;        // Turn left
const int RIGHT=1;       // Turn right
const int FORWARD=2;     // Move forward
const int IFFOODAHEAD=3; // If food is ahead
const int PROG2=4;       // Execute two arguments, return sum
const int PROG3=5;       // Execute three arguments, return sum

// What character in the trail file means what?
const char NOTHING='.';
const char FOOD='X';
const char GAP='O';

// The size of the trail
const int worldHorizontal=32;
const int worldVertical=32;



// A structure to hold the position of the ant
struct position {
  int x, y;
};



// A class for our ant on the trail
class Ant
{
public:
  void readTrail (char *fname);

  void reset (int energy_);

  void left ();
  void right ();
  int forward ();
  position forwardPosition ();

  int isFoodAhead ();

  friend ostream& operator << (ostream &os, Ant &ant);

  // The world the ant is moving in
  unsigned char world [worldHorizontal][worldVertical];
  unsigned char constantWorld [worldHorizontal][worldVertical];

  // Current position of ant
  position pos;

  // Direction the ant is facing (value 0-3) and energy left to move
  int direction, energy;

  // The maximum amount of food (or fitness) that can be reached
  int maxFood;
};



// We are now inheriting the classes we want to change.  These are
// usually the class GPGene, GP and GPPopulation.  Read the GP
// documentation for what has to be done when inheriting from the GP
// classes.  Load and save is not implemented.

class MyGP;

class MyGene : public GPGene
{
public:
  // The constructor
  MyGene (GPNode& gpo) : GPGene (gpo) { }

  // Copy constructor and the function duplicate used by the container
  // class
  MyGene (const MyGene& gpo) : GPGene (gpo) { }
  virtual GPObject& duplicate () { return *(new MyGene(*this)); }

  // We have to overwrite this function to create the objects of our
  // own class MyGene
  virtual GPGene* createChild (GPNode& gpo) {
    return new MyGene (gpo); }

  // Just an easy way to access the children of the node
  MyGene* NthMyChild (int n) {
    return (MyGene*) GPContainer::Nth (n); }

  int evaluate ();
};



class MyGP : public GP 
{
public:
  // The constructor
  MyGP (int genes) : GP (genes) { }

  // Copy constructor and the function duplicate used by the container
  // class
  MyGP (MyGP& gpo) : GP (gpo) { }
  virtual GPObject& duplicate () { return *(new MyGP(*this)); }

  // We have to overwrite this function to create the objects of our
  // own class MyGene
  virtual GPGene* createGene (GPNode& gpo) {
    return new MyGene (gpo); }

  // Just an easy way to access the trees of the GP
  MyGene* NthMyGene (int n) {
    return (MyGene*) GPContainer::Nth (n); }

  virtual void evaluate ();
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

