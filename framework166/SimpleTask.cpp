/*******************************************************************************
*  Project   		: Framework
*  File Name  		: TaskTemplate.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Template source file for tasks, with template functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
/* Find & Replace "Template" with the name you would like to give this task     */
/* Find & Replace "Testing" with the name you would like to give this task      */
/* Find & Replace "TaskTemplate" with the name you would like to give this task */
/*------------------------------------------------------------------------------*/

#include "WPILib.h"
#include "SimpleTask.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// task constructor
SimpleTask::SimpleTask(string n,SimpleTask::MainPtr tM, unsigned speed)
{
	taskMain = tM;
	taskName = n;
	Start((char*)taskName.c_str(), speed);
	// ^^^ Rename those ^^^
	// <<CHANGEME>>
	// Register the proxy
	proxy = Proxy::getInstance();
	return;
};
	
// task destructor
SimpleTask::~SimpleTask(void)
{
	return;
};
	
// Main function of the task
int SimpleTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	if(taskMain == NULL) {
		return(0);
	}
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"SimpleTask started: %s\n", taskName.c_str());
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot::getInstance();
		
    // Call the "main" function
	while(!taskMain(proxy,lHandle)) {
		WaitForNextLoop();
	}
	return (0);
	
};
