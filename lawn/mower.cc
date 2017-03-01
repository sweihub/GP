
// mower.cc

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

#include "gp.h"
#include "lawn.h"



// Reset lawn and mower.  The lawn grows again, and the mower is
// placed on the upper left corner facing south.
void Mower::reset ()
{
  for (int x=0; x<LawnHorizontal; x++)
    for (int y=0; y<LawnVertical; y++)
      lawn[x][y]=1;
  mown=0;
  direction=3;
  pos=Vector (0, 0);
}



// Print a lawn
ostream& operator << (ostream &os, Mower &mower)
{
  for (int y=0; y<LawnVertical; y++)
    {
      for (int x=0; x<LawnHorizontal; x++)
	{
	  // Mower position?
	  if (mower.pos.x==x && mower.pos.y==y)
	    os << 'M';
	  else
	    os << (mower.lawn[x][y] ? 'G' : '.');
	}
      os << '\n';
    }

  return os;
}



// Move from the current position to one which is the addition of new
// arg
void Mower::frog (Vector& v)
{
  // New position (vector addition!)
  pos=pos+v;
      
  // Mow the grass in the area in which you have moved....
  mowArea ();
}



// Turn the mower left
void Mower::left ()
{
  direction = (direction + 1) % 4;
}



// Move the mower forward and mow that area
void Mower::mow ()
{
  switch (direction)
    {
    case 0:
      // East
      pos.x = (pos.x + 1 ) % LawnHorizontal;
      break;
    case 1:
      // North
      pos.y = (pos.y + (LawnVertical-1) ) % LawnVertical;
      break;
    case 2:
      // West
      pos.x = (pos.x + (LawnHorizontal-1) ) % LawnHorizontal;
      break;
    case 3:
      // South
      pos.y = (pos.y + 1 ) % LawnVertical;
      break;
    default:
      GPExitSystem ("Mower::mow", "Wrong switch statement");
    }
      
  // mow the grass in the area in which you have moved....
  mowArea ();
}



// Mow the area at current mower position
void Mower::mowArea ()
{
  // Count how many areas are mown already
  mown+=lawn[pos.x][pos.y];
  
  // Mow the grass in the area
  lawn[pos.x][pos.y]=0;
}



