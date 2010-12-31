/********************************************************************************
*  Project   		: Framework
*  File Name  		: Team166Task.cpp         
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date 	: January 18, 2010
*  File Description	: This file defines the high level and shared context for a Team 166
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "Robot.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf
// Constructor
Team166Task::Team166Task(int IsEssential)
{	
	// Indicate we've never been spawned before
	MyTaskId = 0;
	MyTaskInitialized = 0;
	MyWatchDog = 0;
	MyTaskIsEssential = IsEssential;
	MyName = 0;
	MyStatusString = "ZZZZZ";
	MissedWatchDog = 0;
	exit_time.tv_sec = 0;	
	exit_time.tv_nsec = 0;
	last_print_sec = 0;
	loop_calls = 0;
	OverRuns = 0;
};
	
// Destructor
Team166Task::~Team166Task()
{
	return;
};
	
// General start routine; needs to be called by target constructor
int Team166Task::Start(char *tname, unsigned int loop_interval)
{
	// Do we have a previous instance of this task?
	if (MyTaskId) {
			
		// Yes. Get rid of it.
		printf("Killing previous instance of task '%s' with id '0x%x'\n",
					tname, MyTaskId);
		taskDelete(MyTaskId);
		MyTaskId = 0;
		MyTaskInitialized = 0;
	}

	// Save the task name for later reference
	MyName = tname;
		
	// Capture the preferred loop time
	if (loop_interval >= 1000) {
		MyLoopNs = 999 * 1000 * 1000; // Convert ms to ns
	} else {
		MyLoopNs = loop_interval * 1000 * 1000; // Convert ms to ns
	}
		
	// Spawn a new task
	MyTaskId = taskSpawn(tname, TEAM166TASK_K_PRIO, VX_FP_TASK,
				TEAM166TASK_K_STACKSIZE, (FUNCPTR) Team166Task::MainJacket,
				(int)this, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	DPRINTF(LOG_DEBUG,"Task '%s' spawned with ID '0x%x'\n", tname, MyTaskId);
 		
	// Figure out this board's tick size (in nano seconds)
	crate = (1000 * 1000 * 1000) / sysClkRateGet();
 		
	// And then the duration of a half-tick (in nano seconds)
	half_tick = crate / 2;
		
	//  Back to caller with the ID of the task we just spawned
	return (MyTaskId);		
};
	
// Jacket routine that leads into the actual Main routine of target
int Team166Task::MainJacket(void *this_p, int a2, int a3, int a4, int a5,
		int a6, int a7, int a8, int a9, int a10)
{

	int l;  // Local loop variable
		
	// Find the next available slot
	for (l=0; l<T166_MAXTASK; l++)
		if (!ActiveTasks[l])
			break;
		
	// Do we have room for one more?
	if (l != T166_MAXTASK) {
		ActiveTasks[l] = (Team166Task *)this_p;
	}

	// Just daisy chain over to virtual main function
	return (((Team166Task *)this_p)->Main(a2, a3, a4, a5, a6, a7, a8, a9, a10));
};
	
// Wait for Robot go-ahead
void Team166Task::WaitForGoAhead(void)
{
		
	// Indicate that we've now completed initialization
	MyTaskInitialized = 1;
			
	// Ensure we get into Autononmous or Tele Operated mode
	while (!Robot::getInstance() ||
		       ((Robot::getInstance()->RobotMode != T166_AUTONOMOUS) &&
		    	(Robot::getInstance()->RobotMode != T166_OPERATOR))) {
		Wait (T166_TA_WAIT_LENGTH);
	}
	MyTaskInitialized = 2;
	
	// Establish our start time
	clock_gettime(CLOCK_REALTIME, &start_time);
	
	// And then the nano seconds left to the next full second
	nano_left = (1000*1000*1000) - start_time.tv_nsec;
	
	// Done
	return;
};

/**
 * @brief Sets the calling task's status. Should be called within a class that
 * derives from this class. 
 * @param exit_status_string A string to represent the exit status.
 */
void Team166Task::SetStatus(const char * exit_status_string) {
	MyStatusString = exit_status_string;
}

/**
 * @brief Gets the calling task's status. 
 */
const char *Team166Task::GetStatus() {
	return MyStatusString;
}

// Get handle to a specific task by name
Team166Task *Team166Task::GetTaskHandle(char *name)
{
	Team166Task *ntask;             // Next task being examined
	int l;                          // Local loop variable
	
	// Enter loop to walk through all tasks
	for (l=0; l<T166_MAXTASK; l++) {
		if ((ntask = ActiveTasks[l]) && (!strcmp(name, ntask->MyName)))
			return (ntask);
	}
	
	// Could not find the named task. Just return a null pointer
	return (0);
}

