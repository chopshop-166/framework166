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


// If we're using a camera, enable this
#define UsingCamera (0)
// Are we using the CAN bus?
#define UsingCan (1)
// Are we on suitcasebot?
#define UsingSuitcase (0)
// Enable autobalance
#define UsingAutobalance (1)

// Code version number
#define T166_CODE_VERSION ("CODE: BATTLECRY-04")

//
// Analog Pin layout
// _MOD refers to cRIO module (1 or 2)
// If module 2 is used, make sure you have 2 sidecars
// Other stuff is the channel in the module
//
#define T166_ANALOG_MODULE_1 (1)
#define T166_ANALOG_MODULE_2 (2)

// first analog module
#define T166_US_MOD (T166_ANALOG_MODULE_1)  // Ultrasound module
#define T166_US_DIST (7)                    // Ultrasound distance pin
#define T166_BAT_MOD (T166_ANALOG_MODULE_1) // Module for battery sensor
#define T166_BAT_VOLT (8)                   // Channel for battery voltage
#define T166_PNEU_MOD (T166_ANALOG_MODULE_1)// Pneumatics module
#define TI66_PNEU_PRESS (1)                 // Pneumatics pressure pin

//
// Digital Pin layout
//
// Inclinometer encoder; channel A
#define T166_ENC_INCL_A (1)
// Inclinometer encoder; channel B
#define T166_ENC_INCL_B (2)   	 
// Latch Magnet Sensor
#define T166_KICKER_MAGNET_SENSOR (3)
// Pneumatic pressure
#define T166_PNEUMATIC_PRESSURE_SWITCH (5)	
// Enables autonomous if jumper is in
#define T166_AUTONOMOUS_JUMPER (7)           
// Banner Sensor
#define T166_BANNER_SENSOR_DIGITAL_INPUT (10)	

//
// PWM layout
//
/** channel for the Horizontal Servo */    
#define T166_HORIZONTAL_SERVO_CHANNEL (8)     
/** channel for the Vertical Servo */    
#define T166_VERTICAL_SERVO_CHANNEL (9)

//
// Relay Layout
//
/** Channels for Compressor */
#define T166_COMPRESSOR_RELAY_CHANNEL (1)

//
// Pneumatics Bumper */
//
/** Channel for Unkicker piston */
#define T166_UNKICKER_PISTON (2)
/** Channel for Lift Piston */
#define T166_UNLIFT_PISTON (3)
/** Channel for Kicker piston */
#define T166_KICKER_PISTON (5)
/** Channel for Un-lift latch piston */
#define T166_LIFT_PISTON (6)

//
// Pneumatics */
//
/** Low pressure limit */
#define T166_PNEU_LOW (110.0)
/** High pressure limit */
#define T166_PNEU_HIGH (120.0)
/** Minimum kicking pressure */
#define T166_PNEU_KICK_MIN (50.0)
/** Amount of time before magnet sensor is overridden */
#define T166_LATCH_OVERRIDE (10)      // this is the number of 10 ms loops before the override

//
// CAN bus layout
//
/** Maximum Jag number plus one */
#define T166_NUM_JAGS (11)
/** channel for the Lift Winch Motor */
#define T166_LIFT_MOTOR_CAN (5)
/** channel for the Ball Control  motor */
#define T166_BALLCONTROL_MOTOR_CAN (9)
/** channel for the Left Motor */
#define T166_LEFT_MOTOR_CAN (7)
/** channel for the Right Motor */
#define T166_RIGHT_MOTOR_CAN (10)


//
// USB port layout
//
/** Left driver stick is in USB port #1 */
#define T166_DRIVER_STICK_LEFT (1)
/** Right driver stick is in USB port #2 */
#define T166_DRIVER_STICK_RIGHT (2)
/** Copilot stick is in USB port #3 */
#define T166_COPILOT_STICK (3)

//
// Joystick buttons
//
// Kicker button
#define T166_KICKER_BUTTON (1)
// Lift release
#define T166_LIFT_RELEASE_BUTTON (1)
// Ball control-pulls towards bot
#define T166_BALLCONTROL_PULL (2)
// Autobalancing button
#define T166_AUTOBALANCE_BUTTON (3)
// Ball control-pushes from bot
#define T166_BALLCONTROL_PUSH (3)
// Lift down button
#define T166_LIFT_DOWN_BUTTON (4)
// Lift up button
#define T166_LIFT_UP_BUTTON (5)
// Camera snapshot button
#define T166_CAMERA_BUTTON1 (8)
// Other camera snapshot button
#define T166_CAMERA_BUTTON2 (9)

#endif // !defined(_T166_DEFINES_H)
