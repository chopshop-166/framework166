/*******************************************************************************
*  Project   		: chopshop10 - 2010 Chopshop Robot Controller Code
*  File Name  		: Vision166.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  Revision History	: From Explorer with TortoiseSVN, Use "Show log" menu item
*  File Description	: Robot code which handles vision of camera
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "Vision166.h"
#include <math.h>
#include "Robot166.h"
#include <vector>
#include "RobotCamera166.h"

#include "Vision/AxisCamera.h" 
#include "BaeUtilities.h"
#include "FrcError.h"
#include "nivision.h"
#include "Target.h"
#include "DashboardDataSender.h"

#define DPRINTF if(false)dprintf

Team166Vision::Team166Vision(void) :
	colorMode(IMAQ_HSL), 			    // Color mode (RGB or HSL) for image processing	
	targetAcquired(false),			// target not acquired
	bearing(0.0),					// current horizontal normalized servo position	
	tilt(0.0),						// current vertical normalized servo position	
	horizontalServo(T166_HORIZONTAL_SERVO_CHANNEL),
	verticalServo(T166_VERTICAL_SERVO_CHANNEL),
	dds(DashboardDataSender::getInstance())
{
	/* set up debug output: 
	 * DEBUG_OFF, DEBUG_MOSTLY_OFF, DEBUG_SCREEN_ONLY, DEBUG_FILE_ONLY, DEBUG_SCREEN_AND_FILE 
	 */
	SetDebugFlag ( DEBUG_SCREEN_ONLY  ) ;
	
	// Start our task
	Start((char *)"166VisionTask", VISION_CYCLE_TIME);	
};

Team166Vision::~Team166Vision(void)
{		
	return;
};
/**
 * Set servo positions (0.0 to 1.0) 
 * 
 * @param servoHorizontal Pan Position from 0.0 to 1.0.
 * @param servoVertical Tilt Position from 0.0 to 1.0.
 */
void Team166Vision::_SetServoPositions(float servoHorizontal, float servoVertical)	{
	
	float currentH = horizontalServo.Get();		
	float currentV = verticalServo.Get();
	
	/* make sure the movement isn't too small */
	if ( fabs(servoHorizontal - currentH) > SERVO_DEADBAND ) {
		horizontalServo.Set( servoHorizontal );
		/* save new normalized horizontal position */
		bearing = RangeToNormalized(servoHorizontal, 1);
	}
	if ( fabs(servoVertical - currentV) > SERVO_DEADBAND ) {
		// don't look straight up or down
		if (servoVertical > 0.9) servoVertical = 0.9;
		if (servoVertical < 0.1) servoVertical = 0.1;
		
		verticalServo.Set( servoVertical );
		/* save new normalized vertical position */
		tilt = RangeToNormalized(servoVertical, 1);
	}
}	

/**
 * Set servo positions (0.0 to 1.0) translated from normalized values (-1.0 to 1.0). 
 * 
 * @param normalizedHorizontal Pan Position from -1.0 to 1.0.
 * @param normalizedVertical Tilt Position from -1.0 to 1.0.
 */
void Team166Vision::SetServoPositions(float normalizedHorizontal, float normalizedVertical)	{
	float servoH = NormalizeToRange(normalizedHorizontal);
	float servoV = NormalizeToRange(normalizedVertical);
	_SetServoPositions(servoH, servoV);
}	
/**
 * @brief Adjust servo positions (0.0 to 1.0) translated from normalized values (-1.0 to 1.0). 
 * Inputs are normalized values from an image with field of view 
 * horizontal 54 degrees/vertical 40.5 degrees. This value is 
 * multiplied by a factor to correspond to the 180 degree H/V 
 * range of the servo. 
 * 
 * @param normalizedHorizontal Pan adjustment from -1.0 to 1.0.
 * @param normalizedVertical Tilt adjustment from -1.0 to 1.0.
 */
