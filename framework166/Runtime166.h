/*******************************************************************************
*  Project   		: Framework
*  File Name  		: Runtime166.h  
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Header for class that generates runtime statistics
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WPILib.h"
#include <vector>
using namespace std;

/**
 * @class Runtime166
 * 
 * Basic runtime class for basic statistics, debugging help. 
 * Usage: Declare an instance of this class, then call Start() when you want to begin
 * tracking, then Stop() when you want to stop tracking. This class will keep
 * track of the amount of time between these two calls, and store them in a list
 * of the last (N) "loop" times. GetStats() returns a string containing some stats
 * about this list - max, median, min, average, and standard deviation. 
 * 
 * This class should be useful if you want to know whether a task is running too slowly
 * or erratically. 
 */
class Runtime166 {
	//! Timer (WPILib) that keeps track of the time between Stop and Start calls.
	Timer t;
	//! Vector that keeps track of the loop times
	vector<float> loop_times;
	//! How many times to keep track of
	int limit;
	char* r;
	
public:
	/**
	 * Makes a new instance of Runtime166.
	 * @param limit How many loop times to keep track of before unshifting the old ones
	 */
	Runtime166(int limit = 50);
	/**
	 * Starts tracking time.
	 */
	void Start();
	/**
	 * Stops tracking time.
	 */
	void Stop();
	/**
	 * Generates statistics based on the times designed by stop and start.
	 */
	char* GetStats();
	/**
	 * Gets the number of times it's been stopped.
	 */
	unsigned int Loops();
	/**
	 * Clears tracking time.
	 */
	void Reset();
};
