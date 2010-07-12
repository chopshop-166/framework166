/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: Robot166.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Base class used in entire robot
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include <semLib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include "WPILib.h"
#include "Autonomous166.h"
#include "Robot.h"
#include "Includes.h"
#include <stdarg.h>

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// List of tasks that have requested to come up
Team166Task *Team166Task::ActiveTasks[T166_MAXTASK + 1] = {0};

// Create the robot handle that's used by all the other classes
Robot *Robot::RobotHandle = NULL;

// This task has to always be started first or the system will crash
Proxy166 Team166ProxyObject;
// Declare external tasks inside Tasks.h
#include "Tasks.h"

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
Robot::Robot(void)  
{
	/* set up debug output: 
	 * DEBUG_OFF, DEBUG_MOSTLY_OFF, DEBUG_SCREEN_ONLY, DEBUG_FILE_ONLY, DEBUG_SCREEN_AND_FILE  */
	SetDebugFlag ( DEBUG_SCREEN_ONLY  ) ;

	DPRINTF(LOG_DEBUG, "Constructor\n");
	
	RobotMode = T166_CONSTRUCTOR;
	DSLock = semBCreate(SEM_Q_PRIORITY, SEM_FULL);
	dsHandle = DriverStation::GetInstance();
	dsHandleLCD = DriverStationLCD::GetInstance();
	sender = DashboardDataSender::getInstance();
	if(RobotHandle == NULL) {
		RobotHandle = this;
	}
	mlHead = 0;

	// update DS
	DriverStationDisplay("Starting Robot");
	
	// Setup expiration for task watchdog.
	GetWatchdog().SetExpiration(5.0); // 5 seconds

	// Wait for all of our tasks to come up
	printf("Getting ready to check if tasks are up.\n");
	while (!Team166Task::IfUp()) {
		printf("Waiting for task(s) to come up: ");
		Team166Task::PrintInactive();
		printf("\n");
		Wait (0.100);
	}
	printf("All tasks we depend upon are up!\n");	
}

/**
 * Run autonomous class if jumper is in, otherwise wait for Teleop
 */
void Robot::Autonomous(void)
{
	GetWatchdog().SetEnabled(false);
	RobotMode = T166_AUTONOMOUS;
	if(!DigitalInput(T166_AUTONOMOUS_JUMPER).Get()) {
		DPRINTF(LOG_DEBUG,"Entered enabled autonomous\n");
		DriverStationDisplay("IN AUTONOMOUS");
		Autonomous166();
	} else {
		DPRINTF(LOG_DEBUG,"Entered disabled autonomous\n");
		DriverStationDisplay("NO AUTONOMOUS");
	}
}

/**
 * Print a message detailing the robot code version-run once, when disabled
 */
void Robot::Disabled(void)
{
	DriverStationDisplay(CODE_VERSION);
	printf("%s\n", CODE_VERSION);
}

/** 
 * Runs the motors with arcade steering. 
 */
void Robot::OperatorControl(void)
{
	int has_been_disabled = 0;
	
	Timer debugTimer;
	debugTimer.Start();
	
	printf("Operator control\n");
	RobotMode = T166_OPERATOR;
	GetWatchdog().SetEnabled(true);
	DriverStationDisplay("Teleoperated Enabled.");
		dsHandleLCD->UpdateLCD();
	while (IsOperatorControl())
	{
		if(debugTimer.HasPeriodPassed(ROBOT_WAIT_TIME))
			Team166Task::PrintStats();
		
		// Are we being disabled?
		if (IsDisabled()) {
			if (!has_been_disabled) {
				has_been_disabled = true;
				DriverStationDisplay("Dumping Memory Log...");
			    printf("Dumping log files...\n");
			    DumpLoggers(maxLogId);
			    printf("Logfiles dumped!\n");
			    maxLogId++;
			}
			Wait (ROBOT_WAIT_TIME);
			continue;
		} else {
			has_been_disabled = false;
		}
		
		// Each task needs to update for us to feed the watch dog.
		if (Team166Task::FeedWatchDog())
		    GetWatchdog().Feed();
		
		sender->sendIOPortData();
		Wait (ROBOT_WAIT_TIME);
		dsHandleLCD->UpdateLCD();
	}
	
}
	
/**
 * Return a pointer to myself
 */
Robot *Robot::getInstance(void)
{
	return (RobotHandle);
}

/**
 * Register a log object
 */
void Robot::RegisterLogger(MemoryLog166 *ml)
{
	
	// Has this handler been registered already?
	if (!ml->Registered) {
		
		// No. Insert it at the head
		ml->mlNext = mlHead;
		mlHead = ml;
		ml->Registered = 1;
		clock_gettime(CLOCK_REALTIME, &(ml->starttime));
	}
}

/**
 * Dump log objects
 */
void Robot::DumpLoggers(int dnum)
{
	MemoryLog166 *ml;
	
	// Iterate through the list of loggers
	ml = mlHead;
	while (ml) {
		
		// Dump the next one
		ml->DumpToFile();
		
		// Advance to the next log
		ml = ml->mlNext;
	}
}

/**
 * Send text to DS LCD display
 */
int Robot::DriverStationDisplay(const char* format, ...)
{
	va_list args;
	static string dash_string[6];
	static bool init=true;
	char formatted_string[21];
	if(init) {
		//Initializes it first call.
		for(int i=0;i<6;i++) {
			dash_string[i] = "                     ";
		}
		init=false;
	}
	va_start( args, format );
	vsnprintf(formatted_string, DASHBOARD_BUFFER_MAX, format, args);
	va_end(args);
	
	//Move lines up to make room for the newline 
	for(int i=5; i>=1; i--) {
		dash_string[i] = dash_string[i-1];
	}
	dash_string[0] = formatted_string;

	for(int i=0; i<6; i++) {
		dsHandleLCD->PrintfLine((DriverStationLCD::Line)i, dash_string[i].c_str());
	}
	dsHandleLCD->UpdateLCD();
	return 0;
}

START_ROBOT_CLASS(Robot);