void Team166Vision::AdjustServoPositions(float normDeltaHorizontal, float normDeltaVertical)	{
			
	//NORMALIZED
	/* adjust for the fact that servo overshoots based on image input */
	normDeltaHorizontal *= HORIZONTAL_IMAGE_TO_SERVO_ADJUSTMENT;
	normDeltaVertical *= VERTICAL_IMAGE_TO_SERVO_ADJUSTMENT;
	
	/* compute horizontal goal */
	float currentH = horizontalServo.Get();  //servo range
	float normCurrentH = RangeToNormalized(currentH, 1);
	float normDestH = normCurrentH + normDeltaHorizontal;	
	
	/* narrow range keep servo from going too far */
	if (normDestH > 1.0) normDestH = 1.0;
	if (normDestH < -1.0) normDestH = -1.0;		

	/* compute vertical goal */
	float currentV = verticalServo.Get();  //servo range
	float normCurrentV = RangeToNormalized(currentV, 1);
	float normDestV = normCurrentV + normDeltaVertical;	
	
	if (normDestV > 1.0) normDestV = 1.0;
	if (normDestV < -1.0) normDestV = -1.0;
	
	//SERVO RANGE
	/* convert input to servo range */
	float servoH = NormalizeToRange(normDestH);
	//float servoV = NormalizeToRange(normDestV, 0.2, 0.8);
	float servoV = NormalizeToRange(normDestV);

	_SetServoPositions(servoH, servoV);
}

/*
 * @brief Processes images to find a target, then pans the camera so that 
 * 		we are looking at the target. 
 * 
 * @return bool Whether we found the target; copy of Team166Vision::targetAcquired
 */
bool Team166Vision::IsTargetAcquired() {
	return targetAcquired;
}

/*
 * @brief Returns a float -1.0 to 1.0 returning the target's relative position to the front right corner of the robot. 
 * 
 * @return float Bearing -1.0 to 1.0
 */
float Team166Vision::GetBearing() {
	return bearing;
}

