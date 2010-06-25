/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: Robot.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Header for base class used in entire robot
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#if !defined(_ROBOT166_H)
#define _ROBOT166_H
#include "WPILib.h"
#include "BaeUtilities.h"
#include "CANJaguar.h"
#include "Team166Task.h"
#include "Target.h"
#include "Autonomous166.h"
#include "MemoryLog166.h"
#include "Proxy166.h"
class DashboardDataSender;
#include "DashboardDataSender.h"
#include "Defines.h"

// Maximum dashboard buffer size
#define DASHBOARD_BUFFER_MAX (21)

// Robot166 wait time
#define ROBOT_WAIT_TIME (0.5)

//
// Robot modes
//
typedef enum {T166_UNKNOWN=0, T166_CONSTRUCTOR, T166_AUTONOMOUS, T166_OPERATOR, T166_DISABLED} t_RobotMode;

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class Robot166 : public SimpleRobot
{
public:
    t_RobotMode RobotMode;                    // Robot mode
private:
	SEM_ID JoyLock;                           // Coordination of Joystick parameters
	float JoyX;                               // Joystick X position
	float JoyY;                               // Joystick Y position
    SEM_ID DSLock;                            // Coordination of drive station interface
    SEM_ID LiftLock;                          // LIft lock
    SEM_ID KickLock;                          // Kicker lock
    DriverStation *dsHandle;                  // Driver Station handle
    DriverStationLCD *dsHandleLCD;            // Driver Station display handle
    MemoryLog166 *mlHead;                     // Memory log head
    int maxLogId;                             // Max log file id
    DashboardDataSender *sender;				  // Dashboard sender
public:
	Robot166(void);                           // Constructor
	void Autonomous(void);                    // Method called by WPI when we're in autonomous mode
	void OperatorControl(void);               // Method called by WPI when we're in operator control mode
	void Disabled(void);                    // Method called by WPI when we're disabled
	static Robot166 *getInstance(void);       // Get pointer to our Robot166 instance
	float GetBatteryVoltage(void);            // Get voltage of battery on robot
			
	void RegisterLogger(MemoryLog166 *ml);    // Register memory logger
	void DumpLoggers(int dnum);               // Dump all logs
	int DriverStationDisplay (char*);		  // Display text on DS
};

#endif // !defined(_ROBOT166_H)