// Wait for next lap
void Team166Task::WaitForNextLoop(void)
{
		
	struct timespec current_time; // Time when we check again
	struct timespec delta_time;   // Time when we check again
	struct timespec non_sleep;    // Time not slept
	struct timespec rundelta_time;// Time when we check again
	
	// Indicate that we've checked in
	MyWatchDog = 1;
	// Get the current time
	clock_gettime(CLOCK_REALTIME, &current_time);
	
	// Compute delta time since we started our loop
	delta_time.tv_sec = current_time.tv_sec - start_time.tv_sec;
	if (current_time.tv_nsec >= start_time.tv_nsec) {
		delta_time.tv_nsec = current_time.tv_nsec - start_time.tv_nsec;
	} else {
		delta_time.tv_nsec = current_time.tv_nsec + nano_left;
		delta_time.tv_sec--; // We do not care about this for our sleep
	}
	
	// How far into this cycle have we run?
	delta_time.tv_nsec = MyLoopNs - (delta_time.tv_nsec % MyLoopNs);
	if ((unsigned int)delta_time.tv_nsec < half_tick)
		  delta_time.tv_nsec = MyLoopNs;
	delta_time.tv_sec = 0;
	
	// Enable task preemption
	taskUnlock();
	
	// Figure out how long our task ran
	if ((exit_time.tv_sec != 0) || (exit_time.tv_nsec)) {
		rundelta_time.tv_sec = current_time.tv_sec - exit_time.tv_sec;
		if (current_time.tv_nsec >= exit_time.tv_nsec) {
			rundelta_time.tv_nsec = current_time.tv_nsec - exit_time.tv_nsec;
		} else {
			rundelta_time.tv_nsec = exit_time.tv_nsec + current_time.tv_nsec;
			rundelta_time.tv_sec--; // We do not care about this for our sleep
		}
		
		// Have we exceeded our run length?
		if (rundelta_time.tv_sec || ((unsigned int)rundelta_time.tv_nsec > MyLoopNs))
			OverRuns++;
	} else {
		rundelta_time.tv_sec = 0;
		rundelta_time.tv_nsec = 0;
	}
	
	// Next, wait for our time.
	nanosleep((const struct timespec *)&delta_time, &non_sleep);
	
	// Get our exit time from this routine
	clock_gettime(CLOCK_REALTIME, &exit_time);	
	
	// Display a log message once per seond
	loop_calls++;
	if (last_print_sec != current_time.tv_sec) {
		last_print_sec = current_time.tv_sec;
		loop_calls = 0;
	}
	
	// Disable task preemption so we get a clean run-burst
	taskLock();
};

// Check if all registered tasks are up
int Team166Task::IfUp(void)
{
	

	// Loop through all the slots and check each registered task
	for (int i=0; i<T166_MAXTASK; i++) {
			
		// If the task is registered but not initialized, we're not done
		if ((ActiveTasks[i]) &&
			 (!ActiveTasks[i]->MyTaskInitialized)) {
			return (0);	
		}
	}
		
	// We're good
	return (1);
};

/**
 * @brief Prints out which tasks aer not feeding the watchdog.
 */
void Team166Task::PrintStats(void) {
	int last_id = 0;
	for(int x = 0;x<T166_MAXTASK;x++) {
		if(ActiveTasks[x])
			last_id = x;
		else
			break;
	}
	bool printed = false;
	for(int x = 0;x<T166_MAXTASK;x++) {
		if ((ActiveTasks[x]) &&
					 (!ActiveTasks[x]->MyWatchDog)) {
			if(!printed) {
				printed = true;
				printf("Tasks not feeding the watchdog: ");
			}
			printf("%s%s",  ActiveTasks[x]->MyName, (x == last_id ? "" : ", "));
		}
	}
	if(printed)
		printf("\n");
}

/**
 * Prints out a list of tasks that have not initialized.
 */
void Team166Task::PrintInactive(void) {
	int last_id = 0;
	for(int x = 0;x<T166_MAXTASK;x++) {
		if(ActiveTasks[x])
			last_id = x;
		else
			break;
	}
	for(int x = 0;x<T166_MAXTASK;x++) {
		if ((ActiveTasks[x]) &&
					 (!ActiveTasks[x]->MyTaskInitialized)) {
			printf("%s%c",  ActiveTasks[x]->MyName, (x == last_id ? '\0' : ' '));
		}
	}
}

/**
 * Prints out a list of all the Framework-based tasks.
 */
void Team166Task::PrintAllTasks(void) {
	int last_id = 0;
	for(int x = 0;x<T166_MAXTASK;x++) {
		if(ActiveTasks[x])
			last_id = x;
		else
			break;
	}
	for(int x = 0;x<T166_MAXTASK;x++) {
		if (ActiveTasks[x]) {
			printf("%s%c",  ActiveTasks[x]->MyName, (x == last_id ? '\0' : ' '));
		}
	}
}
	
// Should we feed the watchdog?
int Team166Task::FeedWatchDog(void)
{
	int i;	//Looping variable
	// Loop through all the slots and check each registered task
	for (i=0; i<T166_MAXTASK; i++) {
			
		// Is this a registered and essential task?
		if ((ActiveTasks[i]) &&
			 (ActiveTasks[i]->MyTaskIsEssential)) {
				
			// Yes. Has this task set its watchdog?
			if (!ActiveTasks[i]->MyWatchDog) {
				// No. Tell caller at least one task is not ready.
				
				// Increase the number of times it's missed
				ActiveTasks[i]->MissedWatchDog++;
				
				// Has the task missed the watchdog too many times?
				if (ActiveTasks[i]->MissedWatchDog > T166_WATCHDOG_MIN) {
					printf("Task '%s' has not reported its watchdog %d times in a row.\n",
						ActiveTasks[i]->MyName ? ActiveTasks[i]->MyName : "unknown",
						T166_WATCHDOG_MIN);
				}
				return (0);
			}
		}
	}
		
	// If we get here they have all said we're good. Clear them and tell caller
	for (i=0; i<T166_MAXTASK; i++)
		if ((ActiveTasks[i]) &&
			 (ActiveTasks[i]->MyTaskIsEssential)) {
			ActiveTasks[i]->MyWatchDog = 0;
			ActiveTasks[i]->MissedWatchDog = 0;				
		}
	return (1);
};
