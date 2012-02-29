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



#include "mouse.h"	// for the declaration of types, constants, variables and functions


#define MAIN_PROGRAM  // notify the compiler that this is where main() is, especially to avoid duplicate definitions of global variables


void main(void)
{
    byte sw1, sw2;
    
//  EnableInterrupts;
    DisableInterrupts;
    SOPT = 0x00; //disable watchdog

    //--------------------------------------------------------
    // Initialization
    //--------------------------------------------------------
    // for motor driving with PWM from TPM1
    TPM1SC = 0b00001000;    // edge-aligned PWM on bus rate clock
    TPM1MOD = 0x8888;       // set PWM period
    TPM1C2SC = 0b00101000;  // edge-aligned PWM with high-true pulses for PTF0
    TPM1C3SC = 0b00101000;  // edge-aligned PWM with high-true pulses for PTF1
    TPM1C4SC = 0b00101000;  // edge-aligned PWM with high-true pulses for PTF2
    TPM1C5SC = 0b00101000;  // edge-aligned PWM with high-true pulses for PTF3

    // for motor speed control with timer overflow interrupt of TPM2
    TPM2SC = 0b01001000;    // enable timer overflow interrupt on bus rate clock
    TPM2MOD = 0x4e20;       // set timer period to 10ms (= 20,000*0.5us)
/*
    TPM2C0SC = 0b01010000;  // enable interrups on time-out
    TPM2C1SC = 0b01010000;  // enable interrups on time-out
*/

    // for KBI handling
    PTDPE = 0xFF;   // enable port D pullups for push button switch
    PTDDD = 0x00;   // set port D as input; switches 3 and 4 are connected to port D3 and D2, respectively
    KBI1SC = BitClear(1, KBI1SC);   // KBIE=0; disable KBI
    KBI1PE = 0x60;                  // enable KBI function for pins 5 and 6 only
    KBI1SC = BitClear(0, KBI1SC);   // KBIMOD=0; select edge-only detection
    KBI1SC = BitSet(2, KBI1SC);     // KBACK=1; clear KBI flag
    KBI1SC = BitSet(1, KBI1SC);     // KBIE=1; enable KBI

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
    // Closed | Closed | Reserved                     | OFF   | OFF
    // --------------------------------------------------------------
    // *  We assume that PTE0 and PTE1 are connected to LED8 and LED9, respectively.
    // ** These switches are active low and input a logic high when set to the open position.
    //
    PTAPE = 0xFF;   // enable port A pullups for push button switch
    PTADD = 0x00;   // set port A as input; switches 1 and 2 are connected to port A0 and A1, respectively
    
    sw1 = PTAD_PTAD0;  // read switch configuration
    sw2 = PTAD_PTAD1;  // read switch configuration

    PTEDD = 0xFF;   // set port E as output    
    PTED =  0xFF;   // turn off LEDs

    // simple FSM for motor status handling
    if (sw1 == 1) {
        if (sw2 == 1) {
            mouseMode = MOUSE_MODE_OBSTACLE_AVOIDING;
            PTED_PTED0 = 1;
            PTED_PTED1 = 1;
        }
        else {
            mouseMode = MOUSE_MODE_LINE_FOLLOWING;
            PTED_PTED0 = 1;
            PTED_PTED1 = 0;
        }
    }
    else {
        if (sw2 == 1) {
            mouseMode = MOUSE_MODE_COMBAT;
            PTED_PTED0 = 0;
            PTED_PTED1 = 1;
        }
        else {
            // reserved for another mode in the future
            PTED_PTED0 = 0;
            PTED_PTED1 = 0;
        }
    }

    // now we are ready to go!
    EnableInterrupts;
    for(;;) {
        // do nothing; justwaiting for interrupts
    }
}
