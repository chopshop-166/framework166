/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: HealthMon166.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 23, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Header for code which monitors health of system
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#if !defined(_HEALTH166_H)
#define _HEALTH166_H
#include "WPILib.h"
#include "Robot166.h"
//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
//
#define HEALTHMON_CYCLE_TIME (100) // 999ms

class Team166HealthMon : public Team166Task
{
	
public:
	
	// task constructor
	Team166HealthMon(void);

	// task destructor
	virtual ~Team166HealthMon(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
};

#endif // !defined(_CANHEALTH166)
