
// contain.cc

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

// Container class.  This class is responsible for the objects placed
// in the container.  If a container object is deleted, all objects
// contained in the container are also deleted.

#include <stdlib.h>

#include "gp.h"



// Constructor
GPContainer::GPContainer ()
{
  container=NULL;
  contSize=0;
}



// Constructor
GPContainer::GPContainer (int numObjects) 
{
  // Set pointer to NULL, otherwise reserveSpace() complains, and
  // allocate space
  container=NULL;
  reserveSpace (numObjects);
}



// Copy constructor
GPContainer::GPContainer (const GPContainer& gpc) : GPObject(gpc)
{
  // Set pointer to NULL, otherwise reserveSpace() complains, and
  // allocate space
  container=NULL;
  reserveSpace (gpc.contSize);

  // Make a copy of all container objects of gpc and save them into
  // the array
  for (int n=0; n<contSize; n++)
    {
      if (gpc.container[n])
	// This is real magic!  We can duplicate whatever object is in
	// the container, be it a whole population!  Thanks to OOP...
	put (n, gpc.container[n]->duplicate ());
      else	
	container[n]=NULL;
    }
}



// Destructor.
GPContainer::~GPContainer ()
{
  deleteContainer ();
}



// Deletes all objects that are in the container, and then the array
// that points to the objects.
void GPContainer::deleteContainer () 
{
  if (container)
    {
      // Delete container objects
      for (int i=0; i<contSize; i++)
	if (container[i])
	  // As the destructor is virtual, any object in the container
	  // is destroyed here
	  delete container[i];

      // Delete array of pointer to members
      delete [] container;

      // Set initial conditions
      container=NULL;
    }
}



// Reserve space for container, e.g. an array of pointers to the
// objects.
void GPContainer::reserveSpace (int numObjects) 
{
#if GPINTERNALCHECK
  if (container)
    GPExitSystem ("GPContainer::reserveSpace", "Container not empty");
#endif

  // We can't alloc space, if this is 0.  This is not an error!  A
  // terminal node, for example, doesn't have children.
  if (numObjects<=0)
    { 
      contSize=0;
      return;
    }

  // Save the container size in object
  contSize=numObjects;

  // Alloc array of pointers and set them to NULL
  container=new GPObject*[numObjects];
  for (int i=0; i<numObjects; i++)
    container[i]=NULL;
}



// Print the contents of the container
void GPContainer::printOn (ostream& os)
{
  os << "Container has " << contSize << " Elements:\n";

  // Print all members of the container
  for (int i=0; i<containerSize(); i++)
    if (container[i]==NULL)
      os << "(NULL) ";
    else
      os << *container[i];
}



// Gets the n-th object from the container.  This function might
// return NULL, if there is no object at that location.
GPObject* GPContainer::Nth (int n) const
{
#if GPINTERNALCHECK
  if (!container)
    GPExitSystem ("GPContainer::Nth", "No container");
  if (n>=contSize || n<0)
    GPExitSystem ("GPContainer::Nth", "Wrong range for index n");
#endif

  return container[n];
}



// Return the adress of the pointer to the n-th Object within the
// container.  This is usefule for crossover, as only pointers are
// swapped to exchange complete subtrees
GPObject** GPContainer::getPointerAddress (int n) const
{
#if GPINTERNALCHECK
  if (!container)
    GPExitSystem ("GPContainer::getPointerAdress", "No container");
  if (n>=contSize || n<0)
    GPExitSystem ("GPContainer::getPointerAdress", 
		  "Wrong range for index n");
#endif

  return &container[n];
}



// Place an object in the container.  Now it's owned by the container!
// If the container is destroyed, its objects are destroyed also
void GPContainer::put (int n, GPObject& gpo)
{
#if GPINTERNALCHECK
  if (!container)
    GPExitSystem ("GPContainer::put", "No container");
  if (n>=contSize || n<0)
    GPExitSystem ("GPContainer::put", "Wrong range for index n");
#endif

  // If there is already an element, delete it.  The virtual
  // destructor is used to kill the object.
  if (container[n]!=NULL)
    delete container[n];

  // Put the object at the given location
  container[n]=&gpo;
}



// Get an object from the container.  Now the container is no longer
// responsible for its destruction, as it doesn't own it any longer.
// The receiver expects an object and therefore it is checked that
// there is indeed an object in the container at that position.
GPObject& GPContainer::get (int n)
{
#if GPINTERNALCHECK
  if (!container)
    GPExitSystem ("GPContainer::get", "No container");
  if (n>=contSize || n<0)
    GPExitSystem ("GPContainer::get", "Wrong range for index n");
  if (container[n]==NULL)
    GPExitSystem ("GPContainer::get", "No object at position");
#endif

  GPObject* tmp=container[n];

  // Set the reference to NULL, so that we know there is no longer any
  // object
  container[n]=NULL;

  return *tmp;
}



// Loading a container is more difficult than saving...
char* GPContainer::load (istream& is)
{
  int id=0;

  // First check the ID
  is >> id;
  if (id!=isA ())
    return "Object ID GPContainerID expected, but not found";
  
  // Read container size from stream and reserve space
  is >> contSize;
  reserveSpace (contSize);

  // Load all members of the container
  for (int i=0; i<contSize; i++)
    {
      // Test if element was a NULL pointer or an object
      char elementIsThere='n';
      is >> elementIsThere;
      if (elementIsThere=='y')
	{
	  // Read identification number of object
	  int ID;
	  is >> ID;

	  // Create that element depending on the ID that was saved
	  // before.  A special registration mechanism is used to be
	  // able to create any object that was saved before and was
	  // registered also.
	  container[i]=GPCreateRegisteredClassObject (ID);

	  // Let the created object load itself.  Abort, of an error
	  // occured
	  char* errMsg=container[i]->load (is);
	  if (errMsg) 
	    return errMsg;
	}
    }

  // No error
  return NULL;
}



// Saving a container is pretty easy.
void GPContainer::save (ostream& os)
{
  // Save the ID
  os << "\n" << isA() << ' ';

  // Size
  os << contSize << ' ';

  // Save all members of the container
  for (int i=0; i<containerSize(); i++)
    if (container[i]==NULL)
      // No element
      os << 'n';
    else
      {
	// Element is there
	os << 'y';

	// Save identification number and element itself
	os << container[i]->isA () << ' ';
	container[i]->save (os);
      }
}
