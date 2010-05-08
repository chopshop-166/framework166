/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: Vision166.h    
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Robot code header which handles vision of camera
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#ifndef _VISION166_H
#define _VISION166_H

#include "semLib.h"
#include "WPILib.h"
#include "Team166Task.h"
#include "MemoryLog166.h"
#include "Target166.h" 
#include "Target.h"


// WPILib include files for vision
#include "TrackAPI.h" 
#include "Vision/AxisCamera.h"
#include "Vision/HSLImage.h"

#define FLIP_SERVO_VERTICAL true
#define PI 3.14159265358979
#define DEFAULT_VERTICAL_PAN_POSITION 0
#define CAMERA_SPAWN_TRY_WAIT 0.5 // seconds to wait for each thread
#define CAMERA_SPAWN_WAIT_MAX 20.0 // seconds to wait for the camera to come up. 

/** ratio of horizontal image field of view (54 degrees) to horizontal servo (180) */
#define HORIZONTAL_IMAGE_TO_SERVO_ADJUSTMENT (54.0/180.0)   // this seems to work
/** ratio of vertical image field of view (40.5 degrees) to vertical servo (180) */
#define VERTICAL_IMAGE_TO_SERVO_ADJUSTMENT (40.5/180.0)	    // this seems to work
#define DAMPING_FACTOR 0.75

#define SERVO_DEADBAND 0.01
#define SCORE_MINIMUM 0.01
#define SCORE_GOOD 0.4

#define DISTORTION_DELTA_MAX 0.25

//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
//
#define VISION_CYCLE_TIME (100) // 100ms

class DashboardDataSender;

/*
 * Team 166 Vision Class
 * 
 */
class Team166Vision : public Team166Task
{
public:
	Team166Vision(void);
	virtual ~Team166Vision(void);
	
	//! Main function of the Vision task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);	

	/**
	 * @return bool Whether the target has been acquired.
	 */
	bool IsTargetAcquired(void);
	/**
	 * The bearing to the target in degrees.
	 * @return float Degrees to the angle, 0 being full left and 180 being full right. 
	 */
	float GetBearing(void);
	/**
	 * @brief Tries to start the camera using vxWorks threads, since
	 * StartCamera() will hang the thread if it can't successfully start
	 * the camera. Will try to start the camera several times, timing out 
	 * after a predefined amount of time.
	 */
	void TryStartCamera(bool);
	/**
	 * Allocates a pointer, and stores a captured image into it.
	 * @return ColorImage Captured image.
	 */
	ColorImage *GetImage();
	
	/**
	 * Gets the score horizontally to the target.
	 * @return float Score value, -1.0 being full left and 1.0 being full right.
	 */
	float GetScoreToTargetX();
	/**
	 * Gets the score vertically to the target.
	 * @return float Score value, -1.0 being down and 1.0 being up.
	 */
	float GetScoreToTargetY();
// Private functions and attributes
private:	
	ColorMode colorMode;
	
	//! Target has been acquired since last run
	bool targetAcquired; 
	//! Whether the image ( Proxy166::GetImage() ) is new or not. 
	bool staleFlag; 
	//! Angle horizontally to the target
	float bearing;
	//! Angle vertically to the target
	float tilt; 
	
	//! -1.0 to 1.0 score to target x-axis bearing, independent of servo positions, to be passed to the drive functions.
	float score_to_target_x;
	//! -1.0 to 1.0 score to target y-axis tilt, independent of servo positions, to be passed to the drive functions.
	float score_to_target_y;
	
	Servo horizontalServo;
	Servo verticalServo;
	
	DashboardDataSender *dds;

	/**
	 * @brief The main function of the threads spawned in TryStartCamera(). 
	 * Spawned via taskSpawn vxWorks function. 
	 * Calls StartCamera(), which will hang the calling thread in a variety of circumstances.
	 */
	static int _StartCameraThreadFunc(void *this_p,int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
	/**
	 * @brief Tries to find the target in the current view of the camera.
	 * @param matches A vector to store the matches for elipses in.
	 * @param psx Previous servo X value. Will be used later for smoothing movement.
	 * @param psy Previous servo Y value. Will be used later for smoothing movement.
	 */
	void AcquireTarget(vector<Target>& matches,float& psx,float& psy);
	//! Sets the servo positions directly.
	void SetServoPositions(float normalizedHorizontal, float normalizedVertical);
	//! Sets the servo positinos from image x and y values. 
	void AdjustServoPositions(float normDeltaHorizontal, float normDeltaVertical);
	//! Directly maps x and y to servo X and Y values. 
	void _SetServoPositions(float servoHorizontal, float servoVertical);
};
#endif // !defined(_VISION166_H)
