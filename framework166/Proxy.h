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
//This is how often the Driver Station sends back data.
//
#define PROXY_CYCLE_TIME (20) // 20ms

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
		bool exists(string);
		
		bool RegisterCounter(string);
		bool UnregisterCounter(string);
		int GetPendingCount(string);
		bool IsRegistered(string);
		
		~Proxy(void);
		
		bool AreSettingJoysticks();
		void ToggleSettingJoysticks(bool);
		void OverrideJoystick(int,bool);
		bool IsSettingJoystick(int);
		
		static Proxy *getInstance(void);
		
		virtual int Main(int a2, int a3, int a4, int a5,
					int a6, int a7, int a8, int a9, int a10);
	private:
		Proxy(void);
		// Handle to the proxy
		static Proxy *ProxyHandle;
		
		// internal method to get values from real joystick
		void SetJoystick(int,Joystick&);
		void setNewpress(void);
		
		/**
		 * A tuple of ints. For every tracked button, there is three
		 * ints in this tuple: first, the joystick number, second, the button number,
		 * and third, the number of times it has been pressed.
		*/
		//vector<int> tracker;
		static map<string,int> tracker;
		static short newpress_values[NUMBER_OF_JOYSTICKS][NUMBER_OF_JOY_BUTTONS];

		// The physical joysticks
		Joystick stick1;
		Joystick stick2;
		Joystick stick3;
		Joystick stick4;
		
		bool wasEnabled;
		bool manualJoystick[4];
};
