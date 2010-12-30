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
class MemoryLog
{

// Methods	
public:
	struct timespec starttime;					// Time the task started at
	MemoryLog(unsigned int msize, unsigned int ltime, char *f, char *titles);
	virtual ~MemoryLog(void);
	char *GetNextBuffer(unsigned int bsize);	// Get next buffer to write
	int DumpToFile(void);						// Dump the data into a file
	virtual unsigned int DumpBuffer(			// Dump the next buffer into the file
			char *nptr,							// Buffer that needs to be formatted
			FILE *outputFile) = 0;				// and then stored in this file
	
// Members
public:
	int Registered;								// This handler has been registered with Robot166
	MemoryLog *mlNext;						// Link to the next block

private:
	unsigned int MemorySize;					// Size of the memory we have allocated
	char *MemoryBase;							// Base pointer to memory just allocated
	char *MemoryEnd;							// End of allocated memory
	char *MemoryNext;							// Next
	MemoryLog *Next;					// Link to next log structure
	string FileName;								// Filename
	string Titles;								// Titles at the top of the csv
	int BuffersRequested;						// Buffers requested
	int BuffersObtained;						// Buffers obtained
};
