/*******************************************************************************
*  Project   		: Framework
*  File Name  		: MemoryLog166.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Header for general memory logger on chopshop robot
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/
#pragma once

#include <cstdio>
#include <ctime>
#include <string>
using std::string;

//
// This class defines an interface to logging to memory and then into a .csv file
//
class FrameworkLogger
{
protected:
	FrameworkLogger() {};							// Constructor must be overloaded
// Methods	
public:
	struct timespec starttime;					// Time the task started at
	virtual ~FrameworkLogger(void) {};
	virtual int DumpToFile(void) = 0;						// Dump the data into a file
	
// Members
public:
	int Registered;								// This handler has been registered with Robot166
	FrameworkLogger *nextLog;						// Link to the next block
};
