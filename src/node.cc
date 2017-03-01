
// node.cc

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

// Function and Terminal Sets

// The class NodeSet contains nodes, e.g. functions and terminals.
// Some functions choose from either functions and terminals.  A quick
// and fast access is desirable.  We do this by placing functions at
// the beginning of the container, terminals at the end.


#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "gp.h"

using namespace std;

// Copy Constructor
GPNode::GPNode (const GPNode& gpo) : GPObject(gpo) 
{
  nodeValue=gpo.nodeValue; 
  numOfArgs=gpo.numOfArgs; 
  representation=copyString (gpo.representation);
}



// Duplicate a string
char *GPNode::copyString (char *str)
{
  char *s=new char[strlen (str)+1];
  strcpy (s, str);
  return s;
}



// Load operation
char* GPNode::load (istream& is)
{
  char line[400];

  is >> nodeValue;
  is >> numOfArgs;

  // Read in string
  do 
    {
      if (!is)
	return "Character '\"' not found in stream";
      is >> line[0];
    }
  while (line[0]!='"');
  int ix=0;
  do 
    {
      if (!is)
	return "Matching character '\"' not found in stream";
      is >> line[ix++];
      if (ix>=(int)sizeof(line)) 
	return "Internal buffer overflow: matching character '\"' not found";
    }
  while (line[ix-1]!='"');
  line[ix-1]='\0';

  if (representation)
    delete [] representation;
  representation=copyString (line);
  return NULL;
}



// Save operation
void GPNode::save (ostream& os)
{
  os << nodeValue << ' ';
  os << numOfArgs << ' ';
  os << "\"" << representation << "\" ";
}



// We inherit this function because the user must not put anything in
// our container by hand.
void GPNodeSet::put (int, GPObject&)
{
  GPExitSystem ("GPNodeSet::put", 
		"User must not use this function, but putNode() instead."); 
}



// Put a node in the container.  
void GPNodeSet::putNode (GPNode& gpo)
{
#if GPINTERNALCHECK
  // Check if full
  if (numFunctions+numTerminals==containerSize())
    GPExitSystem ("GPNodeSet::putNode", "Node set is full"); 

  // Check if node with same identification number is already in set
  // (that's forbidden!)
  if (searchForNode (gpo.value()))
    GPExitSystem ("GPNodeSet::putNode", 
		  "Node with same ID is not allowed in one node set");
#endif

  // Put functions at the beginning, terminals at the end.
  if (gpo.isFunction())
    GPContainer::put (numFunctions++, gpo);
  else
    GPContainer::put (containerSize() - (++numTerminals), gpo);
}



// Choose a node at random from node set with the given number of
// arguments
GPNode* GPNodeSet::chooseNodeWithArgs (int args)
{
  int i, num;
  GPNode *n;

  // First we count all nodes that have the specified number of
  // arguments
  for (i=0, num=0; i<containerSize(); i++)
    if ((n=NthNode (i)))
      if (n->arguments()==args)
	num++;

  // No node with given number of arguments?
  if (num==0)
    return NULL;

  // Choose one
  int k=GPrand() % num;

  // Return the node with chosen index
  for (i=0, num=0; i<containerSize(); i++)
    if ((n=NthNode (i)))
      if (n->arguments()==args)
	if (num++==k)
	  return n;

#if GPINTERNALCHECK
  GPExitSystem ("GPNodeSet::chooseNodeWithArgs", "Internal error"); 
#endif

  // Avoid compiler warnings (this code is never reached)
  return NULL;
}



// This chooses a function from the set.
GPNode& GPNodeSet::chooseFunction ()
{
#if GPINTERNALCHECK
  if (numFunctions==0)
    GPExitSystem ("GPNodeSet::chooseFunction", 
		  "No function to choose from"); 
#endif

  return *NthNode (GPrand() % numFunctions);
}



// This chooses a terminal from the set.
GPNode& GPNodeSet::chooseTerminal ()
{
#if GPINTERNALCHECK
  if (numTerminals==0)
    GPExitSystem ("GPNodeSet::chooseTerminal", 
		  "No terminal to choose from"); 
#endif

  return *NthNode (containerSize()-numTerminals+GPrand() % numTerminals);
}



// Prints out complete node set.  Functions are printed with
// parenthesis and a value to indicate their number of arguments
void GPNodeSet::printOn (ostream& os)
{
  // Move through the set printing out each node in turn
  for (int i=0; i<containerSize(); i++)
    {
      GPNode* current=NthNode (i);
      if (i>0)
	os << ' ';
      if (current)
	{
	  os << *current;
	  if (current->isFunction())
	    os << "(" << current->arguments() << ")";
	}
      else
	os << "NoNode";
    }
}



// Move through the set looking for the corresponding value.  Return
// NULL, if no one found, otherwise the address of the node
GPNode* GPNodeSet::searchForNode (int value)
{
  // Move through the container
  for (int n=0; n<containerSize(); n++)
    {
      GPNode* current=NthNode (n);
      if (current)
	if (current->value ()==value)
	  return current;
    }

  // Not found
  return NULL;
}



// Load operation
char* GPNodeSet::load (istream& is)
{
  is >> numFunctions;
  is >> numTerminals;

  // Load container
  return GPContainer::load (is);
}



// Save operation
void GPNodeSet::save (ostream& os)
{
  os << numFunctions << ' ';
  os << numTerminals << ' ';

  // Save container
  GPContainer::save (os);
}



// Prints out complete node set for each adf tree
void GPAdfNodeSet::printOn (ostream& os)
{
  // Move through container printing out each function set in turn
  for (int i=0; i<containerSize(); i++)
    {
      GPNodeSet* current=NthNodeSet (i);
      if (current)
	os << "GP tree " << i << ": "  << *current << endl;
      else
	os << "No Set";
    }
}











