/*******************************************************************************
*  Project   		: Framework
*  File Name  		: Proxy166.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Header for Proxy class
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/
#pragma once

#include "WPILib.h"
#include "Utility.h"
#include "Team166Task.h"
#include "FrcError.h"

#define NUMBER_OF_JOYSTICKS (4)
#define NUMBER_OF_SWITCHES (10)
#define NUMBER_OF_JOY_BUTTONS (12)

//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
//
#define PROXY_CYCLE_TIME (25) // 25ms

/**
 * @brief Proxy Joystick class that will be returned if a full cached joystick is requested.
 * 
 * If Proxy1666::GetJoystick is called, then a full joystick should be returned. 
 */
class ProxyJoystick {
	public:
		float X;
		float Y;
		float Z;
		float throttle;
		bool button[NUMBER_OF_JOY_BUTTONS];
		
		ProxyJoystick(void);
		static ProxyJoystick Copy(Joystick input);
};

/**
 * @brief Proxy class to store cached values for joysticks and switches.
 * 
 * This class will store the cached values for joysticks and switches. It will also
 * serve as a interface between the joysticks and switches, and any code that wants to 
 * access them like Drive, Autonomous, etc. 
 */

class Proxy166 : public Team166Task {
	public:		
		
		// joystick axes
		void SetJoystickX(int, float);  
		void SetJoystickY(int, float);
		void SetJoystickZ(int, float);
		
		float GetJoystickX(int);
		float GetJoystickY(int);
		float GetJoystickZ(int);
		
		// driver station switches
		void SetSwitch(int, int);
		int GetSwitch(int);
		
		// joystick buttons
		void SetButton(int,int,bool);
		bool GetButton(int,int,bool=false);
		
		void RegisterCounter(int,int);
		void UnregisterCounter(int,int);
		int GetPendingCount(int,int);
		bool IsRegistered(int,int);
		
		// joystick throttle
		void SetThrottle(int,float);
		float GetThrottle(int);

		// joystick trigger (same as switch #1)
		void SetTrigger(int,bool);
		bool GetTrigger(int,bool=false);

		// internal representation of joystick as a whole
		ProxyJoystick GetJoystick(int);
		
		Proxy166(void);
		~Proxy166(void);
		
		bool AreSettingJoysticks();
		void ToggleSettingJoysticks(bool);
		
		static Proxy166 *getInstance(void);
		
		virtual int Main(int a2, int a3, int a4, int a5,
					int a6, int a7, int a8, int a9, int a10);
	private:
		/**
		 * @brief The single instance handle to Proxy166.
		 */
		static Proxy166* ProxyHandle;
		
		// internal method to get values from real joystick
		void SetJoystick(int,Joystick&);

		// proxy storage for driver station switches
		int Switches[NUMBER_OF_SWITCHES];
		
		// proxy storage for driver station joysticks
		ProxyJoystick Joysticks[NUMBER_OF_JOYSTICKS];
		
		/**
		 * A tuple of ints. For every tracked button, there is three
		 * ints in this tuple: first, the joystick number, second, the button number,
		 * and third, the number of times it has been pressed.
		*/
		vector<int> tracker;


		Joystick driveStickRight;     // joy stick for driving
		Joystick driveStickLeft;      // joy stick for driving
		Joystick driveStickCopilot;	  // Camera / copilot joy stick
		
		// Battery level
		float Battery;
		
		
		bool areSettingJoysticks;
};
