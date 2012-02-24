///
/// @file       main.c
/// @author     Kyeong Soo (Joseph) Kim <k.s.kim@swansea.ac.uk>
/// @date       2012-02-21
///
/// @brief      Main part of the micromouse program
///
/// @remarks    This program is based on Dr Tim Davies' assembly version of
///             micromouse program and the C port of it by Mr Gareth Evans.
///
/// @remarks    Copyright (C) 2012 Swansea University. All rights reserved.
///
/// @remarks    This software is written and distributed under the GNU General
///             Public License Version 2 (http://www.gnu.org/licenses/gpl-2.0.html).
///             You must not remove this notice, or any other, from this software.
///



#include "mouse.h"	// for the declaration of types, constants, variables and functions


#define MAIN_PROGRAM  // notify the compiler that this is where main() is, especially to avoid duplicate definitions of global variables


void main(void)
{
    EnableInterrupts;
    SOPT = 0x00; //disable watchdog

    //--------------------------------------------------------
    // Initialization
    //--------------------------------------------------------
    // for motor driving with PWM from TPM1
    TPM1SC = 0b00001000;  // edge-aligned PWM on bus rate clock
    TPM1MOD = 0x8888;     // set PWM period
    TPM1C2SC = 0x28;      // TMP1C2SC=00101000; edge-aligned PWM with high-true pulses for PTF0
    TPM1C3SC = 0x28;      // TMP1C3SC=00101000; edge-aligned PWM with high-true pulses for PTF1
    TPM1C4SC = 0x28;      // TMP1C4SC=00101000; edge-aligned PWM with high-true pulses for PTF2
    TPM1C5SC = 0x28;      // TMP1C5SC=00101000; edge-aligned PWM with high-true pulses for PTF3
    
    // for motor speed control with timer overflow interrupt of TPM2
    TPM2SC = 0b01001000   // enable timer overflow interrupt on bus rate clock
    TPM2MOD = 20000;      // set timer period to 10ms (= 20,000*0.5us)
    TPM2C0SC = 0b01010000;  // enable interrups on time-out
    TPM2C1SC = 0b01010000;  // enable interrups on time-out
    

    // for KBI handling
    PTDPE = 0xFF;   // enable port D pullups for push button switch
    PTDDD = 0x00;	// set port D as input; switch 3 and 4 are connected to port D3 and D2, respectively
    KBI1SC = BitClear(1, KBI1SC);   // KBIE=0; disable KBI
    KBI1PE = 0x60;                  // enable KBI function for pins 5 and 6 only
    KBI1SC = BitClear(0, KBI1SC);   // KBIMOD=0; select edge-only detection
    KBI1SC = BitSet(2, KBI1SC);     // KBACK=1; clear KBI flag
    KBI1SC = BitSet(1, KBI1SC);     // KBIE=1; enable KBI

    // for motor status
    leftMotor = MOTOR_STATUS_STOP;
    rightMotor = MOTOR_STATUS_STOP;

    // Now mouse is ready!
    mouseMode = MOUSE_MODE_READY;

    for(;;) {
        // do nothing; justwaiting for interrupts
    }
}
