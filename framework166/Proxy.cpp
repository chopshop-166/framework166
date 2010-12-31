/*******************************************************************************
*  Project   		: Framework
*  File Name  		: Proxy166.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Code for Proxy class to hold shared variables
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "wpilib.h"
#include "Robot.h"
#include <string>

// Enable proxy logging?
#define LoggingProxy (0)

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

map<string,pair<float, SEM_ID> > Proxy::data = map<string,pair<float, SEM_ID> >();

Proxy *Proxy::ProxyHandle = 0;

/**
 * @brief Starts the Proxy166 task.
 */
Proxy::Proxy(void):
	stick1(1), stick2(2), stick3(3), stick4(4),
	areSettingJoysticks(true)
{
	//
	// Add the built in storage areas
	//
	static bool runonce = 0;
	if (runonce == 0) {
		ProxyHandle = this;
		for (int switchid=1; switchid < NUMBER_OF_SWITCHES+1; switchid++) {
			char tmp[32];
			sprintf(tmp, "Switch%d", switchid);
			string switchwid = tmp;
			//Add switches to storage
			add(switchwid);
		}
		// Lets do this the easy way:
		for (int joyid=1; joyid <NUMBER_OF_JOYSTICKS+1; joyid++) {
			char tmp[32];
			sprintf(tmp, "Joy%d", joyid);
			string joywid = tmp;
			add(joywid + "X");
			add(joywid + "Y");
			add(joywid + "Z");
			add(joywid + "T");
			add(joywid + "BT");
			//Add Buttons, and newpress
			for (int buttonid=1;buttonid<NUMBER_OF_JOY_BUTTONS+1;buttonid++) {
				char tmp[32];
				sprintf(tmp, "%sB%d", joywid.c_str(), buttonid);
				string butwid = tmp;
				add(butwid);
				add(butwid + "N");
			}
		}
		//Make sure they're only added once
		runonce = 1;
	}
	// Start the actual task
	Start((char *)"166ProxyTask", PROXY_CYCLE_TIME);
}

Proxy::~Proxy(void)
{
	for(map<string,pair<float, SEM_ID> >::iterator it = data.begin();it != data.end();it = data.begin()) {
		del(it->first);
	}
	return;
}
/**
 * @brief Main thread function for Proxy166.
 * Runs forever, until MyTaskInitialized is false. 
 * 
 * @todo Update DS switch array
 */
int Proxy::Main(	int a2, int a3, int a4, int a5,
					int a6, int a7, int a8, int a9, int a10) {

	Robot *lHandle = NULL;
#if LoggingProxy
	ProxyLog sl;
#endif
	WaitForGoAhead();
	
	lHandle = Robot::getInstance();
#if LoggingProxy
	lHandle->RegisterLogger(&sl);
#endif
	
	Timer debugTimer;
	debugTimer.Start();
	
	while(MyTaskInitialized) {
		setNewpress();
		if(lHandle->IsOperatorControl() && true == AreSettingJoysticks()) {
			SetJoystick(1, stick1);
			SetJoystick(2, stick2);
			SetJoystick(3, stick3);
			SetJoystick(4, stick4);
			
			if(debugTimer.HasPeriodPassed(1.0)) {
				// Debug info
			}
		}
		// The task ends if it's not initialized
		WaitForNextLoop();
	}
	
	return 0;
}

