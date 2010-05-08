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
#include "Banner166.h"
#include "MemoryLog166.h"
#include "Robot166.h"
#include "BaeUtilities.h"
#include "Proxy166.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct bbuf166
{
	struct timespec tp;               // Time of snapshot
	float T166_BANNER_VALUE;          // banner value 1 or 0
};

//  Memory Log
class BannerLog : public MemoryLog166
{
public:
	BannerLog() : MemoryLog166(
			sizeof(struct bbuf166), BANNER_CYCLE_TIME, "banner",
			"Seconds,Nanoseconds,Elapsed Time,Banner State\n"
			) {
		return;
	};
	~BannerLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(UINT32 T166_BANNER_VALUE);     // Log the x and y values
};

// Write one buffer into memory
unsigned int BannerLog::PutOne(UINT32 T166_BANNER_VALUE)
{
	struct bbuf166 *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct bbuf166 *)GetNextBuffer(sizeof(struct bbuf166)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->T166_BANNER_VALUE = T166_BANNER_VALUE;
		return (sizeof(struct bbuf166));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int BannerLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct bbuf166 *ab = (struct bbuf166 *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f,%f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->T166_BANNER_VALUE);
	
	// Done
	return (sizeof(struct bbuf166));
}


// task constructor
Team166Banner::Team166Banner(void):	BannerSensor(T166_BANNER_SENSOR_DIGITAL_INPUT)
{
	Start((char *)"166BannerTask", BANNER_CYCLE_TIME);
	return;
};
	
// task destructor
Team166Banner::~Team166Banner(void)
{
	return;
};
	
// Main function of the task
int Team166Banner::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	Proxy166 *proxy;				// Handle to proxy
	UINT32 CurrentBannerValue;
	Robot166 *lHandle;              // Local handle
	BannerLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Banner task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot166::getInstance();
	lHandle->RegisterLogger(&sl);
	
	// Register the proxy
	proxy = Proxy166::getInstance();
		
    // General main loop (while in Autonomous or Tele mode)
	while ((lHandle->RobotMode == T166_AUTONOMOUS) || 
			(lHandle->RobotMode == T166_OPERATOR)) {
		
		CurrentBannerValue = BannerSensor.Get();	
		
		static int throttle = 0;
		throttle++;
		if (throttle%10==0){
				throttle = 0;
				DPRINTF(LOG_DEBUG,"Channel: %i Value: %i",
						BannerSensor.GetChannel(),CurrentBannerValue);				
				}
		
		proxy->SetBanner(CurrentBannerValue);
		if(CurrentBannerValue) {
			if (!strcmp(GetStatus(),"not detected")){
				// log first detection on DS
				lHandle->DriverStationDisplay("BANNER DETECTED");
			}
			SetStatus("detected");
		} else {
			SetStatus("not detected");
		}		
		
        // Should we log this value?
		sl.PutOne(CurrentBannerValue);
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
