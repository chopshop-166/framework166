/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: TaskTemplate.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Template header file for tasks, with template functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#if !defined(_TASK_TEMPLATE_H)
#define _TASK_TEMPLATE_H
#include "WPILib.h"
#include "Robot166.h"

//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
// You should rename this when you copy it into a new file 
#define TEMPLATE_CYCLE_TIME (10) // 10ms

// Rename this, too, or you'll run into collisions
class Template166 : public Team166Task
{
	
public:
	
	// task constructor
	Template166(void);

	// task destructor
	virtual ~Template166(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	// Any variables that the task has as members go here
};

#endif // !defined(_TASK_TEMPLATE_H)
