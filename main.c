///
/// @file       main.c
/// @author     Kyeong Soo (Joseph) Kim <k.s.kim@swansea.ac.uk>
/// @date       2012-02-21
///
/// @brief      Main part of the micro mouse program
///
/// @remarks    This program is based on Dr Tim Davies' assembly version of
///             micro mouse program and the C port of it by Mr Gareth Evans.
///
/// @copyright  Copyright (C) 2012 Swansea University. All rights reserved.
///
/// @copyright  This software is written and distributed under the GNU General
///             Public License Version 2 (http://www.gnu.org/licenses/gpl-2.0.html).
///             You must not remove this notice, or any other, from this software.
///

///
/// @mainpage   Sample Programs in C for EG-252 Group Design Exercise - Micromouse
///
/// @section    sec_intro Introduction
///             
/// This web page provides information on the sample micromouse programs in C
/// and gives advices on how to install programs and modify them for your own
/// micromouse.
///
/// @section    sec_install Installation
///
/// The source files are maintained at a <a href="https://github.com/kyeongsoo/microMouse">github repository</a>.
/// You can either download the whole <a href="https://github.com/kyeongsoo/microMouse/downloads">source tree</a>
/// or clone the repository on your computer using the URL for a read-only access (git://github.com/kyeongsoo/microMouse.git)
/// if you have a git client (e.g., <a href="http://code.google.com/p/tortoisegit/">tortoisegit</a>).
///
/// @section    sec_config Configurations
///
/// The sample programs have been prepared for a DEMO9S08AW60E demonstration
/// board for ease of testing and debugging with the following configurations:
///
/// @subsection sec_global_constants Global Constans
///
/// The table below summarises global constants and their default values and descriptions,
/// which are defined in the header file ('mouse.h'):
/// <table border="1">
/// <tr>
/// <th>Name</th>
/// <th>Default Value</th>
/// <th>Description</th>
/// </tr>
/// <tr>
/// <td>touchBarFrontLeft</td>
/// <td>PTAD_PTAD2</td>
/// <td>left front touch bar</td>
/// </tr>
/// <tr>
/// <td>touchBarFrontRight</td>
/// <td>PTAD_PTAD1</td>
/// <td>right front touch bar</td>
/// </tr>
/// <tr>
/// <td>infraredFrontLeft</td>
/// <td>PTAD_PTAD7</td>
/// <td>left front infrared sensor</td>
/// </tr>
/// <tr>
/// <td>infraredFrontRight</td>
/// <td>PTAD_PTAD6</td>
/// <td>right front infrared sensor</td>
/// </tr>
/// <tr>
/// <td>busClock</td>
/// <td>2</td>
/// <td>system bus clock in MHz</td>
/// </tr>
/// <tr>
/// <td>pwmPeriod</td>
/// <td>10</td>
/// <td>Period of PWM signal in ms</td>
/// </tr>
/// <tr>
/// <td>controlPeriod</td>
/// <td>50</td>
/// <td>Period of motor speed control in ms</td>
/// </tr>
/// <tr>
/// <td>defaultSpeed</td>
/// <td>25</td>
/// <td>Default speed in terms of percentage duty cycle (e.g., 100% for full speed)</td>
/// </tr>
/// </table>
///
/// @subsection sec_mouse_operation Mouse Operations
///
/// Mouse operation mode can be set with the rocker switches 1/2 on the demo
/// board as follows:
/// <table border="1">
/// <tr>
/// <th>SW1</th>
/// <th>SW2</th>
/// <th>Operation Mode</th>
/// <th>LED8*</th>
/// <th>LED9*</th>
/// </tr>
/// <tr>
/// <td>Open**</td>
/// <td>Open</td>
/// <td>MOUSE_MODE_OBSTACLE_AVOIDING</td>
/// <td>ON</td>
/// <td>ON</td>
/// </tr>
/// <tr>
/// <td>Open</td>
/// <td>Closed</td>
/// <td>MOUSE_MODE_MODE_LINE_FOLLOWING</td>
/// <td>ON</td>
/// <td>OFF</td>
/// </tr>
/// <tr>
/// <td>Closed</td>
/// <td>Open</td>
/// <td>MOUSE_MODE_MODE_COMBAT</td>
/// <td>OFF</td>
/// <td>ON</td>
/// </tr>
/// <tr>
/// <td>Closed</td>
/// <td>Closed</td>
/// <td>Reserved</td>
/// <td>OFF</td>
/// <td>OFF</td>
/// </tr>
/// </table>
/// @li *  We assume that PTC2 and PTC6 are connected to LED8 and LED9, respectively.
/// @li ** These switches are active low and input a logic high when set to the open position.
///
/// @subsection sec_motor_control Motor Control
///


