
// gp.h

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

// Genetic Program class definitions



#ifndef __GP_H
#define __GP_H

#include <iostream>

using std::ostream;
using std::istream;


// Version and copyright message
extern char* GPVersion;
void GPPrintCopyright (ostream& os);

// Make internal checks or not. Better do it... it saved me a lot of
// work I can tell.  It results in a speed loss of roughly 3%, that is
// less than it sounds.  A good, optimising compiler can do much more
// here.
#define GPINTERNALCHECK 1

// If an error occurs (GPExitSystem()), a segmentation fault helps
// debugging the code to find the location that caused the error
#define GPCREATE_SEGMENTATIONFAULT_ON_ERROR 1

// Random number functions for gp system
void GPsrand (long);
long GPrand ();
int GPRandomPercent (double percent);

// Init and exit
void GPInit (int printCopyright, long seedRandomGenerator);
void GPExitSystem (char *functionName, char *errorMessage); 

// Registering for load/save
class GPObject;
GPObject* GPCreateRegisteredClassObject (int ID);
void GPRegisterClass (GPObject* gpo);
void GPRegisterKernelClasses ();

// Every class gets an unique identification number that is returned
// by the function isA().
#define GPObjectID       1
#define GPContainerID    2
#define GPNodeID         3
#define GPNodeSetID      4
#define GPAdfNodeSetID   5
#define GPVariablesID    6
#define GPGeneID         7
#define GPID             8
#define GPPopulationID   9
#define GPUserID        50



// ------------------------------------------------------------------



// The base class for all other classes.  This is an abstract class.
// The destructor is virtual.

class GPObject
{
public:
  GPObject () {}
  virtual ~GPObject () {}

  GPObject (const GPObject&) {}
  // Duplicate object. This function must be provided by every
  // inheriting class!!!
  virtual GPObject& duplicate ()=0;

  virtual int isA ()=0;
  virtual char* load (istream& is)=0;
  virtual void save (ostream& os)=0;
  virtual GPObject* createObject()=0;

  // Print function. It's virtual, so only one operator << is needed.
  virtual void printOn (ostream& os)=0;
};

// Print operator for all classes that inherit from GPObject
inline ostream& operator << (ostream& os, GPObject& gpo)
{
  gpo.printOn (os);
  return os;
}



class GPContainer : public GPObject
{
public:
  GPContainer ();
  GPContainer (int numObjects);
  virtual ~GPContainer ();

  GPContainer (const GPContainer& gpc);
  virtual GPObject& duplicate () { return *(new GPContainer(*this)); }

  virtual void printOn (ostream& os);

  void reserveSpace (int numObjects);

  int containerSize() const { return contSize; }

  GPObject* Nth (int n) const;
  GPObject** getPointerAddress (int n) const;
  void put (int n, GPObject& gpo);
  GPObject& get (int n);

  virtual int isA () { return GPContainerID; }
  virtual char* load (istream& is);
  virtual void save (ostream& os);
  virtual GPObject* createObject() { return new GPContainer; }

  GPContainer& operator = (GPContainer& gpo) {
    GPExitSystem ("operator =", "Assignment operator not yet implemented"); 
    return gpo; }

protected:
  void deleteContainer ();

  // The actual container is an array of pointers to GPObject
  GPObject** container;
  // The container size
  int contSize;
};



// ------------------------------------------------------------------



class GPNode : public GPObject
{
public:
  GPNode () { nodeValue=0; numOfArgs=0; representation=0; }
  GPNode (int nVal, char* str, int args=0) : nodeValue(nVal), 
      numOfArgs(args) { representation=copyString (str); }
  virtual ~GPNode () { delete [] representation; }

  GPNode (const GPNode& gpo);
  virtual GPObject& duplicate () { return *(new GPNode(*this)); }

  virtual void printOn (ostream& os) { os << representation; }

  int value () { return nodeValue; }
  int isFunction () { return numOfArgs!=0; }
  int isTerminal () { return numOfArgs==0; }
  int arguments () { return numOfArgs; }

  virtual int isA () { return GPNodeID; }
  virtual char* load (istream& is);
  virtual void save (ostream& os);
  virtual GPObject* createObject() { return new GPNode; }

