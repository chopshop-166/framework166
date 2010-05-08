/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: Inclinometer.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: February 11, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Robot code which controls the inclinometer
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/
#include <math.h>
#include "WPILib.h"
#include "Team166Task.h"
#include "Inclinometer.h"
#include "MemoryLog166.h"
#include "Robot166.h"
#include "BaeUtilities.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct incbuf166
{
	struct timespec tp;               // Time of snapshot
	float angle_inc;                 // Angle of the inclinometer   
};

//  Memory Log
class InclinometerLog : public MemoryLog166
{
public:
	InclinometerLog() : MemoryLog166(
			sizeof(struct incbuf166), INCLINOMETER_CYCLE_TIME, "inclinometer",
			"Seconds,Nanoseconds,Elapsed Time,Tilt (degrees)\n"
			) {
		return;
	};
	~InclinometerLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(int angle_inc);     // Log the x and y values
};

// Write one buffer into memory
unsigned int InclinometerLog::PutOne(int angle_inc)
{
	struct incbuf166 *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct incbuf166 *)GetNextBuffer(sizeof(struct incbuf166)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->angle_inc = angle_inc;
		return (sizeof(struct incbuf166));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int InclinometerLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct incbuf166 *ab = (struct incbuf166 *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f,%0.5f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->angle_inc);
	
	// Done
	return (sizeof(struct incbuf166));
}


// task constructor
Team166Inclinometer::Team166Inclinometer(void):	
	Inclinometer(T166_ENC_INCL_A, T166_ENC_INCL_B)

{
	Start((char *)"166InclinometerTask", INCLINOMETER_CYCLE_TIME);
	return;
};
	
// task destructor
Team166Inclinometer::~Team166Inclinometer(void)
{
	return;
};
	
// Main function of the task
int Team166Inclinometer::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
			
	Robot166 *lHandle;            // Local handle
	Proxy166 *proxy;				// Proxy handle
	InclinometerLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Inclinometer task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot166::getInstance();
	lHandle->RegisterLogger(&sl);	
	
	// Register the proxy
	proxy = Proxy166::getInstance();
	int inclinometerAngle;
	
	int printstop=0;
	Inclinometer.Start();
	
    // General main loop (while in Autonomous or Tele mode)
	while ((lHandle->RobotMode == T166_AUTONOMOUS) || 
			(lHandle->RobotMode == T166_OPERATOR)) {
		
		inclinometerAngle = Inclinometer.Get();
		proxy->SetInclinometer(inclinometerAngle);
		
		if ((++printstop)%(1000/INCLINOMETER_CYCLE_TIME)==0)
		{
			DPRINTF(LOG_DEBUG, "%d", proxy->GetInclinometer());
		}
		
		sl.PutOne(inclinometerAngle);
		// Wait for our next lap
		WaitForNextLoop();
	}
	return (0);
	
};