#define MAIN_PROGRAM  // notify the compiler that this is where main() is, especially to avoid duplicate definitions of global variables in "mouse.h"


#include "mouse.h"	// for the declaration of types, constants, variables and functions


void main(void)
{
    byte tbfr, tbfl, tbrr, tbrl;
    
    DisableInterrupts;
    SOPT = 0x00; //disable watchdog
    
    ICGC1 = 0b01110100; // select external crystal
    Delay(64);  // start up delay for crystal
    SCISetup(); // setup serial communication via RS-232 I/F
    
    //--------------------------------------------------------
    // Initialization
    //--------------------------------------------------------
    // for motor driving with PWM from TPM1
    TPM1SC = 0b00001000;    // edge-aligned PWM on bus clock
    TPM1MOD = (word)(pwmPeriod * busClock * 1000);  // set PWM period
    TPM1C2SC = 0b00101000;  // edge-aligned PWM with high-true pulses for PTF0 (left motor IN_A)
    TPM1C3SC = 0b00101000;  // edge-aligned PWM with high-true pulses for PTF1 (left motor IN_B)
    TPM1C4SC = 0b00101000;  // edge-aligned PWM with high-true pulses for PTF2 (right motor IN_A)
    TPM1C5SC = 0b00101000;  // edge-aligned PWM with high-true pulses for PTF3 (right motor IN_B)

    // for motor speed control with timer overflow interrupt of TPM2
    TPM2SC = 0b01001000;    // enable timer overflow and input capture on bus rate clock
    TPM2MOD = (word)(controlPeriod * busClock * 1000);  // set motor speed control period
    TPM2C0SC = 0b01000100;  // enable interrups on positive edge for PTF4 (left tachometer)
    TPM2C1SC = 0b01000100;  // enable interrups on positive edge for PTF5 (right tachometer)
    diffLeft = 0;           // difference between two consecutive counter values for left motor
    diffRight = 0;          // difference between two consecutive counter values for right motor
    travelDistance = 0;     // distance to travel; one unit is approximately 05 mm
    scaleFactor = 200;      // scale factor used in motor speed control
    nomSpeed = 0x2000;      // nominal speed
    pwLeft = defaultSpeed;  // PWM duty cycle for left motor
    pwRight = defaultSpeed; // PWM duty cycle for right motor
    pwMax = 90;             // maximum for PWM duty cycle
    pwMin = 10;             // minimum for PWM duty cycle

    // for ADC
    ADC1CFG = 0b00000000;   // on bus clock, 8-bit conversion
    APCTL1 = 0b11111111;    // use all 8 pins of port B for ADC

    // for motor status
    leftMotor = MOTOR_STATUS_STOP;
    rightMotor = MOTOR_STATUS_STOP;

    // now we are ready to go!
    EnableInterrupts;

    //
    // Now we set the mouse operation mode based on the status of two front
    // touch bars -- at the moment of turning it on -- as follows:
    //
    // ---------------------------------------------------------------------
    // touchBarFrontLeft | touchBarFrontRight | Operation Mode
    // ---------------------------------------------------------------------
    // not touched       | not touched        | MOUSE_MODE_DEBUG (DEFAULT)
    // not touched       | touched            | MOUSE_MODE_COMBAT
    // touched           | not touched        | MOUSE_MODE_LINE_FOLLOWING
    // touched           | touched            | MOUSE_MODE_OBSTACLE_AVOIDING
    // ---------------------------------------------------------------------
    //

    PTAPE = 0xFF;   // enable port A pullups for touchbar switches and infrared sensors
    PTADD = 0x00;   // set port A as input

/*
    tbfl = touchBarFrontLeft;
    tbfr = touchBarFrontRight;
    */
    tbfl = 0;
    tbfr = 0;
    if ((tbfl == 0) && (tbfr == 0)) {
        mouseMode = MOUSE_MODE_DEBUG;
        Test();
    }
    else if ((tbfl == 0) && (tbfr == 1)) {
        mouseMode = MOUSE_MODE_COMBAT;
        Combat();
    }
    else if ((tbrl == 1) && (tbrr == 0)) {
        mouseMode = MOUSE_MODE_LINE_FOLLOWING;
        LineFollowing();
    }
    else {
        mouseMode = MOUSE_MODE_OBSTACLE_AVOIDING;
        AvoidObstacle();
    }

    for (;;) {
        // do nothing; just waiting for interrupts
    }
}