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
#include "FrameworkLogger.h"
using std::string;

//
// This class defines an interface to logging to memory and then into a .csv file
//
class SimpleLog : public FrameworkLogger
{
	FILE *file;
public:
	SimpleLog(string name, string values);		// Format string
	~SimpleLog();
	bool PutOne(string frmtstr, ...);				// Add some data to the log
	bool operator()(string frmtstr, ...);
	int DumpToFile(void);						// Dump the log into a file
	string name;
};
