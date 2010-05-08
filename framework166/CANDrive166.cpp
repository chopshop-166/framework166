/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: CANDrive166.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 23, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Code which monitors jaguar return info
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "Team166Task.h"
#include "CANDrive166.h"
#include "MemoryLog166.h"
#include "Robot166.h"
#include "BaeUtilities.h"
#include "Proxy166.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf166
{
	struct timespec tp;               // Time of snapshot
	float l_current;              // left motor current value
	float r_current;		      // right motor current value	
};

//  Memory Log
class CANDriveLog : public MemoryLog166
{
public:
	CANDriveLog() : MemoryLog166(
			sizeof(struct abuf166), CAN_CYCLE_TIME, "candrive",
			"Seconds,Nanoseconds,Elapsed Time,Left Current,Right Current\n"
			) {
		return;
	};
	~CANDriveLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(float l_current, float r_current);     // Log the x and y values
};

// Write one buffer into memory
unsigned int CANDriveLog::PutOne(float l_current, float r_current)
{
	struct abuf166 *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf166 *)GetNextBuffer(sizeof(struct abuf166)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->l_current = l_current;
		ob->r_current = r_current;
		return (sizeof(struct abuf166));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int CANDriveLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf166 *ab = (struct abuf166 *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f,%f,%f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->l_current, ab->r_current);
	
	// Done
	return (sizeof(struct abuf166));
}

// Initializes the ProxyHandle. 
Team166CANDrive *Team166CANDrive::CANDriveHandle = 0;

/**
 * @brief Gets the singleton instance of Proxy166.
 * @return The instance of Proxy166
 */
Team166CANDrive *Team166CANDrive::getInstance(void)
{
	return CANDriveHandle;
}


// task constructor
Team166CANDrive::Team166CANDrive(void):
	leftJag(T166_LEFT_MOTOR_CAN),
	rightJag(T166_RIGHT_MOTOR_CAN)
{
	CANDriveHandle=this;
	Start((char *)"166CANDriveTask", CAN_CYCLE_TIME);
	return;

};
	
// task destructor
Team166CANDrive::~Team166CANDrive(void)
{
	return;
};

// Main function of the task
int Team166CANDrive::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
		
	Robot166 *lHandle;                  // Local handle
	CANDriveLog sl;                    // log
	Proxy166 *proxy;				  //pointer to proxy	
	
	int valuethrottle=0;
	
	char *buffer = new char[DASHBOARD_BUFFER_MAX];
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 CANDrive task\n");

	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();

	proxy = Proxy166::getInstance();
	
	// Register our logger
	lHandle = Robot166::getInstance();
	lHandle->RegisterLogger(&sl);
	
	printf("CANDrive is ready.\n");
#if UsingAutobalance
	float left=0,right=0;
#endif
	
	float leftCurrent=0, rightCurrent=0;

    // General main loop (while in Autonomous or Tele mode)
	while ((lHandle->RobotMode == T166_AUTONOMOUS) || 
			(lHandle->RobotMode == T166_OPERATOR)) {
#if UsingAutobalance
		if( proxy->GetButton(T166_DRIVER_STICK_LEFT,T166_AUTOBALANCE_BUTTON)
				|| proxy->GetButton(T166_DRIVER_STICK_RIGHT,T166_AUTOBALANCE_BUTTON)) {
			if(proxy->GetInclinometer() < -AUTOBALANCE_DEADZONE) {
				left = -AUTOBALANCE_SPEED;
				right = -AUTOBALANCE_SPEED;
			} else if(proxy->GetInclinometer() > AUTOBALANCE_DEADZONE) {
				left = AUTOBALANCE_SPEED;
				right = AUTOBALANCE_SPEED;
			} else {
				left = 0;
				right = 0;
			}
		} else {
			left = proxy->GetJoystickY(1);
			right = proxy->GetJoystickY(2);
		}
		leftJag.Set(-left);
		rightJag.Set(right);
#else
		leftJag.Set(-proxy->GetJoystickY(1));
		rightJag.Set(proxy->GetJoystickY(2));
#endif
		if ((++valuethrottle) % (1000/CAN_CYCLE_TIME) ==0)
		{
			// Get Current from each jaguar 
			leftCurrent = leftJag.GetOutputCurrent();
			rightCurrent = rightJag.GetOutputCurrent();
			// Put current values into proxy
			proxy->SetCurrent(T166_LEFT_MOTOR_CAN, leftCurrent);
			proxy->SetCurrent(T166_RIGHT_MOTOR_CAN, rightCurrent);
			// Print debug to console
			DPRINTF(LOG_DEBUG, "Left Jag Current: %f", leftCurrent);
			DPRINTF(LOG_DEBUG, "Right Jag Current: %f", rightCurrent );
		}
		if (false) {
			sprintf(buffer,"DRV: %f %f", leftCurrent, rightCurrent);
			lHandle->DriverStationDisplay(buffer);
		}
		
		// do stuff
		sl.PutOne(leftCurrent, rightCurrent);
		
		// Wait for our next lap
		WaitForNextLoop();
	}
	return (0);
	
};
