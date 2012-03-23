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

#include "mouse.h"	// for the declaration of types, constants, variables and functions


#define MAIN_PROGRAM  // notify the compiler that this is where main() is, especially to avoid duplicate definitions of global variables


void main(void)
{
    byte sw1, sw2;
    
    DisableInterrupts;
    SOPT = 0x00; //disable watchdog
    
/*
    ICGC1 = 0b01110100; // select external crystal
    Delay(64);  // start up delay for crystal
*/    
    
    SCISetup(); // setup serial communication via RS-232 I/F
    
    //--------------------------------------------------------
    // Initialization
    //--------------------------------------------------------
    // for motor driving with PWM from TPM1
    TPM1SC = 0b00001000;    // edge-aligned PWM on bus clock
    TPM1MOD = (word)(pwmPeriod * busClock * 1000);  // set PWM period
    TPM1C2SC = 0b00101000;  // edge-aligned PWM with high-true pulses for PTF0
    TPM1C3SC = 0b00101000;  // edge-aligned PWM with high-true pulses for PTF1
    TPM1C4SC = 0b00101000;  // edge-aligned PWM with high-true pulses for PTF2
    TPM1C5SC = 0b00101000;  // edge-aligned PWM with high-true pulses for PTF3

    // for motor speed control with timer overflow interrupt of TPM2
    TPM2SC = 0b01001000;    // enable timer overflow and input capture on bus rate clock
    TPM2MOD = (word)(controlPeriod * busClock * 1000);  // set motor speed control period
    TPM2C0SC = 0b01000100;  // enable interrups on positive edge
    TPM2C1SC = 0b01000100;  // enable interrups on positive edge
    diffLeft = 0;           // difference between two consecutive counter values for left motor
    diffRight = 0;          // difference between two consecutive counter values for right motor
    travelDistance = 0;     // distance to travel; one unit is approximately 05 mm
    scaleFactor = 2;        // scale factor used in motor speed control    

    // for KBI handling
    PTDPE = 0xFF;   // enable port D pullups for push button switch
    PTDDD = 0x00;   // set port D as input; switches 3 and 4 are connected to port D3 and D2, respectively
    KBI1SC = BitClear(1, KBI1SC);   // KBIE=0; disable KBI
    KBI1PE = 0x60;                  // enable KBI function for pins 5 and 6 only
    KBI1SC = BitClear(0, KBI1SC);   // KBIMOD=0; select edge-only detection
    KBI1SC = BitSet(2, KBI1SC);     // KBACK=1; clear KBI flag
    KBI1SC = BitSet(1, KBI1SC);     // KBIE=1; enable KBI

    // for ADC
    ADC1CFG = 0b00000000;   // on bus clock, 8-bit conversion
    APCTL1 = 0b11111111;    // use all 8 pins of port B for ADC

    // for motor status
    leftMotor = MOTOR_STATUS_STOP;
    rightMotor = MOTOR_STATUS_STOP;

    //
    // Now we are going to set the mouse operation mode according to rocker switches 1 & 2 as follows:
    // --------------------------------------------------------------
    // SW1    | SW2    | Operation Mode               | LED8* | LED9*
    // --------------------------------------------------------------
    // Open** | Open   | MOUSE_MODE_OBSTACLE_AVOIDING | ON    | ON
    // Open   | Closed | MOUSE_MODE_LINE_FOLLOWING    | ON    | OFF
    // Closed | Open   | MOUSE_MODE_COMBAT            | OFF   | ON
    // Closed | Closed | MOUSE_MODE_DEBUG             | OFF   | OFF
    // --------------------------------------------------------------
    // *  We assume that PTC2 and PTC6 are connected to LED8 and LED9, respectively.
    // ** These switches are active low and input a logic high when set to the open position.
    //
    PTAPE = 0xFF;   // enable port A pullups for push button switch
    PTADD = 0x00;   // set port A as input; switches 1 and 2 are connected to port A0 and A1, respectively
    
    sw1 = PTAD_PTAD0;  // read switch configuration
    sw2 = PTAD_PTAD1;  // read switch configuration

    PTCDD = 0xFF;   // set port C as output    
    PTCD =  0xFF;   // turn off LEDs

    // simple FSM for motor status handling
    if (sw1 == 1) {
        if (sw2 == 1) {
            mouseMode = MOUSE_MODE_OBSTACLE_AVOIDING;
            PTCD_PTCD2 = 1;
            PTCD_PTCD6 = 1;
            AvoidObstacle();
        }
        else {
            mouseMode = MOUSE_MODE_LINE_FOLLOWING;
            PTCD_PTCD2 = 1;
            PTCD_PTCD6 = 0;
        }
    }
    else {
        if (sw2 == 1) {
            mouseMode = MOUSE_MODE_COMBAT;
            PTCD_PTCD2 = 0;
            PTCD_PTCD6 = 1;
        }
        else {
            mouseMode = MOUSE_MODE_DEBUG;
            // reserved for another mode in the future
            PTCD_PTCD2 = 0;
            PTCD_PTCD6 = 0;
            Debug();
        }
    }

    // now we are ready to go!
    EnableInterrupts;
    for(;;) {
        // do nothing; justwaiting for interrupts
    }
}