  GPNode& operator = (GPNode& gpo) {
    GPExitSystem ("operator =", "Assignment operator not yet implemented"); 
    return gpo; }

protected:
  // The nodes numerical value, the number of arguments it takes (0 if
  // it is a terminal) and its string representation.  We operate on a
  // duplicate
  int nodeValue;
  int numOfArgs;
  char* representation;

  char *copyString (char *s);
};



class GPNodeSet : public GPContainer
{
public:
  GPNodeSet () { numFunctions=0; numTerminals=0; }
  GPNodeSet (int numOfNodes) : GPContainer (numOfNodes) {
    numFunctions=0; numTerminals=0; }

  GPNodeSet (const GPNodeSet& gpo) : GPContainer(gpo) { 
    numFunctions=gpo.numFunctions; numTerminals=gpo.numTerminals; }
  virtual GPObject& duplicate () { return *(new GPNodeSet(*this)); }

  virtual void put (int, GPObject&);
  virtual void putNode (GPNode& gpo);

  virtual void printOn (ostream& os);

  virtual GPNode* searchForNode (int value);
  GPNode* NthNode (int n) {
    return (GPNode*) GPContainer::Nth (n); }

  virtual GPNode& chooseFunction();
  virtual GPNode& chooseTerminal();
  virtual GPNode* chooseNodeWithArgs (int args);

  virtual int isA () { return GPNodeSetID; }
  virtual char* load (istream& is);
  virtual void save (ostream& os);
  virtual GPObject* createObject() { return new GPNodeSet; }

  GPNodeSet& operator = (GPNodeSet& gpo) {
    GPExitSystem ("operator =", "Assignment operator not yet implemented"); 
    return gpo; }

protected:
  // Container contains functions from 0..numFunctions-1 and terminals
  // from containerSize()-1-numTerminals..containerSize()-1
  int numFunctions, numTerminals;
};



class GPAdfNodeSet : public GPContainer
{
public:
  GPAdfNodeSet () {}
  GPAdfNodeSet (int numOfTrees) : GPContainer(numOfTrees) {}

  GPAdfNodeSet (const GPAdfNodeSet& gpo) : GPContainer(gpo) {}
  virtual GPObject& duplicate () { return *(new GPAdfNodeSet(*this)); }

  // As we have no variables, we leave load and save to our container
  // class
  virtual int isA () { return GPAdfNodeSetID; }
  virtual GPObject* createObject() { return new GPAdfNodeSet; }

  virtual void printOn (ostream& os);

  GPNodeSet* NthNodeSet (int n) { 
    return (GPNodeSet*) GPContainer::Nth (n); }

  GPAdfNodeSet& operator = (GPAdfNodeSet& gpo) {
    GPExitSystem ("operator =", "Assignment operator not yet implemented"); 
    return gpo; }
};



// ------------------------------------------------------------------



// Prototypes
class GP;
class GPPopulation;

// These definitions are for the different types of creation method
// available to the genetic programming system and are set by the user.
enum GPCreationType {
  // variable tree to maxdepth creation
  GPVariable=0,
  // all branches go to maxdepth creation
  GPGrow,
  // half and half of the the two types shown below. This is the
  // creation method of Koza...
  GPRampedHalf,
  GPRampedVariable,
  // grow branches equal to max depth ofcreation with this
  // parameter being increased as we move through population
  GPRampedGrow,
  // Whatever the user thinks. We haven't implemented this, but the
  // user can in one of his inherited classes
  GPUserDefinedCreation};

// Selection types: tournament selection or probabilistic selection.
enum GPSelectionType {
  GPProbabilisticSelection=0,
  GPTournamentSelection,
  GPUserDefinedSelection};



class GPVariables : public GPObject
{
public:
  GPVariables ();
  ~GPVariables () {};

  GPVariables (const GPVariables& gpo);
  virtual GPObject& duplicate () { return *(new GPVariables(*this)); }

