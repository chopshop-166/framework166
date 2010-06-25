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

#define AUTONOMOUS_TILT (10)
#define AUTONOMOUS_MOTOR_TOLERENCE (10)
#define AUTONOMOUS_SPEED (-0.50)
#define AUTONOMOUS_MOTOR_FLAG_LIMIT (5)

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

Autonomous166::Autonomous166() {
	// Create handles for proxy and robot
	Robot166 *lHandle;
	Proxy166 *proxy;
	
	// Register robot handle
	while( !(lHandle = Robot166::getInstance()) && !( lHandle->IsAutonomous() ) ) {
		Wait(AUTONOMOUS_WAIT_TIME);
	}
	
	// Register proxy handle
	while( (proxy = Proxy166::getInstance()) == NULL ) {
		Wait(AUTONOMOUS_WAIT_TIME);
	}
	
	while( lHandle->IsAutonomous() ) {
		// Fill in your own logic and functions here
		Wait(AUTONOMOUS_WAIT_TIME);
	}
}

Autonomous166::~Autonomous166(void) { return; }
