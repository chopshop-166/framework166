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

#ifndef _T166_DEFINES_H
#define _T166_DEFINES_H


// Place any #defines for things you want to enable in multiple tasks, simply, here

//
// Analog Pin layout
// _MOD refers to cRIO module (1 or 2)
// If module 2 is used, make sure you have 2 sidecars
// Other stuff is the channel in the module
//
#define T166_ANALOG_MODULE_1 (1)
#define T166_ANALOG_MODULE_2 (2)

// first analog module
#define T166_BAT_MOD (T166_ANALOG_MODULE_1) // Module for battery sensor
#define T166_BAT_VOLT (8)                   // Channel for battery voltage

//
// Digital Pin layout
//
// Place #defines for the digital pins here

//
// PWM layout
//
// Place #defines for the PWMs here

//
// Relay Layout
//

//
// Pneumatics
//

//
// CAN bus layout
//
/** Maximum Jaguar id --Replace the 2 with the highest jaguar id */
#define T166_NUM_JAGS (2)

//
// USB port layout
//
// Place #defines for the Joysticks layout here
/** Left driver stick is in USB port #1 */
#define T166_DRIVER_STICK_LEFT (1)
/** Right driver stick is in USB port #2 */
#define T166_DRIVER_STICK_RIGHT (2)
/** Copilot stick is in USB port #3 */
#define T166_COPILOT_STICK (3)
//
// Joystick buttons
//
// Place #defines for the button layout here

#endif // !defined(_T166_DEFINES_H)
