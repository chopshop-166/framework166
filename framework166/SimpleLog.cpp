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
#include "SimpleLog.h"
#include "Robot.h"


// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

SimpleLog::SimpleLog(string n, string values) : name(n)
{
	clock_gettime(CLOCK_REALTIME, &starttime);
	file = fopen((name+".csv").c_str(),"w");
	if(file) {
		fprintf(file, "Seconds,Nanoseconds,Elapsed Time,%s\n",values.c_str());
	} else {
		
	}
}

bool SimpleLog::PutOne(string frmtstr, ...) {
	struct timespec currtime;
	va_list args;
	va_start(args, frmtstr);
	clock_gettime(CLOCK_REALTIME, &currtime);
	if(!file || Robot::getInstance()->IsDisabled()) return false;
	fprintf(file, "%4.5f,",
			((currtime.tv_sec - starttime.tv_sec) + ((currtime.tv_nsec-starttime.tv_nsec)/1000000000.))
	);
	vfprintf(file, (frmtstr+"\n").c_str(), args);
	va_end(args);
	return true;
}

bool SimpleLog::operator()(string frmtstr, ...) {
	struct timespec currtime;
	va_list args;
	va_start(args, frmtstr);
	clock_gettime(CLOCK_REALTIME, &currtime);
	if(!file || Robot::getInstance()->IsDisabled()) return false;
	fprintf(file, "%4.5f,",
			((currtime.tv_sec - starttime.tv_sec) + ((currtime.tv_nsec-starttime.tv_nsec)/1000000000.))
	);
	vfprintf(file, (frmtstr+"\n").c_str(), args);
	va_end(args);
	return true;
}

int SimpleLog::DumpToFile() {
	if(!file) return false;
	fflush(file);
	return true;
}

SimpleLog::~SimpleLog() {
	fclose(file);
}
