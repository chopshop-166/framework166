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
#include <map>

#define NUMBER_OF_JOYSTICKS (4)
#define NUMBER_OF_SWITCHES (10)
#define NUMBER_OF_JOY_BUTTONS (12)

//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
//
#define PROXY_CYCLE_TIME (25) // 25ms

/**
 * @brief Proxy class to store cached values for joysticks and switches.
 * 
 * This class will store the cached values for joysticks and switches. It will also
 * serve as a interface between the joysticks and switches, and any code that wants to 
 * access them like Drive, Autonomous, etc. 
 */

class Proxy : public Team166Task{
	private:
		static map<string,pair<float, SEM_ID> > data;
	public:
		// Proxy 2.0 functions
		bool add(string);
		float get(string, bool=false);
		float set(string, float);
		bool del(string);
		
		// joystick axes
		void SetJoystickX(int, float);  
		void SetJoystickY(int, float);
		void SetJoystickZ(int, float);
		
		float GetJoystickX(int);
		float GetJoystickY(int);
		float GetJoystickZ(int);
		
		// joystick buttons
		void SetButton(int,int,bool);
		bool GetButton(int,int,bool=false);
		bool GetNewpress(int,int);
		
		void RegisterCounter(int,int);
		void UnregisterCounter(int,int);
		int GetPendingCount(int,int);
		bool IsRegistered(int,int);
		
		Proxy(void);
		~Proxy(void);
		
		bool AreSettingJoysticks();
		void ToggleSettingJoysticks(bool);
		
		static Proxy *getInstance(void);
		
		virtual int Main(int a2, int a3, int a4, int a5,
					int a6, int a7, int a8, int a9, int a10);
	private:
		// internal method to get values from real joystick
		void SetJoystick(int,Joystick&);
		void setNewpress(void);
		
		/**
		 * A tuple of ints. For every tracked button, there is three
		 * ints in this tuple: first, the joystick number, second, the button number,
		 * and third, the number of times it has been pressed.
		*/
		vector<int> tracker;

		// The physical joysticks
		Joystick stick1;
		Joystick stick2;
		Joystick stick3;
		Joystick stick4;
		
		// Battery level
		float Battery;
				
		bool areSettingJoysticks;
};