void Team166Vision::AcquireTarget(vector<Target> & matches, float & prev_servo_x, float & prev_servo_y) {
	static Timer *debug; // Persistent (but local) debug timer
	Proxy166 *pHandle = Proxy166::getInstance();
	if(pHandle == NULL)
		return;
	// These are persistent so that we don't have to keep re-alocating them.
	static float delta_x;
	static float delta_y;
	static float servo_x;
	static float servo_y;
	static float delta_servo_x;
	static float delta_servo_y;
	static float x;
	static float y;
	
	if(NULL == debug) { 
		debug = new Timer;
		debug->Start();
	}
	if(matches.size() > 0 && (1 - (matches[0].m_majorRadius / matches[0].m_minorRadius)) < DISTORTION_DELTA_MAX && matches[0].m_score > SCORE_MINIMUM) {
		targetAcquired = 1;
		
		delta_x = HORIZONTAL_IMAGE_TO_SERVO_ADJUSTMENT * matches[0].m_xPos * 0.875;
		delta_y = VERTICAL_IMAGE_TO_SERVO_ADJUSTMENT * matches[0].m_yPos * 0.875;
		
		servo_x = horizontalServo.Get();
		servo_y = verticalServo.Get();
		
		x = 0.5 - (NormalizeToRange(servo_x, -1.0, 1.0) + delta_x);
		y = 0.5 - (NormalizeToRange(servo_y, -1.0, 1.0) + delta_y);
		
		x *= -1.0;
		y *= -1.0;
		
		delta_x = fabs(matches[0].m_xPos);
		delta_y = fabs(matches[0].m_yPos);
		
		delta_servo_x = prev_servo_x - x;
		delta_servo_y = prev_servo_y - y;
		
		score_to_target_x = x;
		score_to_target_y = y;
		
		pHandle->SetCameraScoreToTargetX(score_to_target_x);
		
		prev_servo_x = x;
		prev_servo_y = y;
		
		bearing = 90.0 + (x * 90.0);
		pHandle->SetCameraBearing(bearing);
		
		if(debug->HasPeriodPassed(0.5)) {
			debug->Reset();
			DPRINTF(LOG_DEBUG, "DSx %f DSy %f\n", delta_servo_x, delta_servo_y);
		}
	}
}
ColorImage *Team166Vision::GetImage() {
	ColorImage *result = new ColorImage(IMAQ_IMAGE_HSL);
	if(camera166 != NULL)
		camera166->GetImage(result);
	return result;
}
int Team166Vision::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{	
	Robot166 *lHandle;
	Proxy166 *pHandle;
	
	DPRINTF(LOG_DEBUG, "Vision task initializing the camera...\n");
	TryStartCamera(false);
	DPRINTF(LOG_DEBUG, "Waiting for autonomous or operator control...\n");
	WaitForGoAhead();
	
	lHandle = Robot166::getInstance();
	pHandle = Proxy166::getInstance();
	
	SetServoPositions(0.0, DEFAULT_VERTICAL_PAN_POSITION);
	
	Timer debugTimer;
	debugTimer.Start();
	
	vector<Target> matches;
	ColorImage *img;
	
	static float prev_servo_x = 0.0, prev_servo_y = DEFAULT_VERTICAL_PAN_POSITION;
	
	while (MyTaskInitialized)
	{
		if(true == pHandle->GetVisionStatus()) {
			// Vision is enabled
			img = GetImage();
			pHandle->SetImage(img);
			if(0 != img) {
				// We successfully grabbed an image
				matches = Target::FindCircularTargets(img);
				AcquireTarget(matches, prev_servo_x, prev_servo_y);
				
				// Disable image overlay for all but the best selection.
				matches.erase(matches.begin()+1, matches.end());
				
				dds->sendVisionData(
					0.0, 
					0.0, 
					0.0, 
					matches[0].m_xPos / matches[0].m_xMax, 
					matches
				);
				//DPRINTF(LOG_DEBUG, "HzS = %f ; VlS = %f\n", horizontalServo.Get(), verticalServo.Get());			
				
				// This method should only be called during Autonomous, but for testing
				// we should call it during any mode. 
				//DriveTowardsTarget();

				if(pHandle->GetCameraScoreToTargetX() < 0) {
					// The target is left of the robot
					SetStatus("Left");
				}
				else {
					SetStatus("Right");
				}

				pHandle->DeleteImage();
			}
		}
		else {
			SetStatus("Error");
		}
		// Controlling the servos based on joystick input
		float Jy = pHandle->GetJoystickY(3);
		if(FLIP_SERVO_VERTICAL) {
			Jy *= -1.0;
		}
		if(lHandle->IsAutonomous() || lHandle->IsOperatorControl()) {
			SetServoPositions(pHandle->GetJoystickX(3), Jy);
		}
		
		if(debugTimer.HasPeriodPassed(3.0)) {
			debugTimer.Reset();
			DPRINTF(LOG_DEBUG, "[img %d] [Ly %f] [Ry %f]\n", 
					img, pHandle->GetJoystickY(DRIVE_JOYSTICK_LEFT), pHandle->GetJoystickY(DRIVE_JOYSTICK_RIGHT)
			);
		}

		WaitForNextLoop();		
	}
	return 0;
}
int Team166Vision::_StartCameraThreadFunc(void *this_p,int a2, int a3, int a4, int a5,
		int a6, int a7, int a8, int a9, int a10) {
	Proxy166 *pHandle = Proxy166::getInstance();
	while(0 == pHandle) {
		// Wait for the Proxy to initialize - we'll be setting the Vision Status in it. 
		pHandle = Proxy166::getInstance();
		Wait(0.05);
	}
	
	// This will hang the calling thread if it fails to initialize, or if a misc error occurs.
	StartCamera();
	if(0 != (&AxisCamera::GetInstance())) {
		pHandle->SetVisionStatus(true);
		return true;
	}
	else {
		return false;
	}
}
void Team166Vision::TryStartCamera(bool long_wait) {
	int id = -1;
	
	Proxy166 *pHandle = Proxy166::getInstance();
	while(0 == pHandle) {
		pHandle = Proxy166::getInstance();
		Wait(0.05);
	}
	pHandle->SetVisionStatus(false);
	
	Wait(3.5);
	
	Timer threadTimer; // Waits for the timout on the specific thread. 
	threadTimer.Start();
	
	Timer cameraTimer; // Waits for the timeout on this function.
	cameraTimer.Start();
	do {
		if(id != -1 && threadTimer.HasPeriodPassed(CAMERA_SPAWN_TRY_WAIT)) {
			// Already ran at least once; restart old thread
			taskRestart(id);
			threadTimer.Reset();
			DPRINTF(LOG_DEBUG, "Restarting VisionSpawner...\n");
		}
		else if (id == -1) { 
			// First time running this loop
			DPRINTF(LOG_DEBUG, "Starting VisionSpawner...\n");
			id = taskSpawn((char*)"VisionSpawner", 90, VX_FP_TASK, 5000, (FUNCPTR) _StartCameraThreadFunc,0,0,0,0,0,0,0,0,0,0);
		}
	} while(
			false == pHandle->GetVisionStatus() && !cameraTimer.HasPeriodPassed(CAMERA_SPAWN_WAIT_MAX)
	);
	if(true == pHandle->GetVisionStatus())
		DPRINTF(LOG_DEBUG, "Started camera.\n");
	else
		DPRINTF(LOG_DEBUG, "Gave up on starting camera.\n");
}
