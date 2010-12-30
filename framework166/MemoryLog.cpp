/*******************************************************************************
*  Project   		: Framework
*  File Name  		: MemoryLog166.cpp    
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: General memory logger on chopshop robot
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include <memLib.h>
#include "wpilib.h"
#include "BaeUtilities.h"
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stat.h>
#include "MemoryLog.h"


// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf
#define Match_Time (135)
#define Padding (5)
// Memory log constructor
MemoryLog::MemoryLog(unsigned int msize, unsigned int ltime, char *filename_, char *titles_)
{
	unsigned int ms;  // Size of memory that we need
	
	// Compute the size that we need for 1 item per loop during 2m15s + 5s padding
	ms = ((1000*(Match_Time + Padding)) / ltime) * msize;
	
	// Allocate the requested memory
	MemoryBase = (char *)valloc(ms);
	
	// Initialize members
	MemorySize = ms;
	MemoryEnd = &MemoryBase[ms];
	MemoryNext = MemoryBase;
	Next = 0;
	BuffersRequested = 0;
	BuffersObtained = 0;
	
	// Capture name of file
	wpi_assert(strlen(filename_)>0);
	FileName = filename_;
	
	// Capture titles
	wpi_assert(strlen(titles_)>0);
	Titles = titles_;
	
	// Not yet registered
	Registered = 0;
	mlNext = 0;
	
	// Done
	return;
}

// Destructor
MemoryLog::~MemoryLog(void)
{
	// Done
	return;
}

// Get next buffer
char *MemoryLog::GetNextBuffer(unsigned int bsize)
{
    char *mptr = MemoryNext;
    
	// Ensure we have something to allocate
    BuffersRequested++;
	if ((!MemoryNext) || (&MemoryNext[bsize] > MemoryEnd))
		return (0);
	
	// Bump next
	BuffersObtained++;
	MemoryNext = &MemoryNext[bsize];
	
	// Back to caller with a link to the memory we just allocated
	return (mptr);
}

// Dump the buffers into a file
int MemoryLog::DumpToFile(void)
{
	char *nptr = MemoryBase;
	char Factual[128];
	FILE *ofile;
	
	// Back out if there is nothing (likely due to allocation failure in constructor
	if (!nptr)
		return (0);
	
	DPRINTF(LOG_DEBUG, "MemoryBase: 0x%x, MemoryNext: 0x%x, nptr: 0x%x\n", MemoryBase, MemoryNext, nptr);
	
	// Create the output file
	Factual[sizeof(Factual) - 1] = 0;
	snprintf(Factual, sizeof(Factual) - 1, "%s.csv", FileName.c_str());
	if (ofile = fopen(Factual, "w")) {
		printf("%s\n",Factual);
		fprintf(ofile,Titles.c_str());
	
		// Enter loop to dump out the data into the file
		int l=0;
		while (nptr < MemoryNext) {
			// Call a user defined routine to format and dump this out
			nptr += DumpBuffer(nptr, ofile);
			l++;
			DPRINTF(LOG_DEBUG, "Dumped buffer #%d; nptr: 0x%x\n", l, nptr);
		}
		// Done. Close the file
		fclose(ofile);
	}
	
	// Reset what we have logged
	MemoryNext = MemoryBase;
	printf("Completed dump for %s into %s;\n", FileName.c_str(), Factual);
	printf("  Requested buffers %d, Obtained buffers %d\n",
			BuffersRequested, BuffersObtained);
	BuffersRequested = 0;
	BuffersObtained = 0;
	
	// Back to caller
	return (0);
}