void Proxy::setNewpress()
{
	for(unsigned joy_id=1;joy_id < NUMBER_OF_JOYSTICKS+1; joy_id++) {
		for(unsigned btn_id=1;btn_id < NUMBER_OF_JOY_BUTTONS+1; btn_id++) {
			char tmp[32];
			sprintf(tmp, "Joy%dB%d", joy_id, btn_id);
			string name = tmp;
			bool lastvalue[4][12];
			bool buttonval = (bool)get(name);
			if (lastvalue[joy_id-1][btn_id-1] == buttonval){
				if(name == "Joy1B1") {
					printf("First");
				}
			} else if(buttonval == 0) {
				if(name == "Joy1B1") {
					printf("Second\n");
				}
				set(name + "N", 0.0);
			} else if(buttonval == 1) {
				if(name == "Joy1B1") {
					printf("Third  ");
					printf("Last: %d Cur: %d\n", lastvalue[joy_id-1][btn_id-1], buttonval);
				}
				set(name + "N", 1.0);
			}
			lastvalue[joy_id-1][btn_id-1] = buttonval;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

// This must be called to add values to the data field
bool Proxy::add(string name)
{
	for(unsigned i=0;i<name.size();i++) {
		name[i] = toupper(name[i]);
	}
	if(data.find(name) == data.end()) {
		data[name] = make_pair(0.0,semBCreate(SEM_Q_PRIORITY, SEM_FULL));
		return true;
	} else {
		return false;
	}
}

// This will get a Proxy value
float Proxy::get(string name, bool reset)
{
	for(unsigned i=0;i<name.size();i++) {
		name[i] = toupper(name[i]);
	}
	wpi_assert(data.find(name) != data.end());
	semTake(data[name].second, WAIT_FOREVER);
	float ret = data[name].first;
	data[name].first = (reset)? 0 : data[name].first;
	semGive(data[name].second);
	return ret;
}

// Set a new proxy value
float Proxy::set(string name, float val)
{
	for(unsigned i=0;i<name.size();i++) {
		name[i] = toupper(name[i]);
	}
	wpi_assert(data.find(name) != data.end());
	semTake(data[name].second, WAIT_FOREVER);
	data[name].first = val;
	semGive(data[name].second);
	return val;
}

// Stop tracking a variable in Proxy
bool Proxy::del(string name)
{
	for(unsigned i=0;i<name.size();i++) {
		name[i] = toupper(name[i]);
	}
	if(data.find(name) != data.end()) {
		semTake(data[name].second, WAIT_FOREVER);
		semDelete(data[name].second);
		data.erase(name);
		return true;
	} else {
		return false;
	}
}

/**
 * @brief Sets a cached joystick value.
 * @param joy_id Which joystick to set the cached value for.
 * @param stick A Joystick object with the X, Y, and Z axes set, as well as each of the buttons.
 */
void Proxy::SetJoystick(int joy_id, Joystick & stick)
{
	wpi_assert(joy_id < NUMBER_OF_JOYSTICKS+1 && joy_id >= 0);
	string name;
	char tmp[32];
	sprintf(tmp, "Joy%d", joy_id);
	name = tmp;
	set(name + 'X', stick.GetX());
	set(name + 'Y', stick.GetY());
	set(name + 'Z', stick.GetZ());
	set(name + 'T', stick.GetThrottle());
	string bname;
	for(unsigned i=1;i<NUMBER_OF_JOY_BUTTONS+1;i++) {
		char tmp1[32];
		sprintf(tmp1, "%sB%d", name.c_str(), i);
		bname = tmp1;
		set(bname,stick.GetRawButton(i));
		
	}
	set(name+"BT", stick.GetTrigger());
}

/**
 * @brief Gets the pending amount of times a button was pressed and released since last call.
 * @param joystick_id Which joystick to check
 * @param button_id Which button on the joystick to check
 * @return How many times the button was pressed and released since last call.
 */
int Proxy::GetPendingCount(string JoyButton) {
	for(unsigned i=0;i<JoyButton.size();i++) {
		JoyButton[i] = toupper(JoyButton[i]);
	}
	wpi_assertWithMessage(tracker.size() == 0, "Tried to fetch pending count for a non-registered button.");
	map<string,int>::iterator it = tracker.find(JoyButton);
	wpi_assertWithMessage(it != tracker.end(), "Tried to fetch pending count for a non-registered button.");
	return (it->second);
}

/**
 * @brief Register a button to track the number of times it was pressed.
 * @param joystick_id Which joystick to track a button on
 * @param button_idd Which button on the joystick to track
 */
bool Proxy::RegisterCounter(string JoyButton) {
	for(unsigned i=0;i<JoyButton.size();i++) {
		JoyButton[i] = toupper(JoyButton[i]);
	}
	if(tracker.find(JoyButton) == tracker.end()) {
		tracker[JoyButton] = 0;
		return true;
	} else {
		return false;
	}
}

/**
 * @brief Unregister a button being tracked.
 * @param joystick_id Which joystick to track a button on
 * @param button_idd Which button on the joystick to track
 */
bool Proxy::UnregisterCounter(string JoyButton) {
	for(unsigned i=0;i<JoyButton.size();i++) {
		JoyButton[i] = toupper(JoyButton[i]);
	}
	return tracker.erase(JoyButton);
}
/**
 * @brief Whether a joystick is registered for tracking
 * @param JoyButton Joystick and Button 
 * @return Whether it is registered.
 */
bool Proxy::IsRegistered(string JoyButton) {
	for(unsigned i=0;i<JoyButton.size();i++) {
		JoyButton[i] = toupper(JoyButton[i]);
	}
	return (tracker.find(JoyButton) != tracker.end());
}

Proxy* Proxy::getInstance(void)
{
	if(ProxyHandle == 0) {
		ProxyHandle = new Proxy;
	}
	return ProxyHandle;
}

bool Proxy::AreSettingJoysticks() {
	return areSettingJoysticks;
}
void Proxy::ToggleSettingJoysticks(bool in) {
	in = areSettingJoysticks;
}
