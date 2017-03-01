
// trail.cc

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

Copyright 1993, 1994 Adam Fraser, 1996, 1997 Thomas Weinbrenner

For comments, improvements, additions (or even money) contact:

Thomas Weinbrenner
Grauensteinstr. 26
35789 Laimbach
Germany
E-mail: thomasw@emk.e-technik.th-darmstadt.de
WWW:    http://www.emk.e-technik.th-darmstadt/~thomasw

--------------------------------------------------------------- */

// This module is responsible for moving the ant around the artificial
// world and picking up all the food the ant moves onto

#include <fstream>

#include "gp.h"
#include "ant.h"

using namespace std;

// Read trail from file
void Ant::readTrail (char* fname)
{
  int x=0, y=0;

  ifstream ifs (fname);
  if (!ifs) 
    GPExitSystem ("Ant::readTrail", 
		  "There is no trail data file" );
  else
    {
      char ch=NOTHING;

      // Move to the correct position in the data file: The area below
      // hash.  The double '!' looks a bit odd, but is needed here as
      // one of it is an operator redefined by the stream class
      while (!(!ifs) && ch!='#') 
	ifs >> ch;
	
      while (y<worldVertical)
	{
	  // Read character and put it in the constant world member
	  // variable
	  ifs >> ch;
	  if (ch!=NOTHING && ch!=FOOD && ch!=GAP)
	    GPExitSystem ("Ant::readTrail", 
			  "Wrong character in trail data file" );
	  constantWorld[x][y]=ch;
	    
	  // Increase index
	  x++;
	  if (x==worldHorizontal) 
	    {
	      y++;
	      x=0;
	    }
	}
      if (!ifs) 
	GPExitSystem ("Ant::readTrail", 
		      "Error reading trail data file" );
    }

  // Count the number of available food.  Needed to evaluate the
  // standardised fitness.
  for (maxFood=0, x=0; x<worldHorizontal; x++)
    for (y=0; y<worldVertical; y++)
      maxFood+=constantWorld[x][y]==FOOD;
}



// Reset ant and trail.
void Ant::reset (int energy_)
{
  for (int x=0; x<worldHorizontal; x++)
    for (int y=0; y<worldVertical; y++)
      world[x][y]=constantWorld[x][y];
  energy=energy_;
  direction=0;     // East
  pos.x=0; pos.y=0;
}



// Print the trail
ostream& operator << (ostream &os, Ant &ant)
{
  for (int y=0; y<worldVertical; y++)
    {
      for (int x=0; x<worldHorizontal; x++)
	{
	  // Ant's current position?
	  if (ant.pos.x==x && ant.pos.y==y)
	    os << 'A';
	  else
	    os << ant.world[x][y];
	}
      os << '\n';
    }

  return os;
}



// Turn the ant left
void Ant::left ()
{
  if (!energy) 
    return;
  energy--;
  direction = (direction+1) % 4;
}



// Turn the ant right
void Ant::right ()
{
  if (!energy) 
    return;
  energy--;
  direction = (direction+3) % 4;
}



// Return the final position if the ant would move forward
position Ant::forwardPosition ()
{
  position p=pos;

  switch (direction)
    {
    case 0:
      // East
      if (p.x<worldHorizontal-1)
	p.x++;
      break;
    case 1:
      // North
      if (p.y>0) 
	p.y--;
      break;
    case 2:
      // West
      if (p.x>0) 
	p.x--;
      break;
    case 3:
      // South
      if (p.y<worldVertical-1)
	p.y++;
      break;
    default:
      GPExitSystem ("Ant::forwardPosition", "Wrong switch statement");
    }
  return p;
}



// Move the ant forward and return whether food was found
int Ant::forward ()
{
  if (!energy) 
    return 0;
  energy--;

  // New position
  pos=forwardPosition ();

  // Eat food, if something is there
  int food=world[pos.x][pos.y]==FOOD;
  if (food)
    world[pos.x][pos.y]=NOTHING;
  return food;
}




// Return whether there is food ahead
int Ant::isFoodAhead ()
{
  position p=forwardPosition ();
  return world[p.x][p.y]==FOOD;
}



