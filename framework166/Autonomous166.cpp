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

#include "Autonomous166.h"
#include "RobotCamera166.h"
#include "Proxy166.h"
#include "Robot166.h"

#define AUTONOMOUS_TILT (10)
#define AUTONOMOUS_MOTOR_TOLERENCE (10)
#define AUTONOMOUS_SPEED (-0.25)
#define AUTONOMOUS_MOTOR_FLAG_LIMIT (5)

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

Autonomous166::Autonomous166() {
	Robot166 *lHandle;
	char buffer[DASHBOARD_BUFFER_MAX];
	float lmotor=0;
	float rmotor=0;
	float lmotor_old=0;
	float rmotor_old=0;
	short motorflag=0;
	bool inclinometerflag=false;
	
	while( !(lHandle = Robot166::getInstance()) && !( lHandle->IsAutonomous() ) ) {
		Wait(AUTONOMOUS_WAIT_TIME);
	}
	
	// Create and register proxy handle
	Proxy166 *proxy;
	while( (proxy = Proxy166::getInstance()) == NULL ) {
		Wait(AUTONOMOUS_WAIT_TIME);
	}

	// Sensor storage data
	int banner=0;
	int inclinometer=0;
	bool AlreadyStopped=false;
	
	while( lHandle->IsAutonomous() ) {
		// Reset sensors
		banner = proxy->GetBanner();
		inclinometer = proxy->GetInclinometer();
		lmotor_old = lmotor;
		lmotor = proxy->GetCurrent(T166_LEFT_MOTOR_CAN);
		rmotor_old = rmotor;
		rmotor = proxy->GetCurrent(T166_RIGHT_MOTOR_CAN);
		

		inclinometerflag = ((inclinometer > AUTONOMOUS_TILT) || (inclinometer < -AUTONOMOUS_TILT));
		
		if( (((lmotor - lmotor_old) >= AUTONOMOUS_MOTOR_TOLERENCE) && lmotor) ||
			(((rmotor - rmotor_old) >= AUTONOMOUS_MOTOR_TOLERENCE) && lmotor) ) {
			++motorflag;
		} else {
			motorflag = ((--motorflag>0)? motorflag : 0);
		}
		
		if( banner || inclinometerflag || (motorflag > AUTONOMOUS_MOTOR_FLAG_LIMIT) ) {
			proxy->SetJoystickY(T166_DRIVER_STICK_LEFT,0);
			proxy->SetJoystickY(T166_DRIVER_STICK_RIGHT,0);
			if(!AlreadyStopped) {
				AlreadyStopped = true;
				if(banner) {
					sprintf(buffer,"STOP: BANNER");
				} else if((inclinometer > AUTONOMOUS_TILT) || (inclinometer < -AUTONOMOUS_TILT)) {
					sprintf(buffer,"STOP: TILT");
				} else {
					sprintf(buffer,"STOP: MOTORS");
				}
				lHandle->DriverStationDisplay(buffer);
			}
		} else {
			proxy->SetJoystickY(T166_DRIVER_STICK_LEFT, AUTONOMOUS_SPEED);
			proxy->SetJoystickY(T166_DRIVER_STICK_RIGHT, AUTONOMOUS_SPEED);
		}
	}
}

Autonomous166::~Autonomous166(void) { return; }