  int PopulationSize,
    NumberOfGenerations,
    CreationType,
    MaximumDepthForCreation,
    MaximumDepthForCrossover,
    SelectionType,
    TournamentSize,
    DemeticGrouping,
    DemeSize,
    AddBestToNewPopulation,
    SteadyState;
  double CrossoverProbability, CreationProbability,
    SwapMutationProbability, ShrinkMutationProbability,
    DemeticMigProbability;

  virtual void printOn (ostream& os);

  virtual int isA () { return GPVariablesID; }
  virtual char* load (istream& is);
  virtual void save (ostream& os);
  virtual GPObject* createObject() { return new GPVariables; }
};



class GPGene : public GPContainer
{
public:
  GPGene () { node=0; }
  GPGene (GPNode& gpo)
    : node(&gpo), GPContainer(gpo.arguments()) {}

  GPGene (const GPGene& gpo)
    : GPContainer (gpo) { node=gpo.node; }
  virtual GPObject& duplicate () { return *(new GPGene(*this)); }

  virtual GPGene* createChild (GPNode& gpo) {
    return new GPGene (gpo); }

  virtual int isA () { return GPGeneID; }
  virtual char* load (istream& is);
  virtual void save (ostream& os);
  void resolveNodeValues (GPNodeSet& ns);
  virtual GPObject* createObject() { return new GPGene; }

  virtual void printOn (ostream& os);

  virtual int isFunction () { return node->isFunction (); }
  virtual int isTerminal () { return node->isTerminal (); }

  GPNode& geneNode () { return *node; }
  GPGene* NthChild (int n) {
    return (GPGene*) GPContainer::Nth (n); }
  GPGene** findNthNode (GPGene** rootPtr, int findFunction,
			int &iLengthCount);

  virtual GPGene** choose (GPGene** rootPtr);

  int countFunctions ();
  GPGene** chooseFunctionNode (GPGene** rootPtr);

  virtual int length ();
  virtual int depth (int depthSoFar=1);

  virtual void create (enum GPCreationType ctype, int allowabledepth, 
		       GPNodeSet& ns);

  friend int operator == (GPGene& pg1, GPGene& pg2);
  virtual int compare (GPGene& g);

  friend GP;

  GPGene& operator = (GPGene& gpo) {
    GPExitSystem ("operator =", "Assignment operator not yet implemented"); 
    return gpo; }

protected:
  // Pointer to Function or Terminal.  It's a good solution (I think)
  // not to have any specific values in this class, but rather
  // pointers to the functions.  You can get all the information you
  // need from them... On the other hand, if we load a gene, we
  // load/save only the node value.  Another function has to be called
  // after the load process to convert this value back to a pointer
  union
  {
    GPNode* node;
    int nodeValue;
  };
};



class GP : public GPContainer
{
public:
  GP () { fitnessValid=0; GPlength=0; GPdepth=0; }
  GP (int trees) : GPContainer (trees) { fitnessValid=0; 
    GPlength=0; GPdepth=0; }

  GP (const GP& gpo) : GPContainer(gpo) { stdFitness=gpo.stdFitness; 
    fitnessValid=gpo.fitnessValid; GPlength=gpo.GPlength; 
    GPdepth=gpo.GPdepth; }
  virtual GPObject& duplicate () { return *(new GP(*this)); }

  virtual GPGene* createGene (GPNode& gpo) {
    return new GPGene (gpo); }

  virtual void printOn (ostream& os);

  GPGene* NthGene (int n) { return (GPGene*) GPContainer::Nth(n); }

  double getFitness () { return stdFitness; }
  virtual int length () { return GPlength; }
  virtual int depth () { return GPdepth; }
  virtual void calcLength ();
  virtual void calcDepth ();

  virtual int compare (GP& gp);
  virtual void create (enum GPCreationType ctype, int allowabledepth, 
		       GPAdfNodeSet& adfNs);

  void shrinkMutation ();
  void swapMutation (GPAdfNodeSet& adfNs);
  virtual void mutate (GPVariables& GPVar, GPAdfNodeSet& adfNs);
  virtual GPContainer& cross (GPContainer* parents, 
			      int maxdepthforcrossover);
  virtual void evaluate ();

  virtual int isA () { return GPID; }
  virtual char* load (istream& is);
  virtual void save (ostream& os);
  void resolveNodeValues (GPAdfNodeSet& adfNs);
  virtual GPObject* createObject() { return new GP; }

