/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: Autonomous166.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Robot code to execute in autonomous mode
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "Robot.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

AutonomousTask::AutonomousTask() {
	// Create handles for proxy and robot
	Robot *lHandle;
	Proxy166 *proxy;
	
	// Register robot handle
	while( !(lHandle = Robot::getInstance()) && !( lHandle->IsAutonomous() ) ) {
		Wait(AUTONOMOUS_WAIT_TIME);
	}
	
	// Register proxy handle
	while( (proxy = Proxy166::getInstance()) == NULL ) {
		Wait(AUTONOMOUS_WAIT_TIME);
	}
	
	while( lHandle->IsAutonomous() ) {
		// <<CHANGEME>>
		// Insert your autonomous logic here
		
		// This wait is required, it makes sure no task uses too much memory
		Wait(AUTONOMOUS_WAIT_TIME);
	}
}

AutonomousTask::~AutonomousTask(void) { return; }
