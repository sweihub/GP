// trail.hpp

//--------------------------------------------------------------------------
// This code is a component of Genetic Programming in C++ (Version 0.40)
// Copyright Adam P. Fraser, 1993,1994
// This code is released for non-commercial use only.
// For comments, improvements, additions (or even money !?) contact:
// Adam Fraser, Postgraduate Section, Dept of Elec & Elec Eng,
// Maxwell Building, University Of Salford, Salford, M5 4WT, United Kingdom.
// Internet: a.fraser@eee.salford.ac.uk
// Tel: (UK) 061 745 5000 x3633
// Fax: (UK) 061 745 5999
//--------------------------------------------------------------------------


#ifndef TRAIL

#define TRAIL

#define World_Horizontal 32    // defined in trail.cc
#define World_Vertical  32

extern unsigned short int World[World_Horizontal][World_Vertical];
extern void CreateWorld( void );
extern void CreateTrail( void );

#endif

//trail.hpp
