/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: HealthMon166.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 23, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Code which monitors health of system
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "Team166Task.h"
#include "HealthMon166.h"
#include "MemoryLog166.h"
#include "Robot166.h"
#include "BaeUtilities.h"
#include "CANJaguar.h"
#include "Vision166.h"
#include "CANDrive166.h"



// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf166
{
	struct timespec tp;						// Time of snapshot
	char healthstats[DASHBOARD_BUFFER_MAX];	// String
};

//  Memory Log
class HealthMonLog : public MemoryLog166
{
public:
	HealthMonLog() : MemoryLog166(
			sizeof(struct abuf166), HEALTHMON_CYCLE_TIME, "healthmon",
			"Seconds,Nanoseconds,Elapsed Time,Healthmon String\n"
			) {
		return;
	};
	~HealthMonLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(char*);     // Log values
};

// Write one buffer into memory
unsigned int HealthMonLog::PutOne(char* stats)
{
	struct abuf166 *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf166 *)GetNextBuffer(sizeof(struct abuf166)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		strncpy(ob->healthstats, stats, DASHBOARD_BUFFER_MAX);
		return (sizeof(struct abuf166));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int HealthMonLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf166 *ab = (struct abuf166 *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f,%s\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->healthstats);
	
	// Done
	return (sizeof(struct abuf166));
}


// task constructor
Team166HealthMon::Team166HealthMon(void)
{
	Start((char *)"166HealthMonTask", HEALTHMON_CYCLE_TIME);
	MyTaskIsEssential=false;
	return;
};
	
// task destructor
Team166HealthMon::~Team166HealthMon(void)
{
	return;
};
	
// Main function of the task
int Team166HealthMon::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	Robot166 *lHandle;            		// Local handle
	Proxy166 *proxy;              		// Local proxy handle
	HealthMonLog sl;					// log
	Team166Task *kickerTask;			// Kicker task
	Team166Task *sonarTask;				// Sonar task
#if UsingCamera
	Team166Task *visionTask;			// Vision task
#endif
	Team166Task *bannerTask;			// Banner task
	Team166Task *ballcontrolTask;			// Ball Control task
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 HealthMon task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register our logger
	while(!(lHandle = Robot166::getInstance())) {
		Wait(T166_TA_WAIT_LENGTH);
	}
	lHandle->RegisterLogger(&sl);
	
	char* buffer = new char[DASHBOARD_BUFFER_MAX];
	
	// Register each task
	while(!(proxy = Proxy166::getInstance())) {
		Wait(T166_TA_WAIT_LENGTH);
	}
	while(!(kickerTask = Team166Task::GetTaskHandle("166KickerTask"))) {
		Wait(T166_TA_WAIT_LENGTH);
	}
	while(!(sonarTask = Team166Task::GetTaskHandle("166SonarTask"))) {
		Wait(T166_TA_WAIT_LENGTH);
	}
#if UsingCamera
	while(!(visionTask = Team166Task::GetTaskHandle("166VisionTask"))) {
		Wait(T166_TA_WAIT_LENGTH);
	}
#endif
	while(!(bannerTask = Team166Task::GetTaskHandle("166BannerTask"))) {
		Wait(T166_TA_WAIT_LENGTH);
	}
	while(!(ballcontrolTask = Team166Task::GetTaskHandle("166BallControl"))) {
		Wait(T166_TA_WAIT_LENGTH);
	}
	
	// Whether the camera is up

	// Print out the key
	lHandle->DriverStationDisplayHS("K B S C B");
	
    // General main loop (while in Autonomous or Tele mode)
	while ((lHandle->RobotMode == T166_AUTONOMOUS) || 
			(lHandle->RobotMode == T166_OPERATOR)) {
		sprintf(buffer,
				"%c %c %c %c %c",
				kickerTask->GetStatus()[0],
				bannerTask->GetStatus()[0],
				sonarTask->GetStatus()[0],
#if UsingCamera
				visionTask->GetStatus()[0],
#else
				'Z',
#endif
				ballcontrolTask->GetStatus()[0]
				);
		lHandle->DriverStationDisplayHSData(buffer);
		
		// do stuff
		sl.PutOne(buffer);
		WaitForNextLoop();
	}
	return (0);
	
};
