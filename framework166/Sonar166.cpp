/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: Sonar166.cpp
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Robot code which reads the sonar sensors
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "Sonar166.h"
#include "MemoryLog166.h"
#include <math.h>
#include <sysLib.h>

// Sample in memory buffer
struct sobuf166
{
	struct timespec tp;               // Time of snapshot
	float distance;                   // Distance seen
	
};

// Sample Memory Log
class SonarLog : public MemoryLog166
{
public:
	SonarLog() : MemoryLog166(
			sizeof(struct sobuf166), SONAR_CYCLE_TIME, "sonar",
			"Seconds,Nanoseconds,Elapsed Time,Distance\n"
			) {
		return;
	};
	~SonarLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(float distance); // Log the distanace
};

// Write one buffer into memory
unsigned int SonarLog::PutOne(float dist)
{
	struct sobuf166 *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct sobuf166 *)GetNextBuffer(sizeof(struct sobuf166)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->distance = dist;
		return (sizeof(struct sobuf166));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int SonarLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct sobuf166 *ab = (struct sobuf166 *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f,%f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->distance);
	
	// Done
	return (sizeof(struct sobuf166));
}


// Sonar task constructor
Team166Sonar::Team166Sonar(void)
{
	// Initialize assorted fields
	
	// Start our task
	Start((char *)"166SonarTask", SONAR_CYCLE_TIME);	
};
	
// Sonar task destructor
Team166Sonar::~Team166Sonar(void)
{		
	return;
};
	
// Main function of the Sonar task
//
// This is now we compute the distance in inches.
//
//  1. The A/D converter can handle -10V to +10V using a signed 12 bit value.
//     This means that each value increment represents 20/4096V
//  2. The Sonar input is 5V
//  3. The sonar output is 5/512 per inch.
//  4. The data collecting loop maintains a ring of the UMAX most recent values
//     and then computes a rolling average.
//  5. The value distance can be obtained by:
//     RollingAvg * ((20 / 4096) / (5 / 512)) = RollingAvg / 2.
//
int Team166Sonar::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{

	Robot166 *lHandle;            // Local handle
	Proxy166 *pHandle;            // Proxy handle
	AnalogChannel ac(T166_US_MOD, T166_US_DIST); // Ultrasound sensor
	SonarLog sl;                  // Sonar log
#define UMAX (20)                 // Window size for rolling average
	INT16 utmp;                   // Temporary ultransound value
	INT16 uval[UMAX];             // Ultrasound value
	int uidx = 0;                 // Index into array
	int al;                       // Average distance loop
	unsigned int aavg;            // Cumulative count
#if 0
	static int cc = 0;            // Counter to control print out
#endif
	
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot166::getInstance();
	lHandle->RegisterLogger(&sl);
	
	// Get a handle to the proxy object
	pHandle = Proxy166::getInstance();
	
    // General main loop (while in Autonomous or Tele mode)
	while ((lHandle->RobotMode == T166_AUTONOMOUS) || 
			(lHandle->RobotMode == T166_OPERATOR)) {
		
        // Pick up A/D converter's value of Ultrasound (12 bits for -10V to +10V)
		utmp =  ac.GetValue(); // -2047 - + 2047
#if 0
        if (!(cc % 40))
        	printf("Value org: %d\n", utmp);
#endif
		
		// If the value is below acceptable range, assume minimum value
		if (utmp < 0) {  // Below 0/0V?
			utmp = 0;    // Yes, set to 0.0V
		} else {
			if (utmp >= 512) // 2.5V or greater?
				utmp = 512;
		}

		// Bring this into the average rung buffer
        uval[(uidx++ % UMAX)] = utmp;
        
        // Add all the values we've seen together (so that we can compute an average)
        aavg = 0;
        for (al=0; al<UMAX; al++)
        	aavg += uval[al];
        
        // Compute the distance in inches. See comments above.
        distance = (aavg / (UMAX * 2.0));
        pHandle->SetSonarDistance(distance);
        if(distance > 15) {
        	SetStatus("not detected");
        } else if(distance <= 15 && distance >= 6){
        	SetStatus("detected");
        	// log value only if valid distance detected
            sl.PutOne(distance);
        } else {
        	SetStatus("error");
        }
#if 0
        // Display a log message
        if (!(cc++ % 40))
        	printf("Value: %u, Distance: %f\n", utmp, distance);
#endif   	
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
}
