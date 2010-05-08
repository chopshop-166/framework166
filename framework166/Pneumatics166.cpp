/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: Pneumatics166.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Robot code which controls the pneumatics
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "Team166Task.h"
#include "Proxy166.h"
#include "Pneumatics166.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct pbuf166
{
	struct timespec tp;               // Time of snapshot
	// Any values that need to be logged go here
	float pressure;                   // psi
	bool compressor_on;
	
};

//  Memory Log
class PneumaticsLog : public MemoryLog166
{
public:
	PneumaticsLog() : MemoryLog166(
			sizeof(struct pbuf166), PNEUMATICS_CYCLE_TIME, "pneumatics",
			"Seconds,Nanoseconds,Elapsed Time,PSI,Compressor State\n"
			) {
		return;
	};
	~PneumaticsLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	unsigned int PutOne(float ppressure, bool compresson_on);        // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int PneumaticsLog::PutOne(float ppress, bool c_on)
{
	struct pbuf166 *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct pbuf166 *)GetNextBuffer(sizeof(struct pbuf166)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		// Add any values to be logged here
		ob->pressure = ppress;
		ob->compressor_on = c_on;
		return (sizeof(struct pbuf166));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int PneumaticsLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct pbuf166 *ab = (struct pbuf166 *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f,%f,%d\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->pressure, ab->compressor_on); // Add values here
	
	// Done
	return (sizeof(struct pbuf166));
}


// task constructor
Pneumatics166::Pneumatics166(void)
{
	Start((char *)"166PneumaticsTask", PNEUMATICS_CYCLE_TIME);
	return;
};
	
// task destructor
Pneumatics166::~Pneumatics166(void)
{
	return;
};
	
// Main function of the task
int Pneumatics166::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	// THE VARS LIVE HERE!!!!!!!!!!
	Proxy166 *proxy;	                            // Get handle for joystick
	Robot166 *lHandle;                              // Local handle
	PneumaticsLog sl;                               // Pneumatics log
	DigitalInput PressureSwitch(T166_PNEUMATIC_PRESSURE_SWITCH); // Pressure switch
	Relay CompressorSpike(T166_COMPRESSOR_RELAY_CHANNEL,Relay::kForwardOnly);   // Compressor spike
	int CompressorOn = 0;                           // Compressor state
	AnalogChannel ps(T166_PNEU_MOD,TI66_PNEU_PRESS);	// Pressure sensor
	float pressure;                                 	// Pressure from a/d converter
	float ppressure;                                // Pressure converted to psi
	bool PressureSensorGood = true;				// Whether the pressure sensor's good
	int doprint = 0;
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Pneumatics task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot166::getInstance();
	lHandle->RegisterLogger(&sl);	

	// Get handle to main Proxy166
	proxy = Proxy166::getInstance();
	
	//int printstop=0;
    // General main loop (while in Autonomous or Tele mode)
	while ((lHandle->RobotMode == T166_AUTONOMOUS) || 
			(lHandle->RobotMode == T166_OPERATOR)) {
		

		// Capture the pressure by adjusted voltage
		// Subtract 0.5 because sensor ranges from 0.5 to 4.5
		pressure = (ps.GetVoltage()-0.5);
		
		// If the value is below acceptable range, assume minimum value
		if( (pressure < 0) || (pressure > 4.0) ) { // 4.5V or greater?
			SetStatus("error");
			if(PressureSensorGood) {
				DPRINTF(LOG_DEBUG, "Bad values, using backup sensor");
			}
			PressureSensorGood = false;
		}
		
		// Convert voltage to PSI
		// (250 (Max PSI)/4 (Max adjusted voltage)==62.5)
		if(PressureSensorGood) {
			ppressure = (pressure * 62.5);
		} else {
			ppressure = (PressureSwitch.Get())? T166_PNEU_HIGH+1 : T166_PNEU_LOW-1;
		}
		
        // Update the pressure value in the proxy
        proxy->SetPressure(ppressure);
    	
        // Print various values for debugging
		if ((doprint++ %40) == 0)
			DPRINTF(LOG_DEBUG, "Pressure switch value: %d %d \n\t Trim: %f Psi: %f\n",
					PressureSwitch.Get(), CompressorOn, pressure, ppressure);
		
		if(!PressureSwitch.Get()) {
			CompressorSpike.Set(Relay::kOn);
			SetStatus("Compressing");
			CompressorOn = true;
		} else {
			CompressorSpike.Set(Relay::kOff);
			SetStatus("Decompressing");
			CompressorOn = false;
		}
		
        // Logging any values
		sl.PutOne(ppressure, CompressorOn);
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