  friend GPPopulation;

  GP& operator = (GP& gpo) {
    GPExitSystem ("operator =", "Assignment operator not yet implemented"); 
    return gpo; }

protected:
  // The standardized fitness (positive numbers, and 0.0 is best) and
  // a flag whether the fitness of this GP-object is already
  // calculated or not. The flag must be set to 0 by any operation
  // that changes the GP (for example the crossover and mutation), but
  // not by reproduction. This can save us a lot of time.
  int fitnessValid;
  double stdFitness;

  // Length and depth of GP
  int GPlength, GPdepth;
};



// A structure to simplify the parameter exchange for all the
// selection functions.  It holds the range for which the selection
// has to take place, and a flag that determines whether this is the
// first selection of that particular deme.  This is used to speed up
// the selection process for the probablistic selection.
struct GPPopulationRange
{
  int startIx, endIx;
  int firstSelectionPerDeme;
};



class GPPopulation : public GPContainer
{
public:
  GPPopulation () {}
  GPPopulation (GPVariables& GPVar_, GPAdfNodeSet& adfNs_) : 
    adfNs(&adfNs_), GPVar(GPVar_) {}

  GPPopulation (const GPPopulation& gpo) : GPContainer(gpo), adfNs(gpo.adfNs)
    { GPVar=gpo.GPVar; avgFitness=gpo.avgFitness; 
    avgLength=gpo.avgLength; avgDepth=gpo.avgDepth; }
  virtual GPObject& duplicate () { return *(new GPPopulation(*this)); }

  virtual void printOn (ostream& os);

  GP* NthGP (int n) { return (GP*) GPContainer::Nth (n); }

  virtual int checkForValidCreation (GP& gpo);
  virtual void create ();
  virtual GP* createGP (int numOfTrees) { return new GP (numOfTrees); }

  double totalFitness ();
  long totalLength ();
  long totalDepth ();

  virtual void tournamentSelection (int *selection, int numToSelect,
				    int selectWorst, 
				    GPPopulationRange& range);
  virtual void probabilisticSelection (int *selection, int numToSelect, 
				       int selectWorst, 
				       GPPopulationRange& range);
  virtual void selectIndices (int *selection, int numToSelect, 
			      int selectWorst, GPPopulationRange& range);
  virtual GPContainer* select (int numToSelect, GPPopulationRange& range);
  virtual GPContainer* selectParents (GPPopulationRange& range);
  virtual void calculateStatistics ();
  virtual void evaluate();

  virtual void createGenerationReport (int printLegend, int generation,
				       ostream& fout, ostream& bout);
  GPContainer* evolution (GPPopulationRange& range);
  virtual void generate (GPPopulation& newPop);
  virtual void demeticMigration ();

  virtual int isA () { return GPID; }
  virtual char* load (istream& is);
  virtual void save (ostream& os);
  void setNodeSets (GPAdfNodeSet& adfNs_);
  virtual GPObject* createObject() { return new GPPopulation; }

  // Index to the best and worst of a population. Only valid after a
  // call to calculateStatistics (done by evaluate())
  int bestOfPopulation, worstOfPopulation;

  GPPopulation& operator = (GPPopulation& gpo) {
    GPExitSystem ("operator =", "Assignment operator not yet implemented"); 
    return gpo; }

protected:
  // We have to save the function and terminal sets here, because we
  // need them later
  GPAdfNodeSet* adfNs;

  // These are important variables used for the configuration of the
  // whole GP system
  GPVariables GPVar;

  // Average values: some useful statistics. Calculated by
  // calculateStatistics() (which is called by evaluate())
  double avgFitness, avgLength, avgDepth;

private:
  // These variables are needed only for the probablistic selection
  // method and are calculated anew for every deme or population.  So
  // they need not to be load/saved.  We save the summed inverse
  // fitness and the summed fitness of the deme/population in a static
  // variable and determine whether we have to calculate it again each
  // call depending on variable firstCallPerDeme (parameter to the
  // selection function).
  double invSumFitness;
  double sumFitness;

  int checkForDiversity (GP& gp);
};



#endif

