/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: Sonar166.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Header for robot code which reads the sonar sensors
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#if !defined(_SONAR166_H)
#define _SONAR166_H
#include "WPILib.h"
#include "Robot166.h"
#include "semLib.h"

//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
//
// Sonar does time readings every 49 ms
//
#define SONAR_CYCLE_TIME (45) // 45 ms

//
// This defines our sonar task
//
class Team166Sonar : public Team166Task
{
	
// Methods
public:
	
	// Sonar task constructor
	Team166Sonar(void);
	
	// Sonar task destructor
	virtual ~Team166Sonar(void);
	
	// Main function of the Sonar task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);

// Members
public:
	float distance;                         // Distance to the object
	
};
#endif // !defined(_SONAR166_H)
