/*******************************************************************************
*  Project   		: Framework
*  File Name  		: Autonomous.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Header for robot code to execute in autonomous mode
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#ifndef _AUTONOMOUS_H
#define _AUTONOMOUS_H
#include "Robot.h"
#include "MemoryLog166.h"
#include "WPILib.h"
#include "BaeUtilities.h"
#include <cmath>

#define AUTONOMOUS_WAIT_TIME (0.050)

class AutonomousTask
{
public:
	AutonomousTask(void);
	~AutonomousTask(void);
};

#endif