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
#include <sstream>

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
			string switchwid = "Switch";
			stringstream switchtochar;
			//turn the switch number into a string
			switchtochar << switchid;
			//concat the switch number with "switch"
			switchwid += switchtochar.str();
			//Add switches to storage
			add(switchwid);
		}
		// Lets do this the easy way:
		for (int joyid=1; joyid <NUMBER_OF_JOYSTICKS+1; joyid++) {
			//Define string for holding joyid
			string joywid = "Joy";
			std::stringstream numtochar;
			numtochar << joyid;
			joywid += numtochar.str();
			add(joywid + "X");
			add(joywid + "Y");
			add(joywid + "Z");
			add(joywid + "T");
			add(joywid + "BT");
			//Add Buttons, and newpress
			for (int buttonid=1;buttonid<NUMBER_OF_JOY_BUTTONS+1;buttonid++) {
				string butwid = "B";
				stringstream buttochar;
				buttochar << buttonid;
				butwid += buttochar.str();
				add(joywid + butwid);
				add(joywid + butwid + "N");
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
			SetJoystick(0, stick1);
			SetJoystick(1, stick2);
			SetJoystick(2, stick3);
			SetJoystick(3, stick4);
			
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
			string name;
			stringstream ss;
			ss << "Joy" << joy_id << "B" << btn_id;
			ss >> name;
			if(get(name+"N") || !get(name)) {
				// Either there's an old "newpress", or the new value is "false"
				set(name+"N",false);
			} else {
				set(name+"N",true);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

// This must be called to add values to the data field
bool Proxy::add(string name)
{
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
	wpi_assert(data.find(name) != data.end());
	semTake(data[name].second, WAIT_FOREVER);
	data[name].first = val;
	semGive(data[name].second);
	return val;
}

// Stop tracking a variable in Proxy
bool Proxy::del(string name)
{
	if(data.find(name) != data.end()) {
		semTake(data[name].second, WAIT_FOREVER);
		semDelete(data[name].second);
		data.erase(name);
		return true;
	} else {
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Sets a cached joystick value.
 * @param joy_id Which joystick to set the cached value for.
 * @param stick A Joystick object with the X, Y, and Z axes set, as well as each of the buttons.
 */
void Proxy::SetJoystick(int joy_id, Joystick & stick)
{
	wpi_assert(joy_id < NUMBER_OF_JOYSTICKS && joy_id >= 0);
	std::stringstream ss;
	string name;
	ss << "Joy" << joy_id+1;
	ss >> name;
	set(name + 'X', stick.GetX());
	set(name + 'Y', stick.GetY());
	set(name + 'Z', stick.GetZ());
	set(name + 'T', stick.GetThrottle());
	string bname;
	for(unsigned i=0;i<NUMBER_OF_JOY_BUTTONS;i++) {
		ss << name << "B" << i+1;
		ss >> bname;
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
int Proxy::GetPendingCount(int joystick_id, int button_id) {
	wpi_assert(joystick_id < NUMBER_OF_JOYSTICKS && joystick_id >= 0);
	wpi_assert(button_id < NUMBER_OF_JOY_BUTTONS && button_id >= 0);
	
	if(tracker.size() == 0)
		wpi_assertWithMessage(false, "Tried to fetch pending count for a non-registered button.");
	vector<int>::iterator it = tracker.begin();
	while(it != tracker.end())
	{
		if(*it == joystick_id && *(it+1) == button_id) {
			return *(it+2);
		}
		it += 3;
	}
	wpi_assertWithMessage(false, "Tried to fetch pending count for a non-registered button.");
	return 0;
}

/**
 * @brief Register a button to track the number of times it was pressed.
 * @param joystick_id Which joystick to track a button on
 * @param button_idd Which button on the joystick to track
 */
void Proxy::RegisterCounter(int joystick_id, int button_id) {
	wpi_assert(joystick_id < NUMBER_OF_JOYSTICKS && joystick_id >= 0);
	wpi_assert(button_id < NUMBER_OF_JOY_BUTTONS && button_id >= 0);
	
	if(tracker.size() != 0) {
		vector<int>::iterator it = tracker.begin();
		while(it >= tracker.end())
		{
			if(*it == joystick_id && *(it+1) == button_id) {
				return;
			}
			it+=3;
		}
	}
	tracker.push_back(joystick_id);
	tracker.push_back(button_id);
	tracker.push_back(0);
}

/**
 * @brief Unregister a button to track the number of times it was pressed.
 * @param joystick_id Which joystick to track a button on
 * @param button_idd Which button on the joystick to track
 */
void Proxy::UnregisterCounter(int joystick_id, int button_id) {
	wpi_assert(joystick_id < NUMBER_OF_JOYSTICKS && joystick_id >= 0);
	wpi_assert(button_id < NUMBER_OF_JOY_BUTTONS && button_id >= 0);
	
	if(tracker.size() == 0)
		return;
	vector<int>::iterator it = tracker.begin();
	while(it >= tracker.end())
	{
		if(*it == joystick_id && *(it+1) == button_id) {
			tracker.erase(it, it+2);
		}
		it+=3;
	}
}
/**
 * @brief Whether a joystick is registered for tracking
 * @param joystick_id What joystick to check
 * @param button_id What button on the joystick to check.
 * @return Whether it is registered.
 */
bool Proxy::IsRegistered(int joystick_id, int button_id) {
	wpi_assert(joystick_id < NUMBER_OF_JOYSTICKS && joystick_id >= 0);
	wpi_assert(button_id < NUMBER_OF_JOY_BUTTONS && button_id >= 0);
	
	if(tracker.size() == 0)
		return false;
	vector<int>::iterator it = tracker.begin();
	while(it >= tracker.end())
	{
		if(*it == joystick_id && *(it+1) == button_id) {
			return true;
		}
		it+=3;
	}
	return false;
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
