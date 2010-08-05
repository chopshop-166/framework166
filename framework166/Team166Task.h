/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: Team166Task.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Header for main robot task and global paramters
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#if !defined(_TEAM166TASK_H)
#define _TEAM166TASK_H
#include "WPILib.h"
#include <cmath>
#include <sysLib.h>
#include "Defines.h"

// Framework version number
#define FRAMEWORK_VERSION ("2010-Aug-04")

//
// task (as in kernel task)
/** Stack size 64 KB */
#define TEAM166TASK_K_STACKSIZE (64 * 1024) 
/** Spawned task priority (100) */
#define TEAM166TASK_K_PRIO (100)

//
// Max tasks we support being started in Robot166
//
#define T166_MAXTASK (32)

//
// Count of times a particular task can fail to report its watchdog before
// we start to display errors.
//
#define T166_WATCHDOG_MIN (3)

//
// Wait time in each task for getting into Autonomous or Teleoperated mode
//
#define T166_TA_WAIT_LENGTH (0.050) // 50ms

class Team166Task
{

// Methods
public:
	
	// Constructor
	Team166Task(int IsEssential=1);
	
	// Destructor
	virtual ~Team166Task();
	
	//! Sets the loop exit status 
	void SetStatus(const char* = ""); // Set a default to empty string for success.
	const char* GetStatus();
	
	// General start routine; needs to be called by target constructor
	int Start(char *tname, unsigned int loop_interval);
	
	// Jacket routine that leads into the actual Main routine of target
	static int MainJacket(void *this_p, int a2, int a3, int a4, int a5,
					int a6, int a7, int a8, int a9, int a10);
	
	// Each class needs to implement this Main function
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10) = 0;
	
	// Wait for Robot go-ahead
	void WaitForGoAhead(void);
	
	// Wait for next lap
	void WaitForNextLoop(void);
	
	static void PrintStats(void);
	// Check if all registered tasks are up
	static int IfUp(void);
	
	// Get handle to a specific task
	static Team166Task *GetTaskHandle(char *name);
	
	// Should we feed the watchdog?
	static int FeedWatchDog(void);
	
	static void PrintInactive(void);
	
	

// Data members
public:
	int MyTaskId;               // Id of our own task
	int MyTaskInitialized;      // This task has been initialized
	int MyWatchDog;             // Feeding my own watch dog
	int MyTaskIsEssential;      // Flag indicating if this task is essential
	int MyStatus;
	const char* MyStatusString;
	char *MyName;               // Name of this task
	int MissedWatchDog;         // Missed watchdog count
	float MyLoopInterval;       // Timing interval for loop
	unsigned int MyLoopMs;      // Length of loop for this 
	unsigned int OverRuns;      // Task over runs
	
private:
	
	// This array points to tasks that have requested to be initialized
	static Team166Task *ActiveTasks[T166_MAXTASK + 1];
	
	// Used for time calculations
	unsigned int crate;           // Clock rate
	unsigned int half_tick;       // Length of a half tick
	struct timespec start_time;   // Time when our logic starts
	struct timespec next_checkin; // Time when we're expected to checkin next
	unsigned int nano_left;       // Nano seconds left of second
	struct timespec exit_time;    // Time when we left the wait call
	unsigned int last_print_sec;  // Last second we printed out a log message
	unsigned int loop_calls;      // Times we've been called to wait
	
};
#endif // !defined(_TEAM166TASK_H)
