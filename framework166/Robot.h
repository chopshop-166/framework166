/*******************************************************************************
*  Project   		: Framework
*  File Name  		: Robot.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Header for base class used in entire robot
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WPILib.h"
#include "BaeUtilities.h"
#include "Team166Task.h"
#include "Autonomous.h"
#include "MemoryLog.h"
#include "SimpleLog.h"
#include "Proxy.h"
#include "Defines.h"

// Maximum dashboard buffer size
#define DASHBOARD_BUFFER_MAX (21)

// Robot166 wait time
#define ROBOT_WAIT_TIME (0.5)

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class Robot : public SimpleRobot
{
private:
    DriverStationLCD *dsHandleLCD;				// Driver Station display handle
    FrameworkLogger *mlHead;					// Memory log head
    int maxLogId;								// Max log file id
    static Robot* RobotHandle;					// Singleton instance reference
public:
	Robot(void);								// Constructor
	void Autonomous(void);						// Method called by WPI when we're in autonomous mode
	void OperatorControl(void);					// Method called by WPI when we're in operator control mode
	void Disabled(void);						// Method called by WPI when we're disabled
	static Robot *getInstance(void);			// Get pointer to our Robot166 instance
	void RegisterLogger(FrameworkLogger *ml);			// Register memory logger
	void DumpLoggers(int dnum);					// Dump all logs
	int DriverStationDisplay (const char*, ...);			// Display text on DS
	DriverStation *dsHandle;					// Driver Station handle
};
