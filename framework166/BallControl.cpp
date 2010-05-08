/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: Lift166.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Robot code which controls the banner sensors
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "Team166Task.h"
#include "Proxy166.h"
#include "BallControl.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf166
{
	struct timespec tp;               // Time of snapshot
	// Any values that need to be logged go here
	float BallControl_Speed;
	float BallControl_Current;
};

//  Memory Log
class BallControlLog : public MemoryLog166
{
public:
	BallControlLog() : MemoryLog166(
			sizeof(struct abuf166), BALLCONTROL_CYCLE_TIME, "ballcontrol",
			"Seconds,Nanoseconds,Elapsed Time,Ball Control Speed,Ball Control Current\n"
			) {
		return;
	};
	~BallControlLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(float Vacuum_On, float Vacuum_Current);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int BallControlLog::PutOne(float BallControl_On, float BallControl_Current)
{
	struct abuf166 *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf166 *)GetNextBuffer(sizeof(struct abuf166)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		// Add any values to be logged here
		ob->BallControl_Speed = BallControl_On;
		ob->BallControl_Current = BallControl_Current;
		return (sizeof(struct abuf166));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int BallControlLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf166 *ab = (struct abuf166 *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f,%4.5f,%4.5f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->BallControl_Speed, ab->BallControl_Current); // Add values here
	
	// Done
	return (sizeof(struct abuf166));
}


// task constructor
Team166BallControl::Team166BallControl(void): BallControl_Jag(T166_BALLCONTROL_MOTOR_CAN)
{
	Start((char *)"166BallControl", BALLCONTROL_CYCLE_TIME);
	return;
};
	
// task destructor
Team166BallControl::~Team166BallControl(void)
{
	return;
};
	
// Main function of the task
int Team166BallControl::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	Proxy166 *proxy;				// Handle to proxy
	Robot166 *lHandle;            // Local handle
	BallControlLog sl;                   // log
	float BallControl_Current = 0;
	float BallControl_Speed;
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the Ball Control\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot166::getInstance();
	lHandle->RegisterLogger(&sl);	
	
	// Register the proxy
	proxy = Proxy166::getInstance();		
	
	int valuethrottle = 0;
    // General main loop (while in Autonomous or Tele mode)
	while ((lHandle->RobotMode == T166_AUTONOMOUS) || 
			(lHandle->RobotMode == T166_OPERATOR)) {
		// is the copilot telling it to go in ONE direction?
		BallControl_Speed = ((-proxy->GetThrottle(T166_COPILOT_STICK)+1)/2);
		proxy->SetBallControlSpeed(BallControl_Speed);
		
		BallControl_Speed *= (
					proxy->GetButton(T166_COPILOT_STICK,T166_BALLCONTROL_PULL) -
					proxy->GetButton(T166_COPILOT_STICK,T166_BALLCONTROL_PUSH)
			);
		BallControl_Jag.Set(BallControl_Speed);
		
		if(BallControl_Speed < 0) {
			SetStatus( "backward" );
		} else if(BallControl_Speed > 0) {
			SetStatus( "forward" );
		} else {
			SetStatus( "neutral" );
		}
		if ((++valuethrottle) % (1000/BALLCONTROL_CYCLE_TIME) == 0)
		{
			BallControl_Current = BallControl_Jag.GetOutputCurrent();
			proxy->SetCurrent(T166_BALLCONTROL_MOTOR_CAN, BallControl_Current);
		}
        // Logging our values
		sl.PutOne(BallControl_Speed, BallControl_Current);
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
