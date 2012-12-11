///
/// @file       isr.c
/// @author     Kyeong Soo (Joseph) Kim <k.s.kim@swansea.ac.uk>
/// @date       2012-02-21
///
/// @brief      Implements interrupt service routines (ISRs).
///
/// @copyright  Copyright (C) 2012 Swansea University. All rights reserved.
///
/// @copyright  This software is written and distributed under the GNU General
///             Public License Version 2 (http://www.gnu.org/licenses/gpl-2.0.html).
///             You must not remove this notice, or any other, from this software.
///


#include "mouse.h"	// for the declaration of types, constants, variables and functions


interrupt VectorNumber_Vkeyboard1 void intSW3_4()
{
    byte sw3, sw4;
    
  sw3 = PTDD_PTDD3;
  sw4 = PTDD_PTDD2;

  // simple FSM for motor status handling
  if (sw3 == 0) {
    // for left motor
    switch (leftMotor) {
      case MOTOR_STATUS_STOP:
        ControlMotor(MOTOR_LEFT, MOTOR_ACTION_FORWARD);
        break;
      case MOTOR_STATUS_FORWARD:
        ControlMotor(MOTOR_LEFT, MOTOR_ACTION_REVERSE);
        break;
      case MOTOR_STATUS_REVERSE:
        ControlMotor(MOTOR_LEFT, MOTOR_ACTION_STOP);
        break;
    }
  } else if (sw4 == 0) {
    // for right motor
    switch (rightMotor) {
      case MOTOR_STATUS_STOP:
        ControlMotor(MOTOR_RIGHT, MOTOR_ACTION_FORWARD);
        break;
      case MOTOR_STATUS_FORWARD:
        ControlMotor(MOTOR_RIGHT, MOTOR_ACTION_REVERSE);
        break;
      case MOTOR_STATUS_REVERSE:
        ControlMotor(MOTOR_RIGHT, MOTOR_ACTION_STOP);
        break;
    }
  }
  
  // 'braking' hasn't been implemented yet
   
  KBI1SC_KBACK = 1; // clear KBI interrupt flag
}


// ISR to provide distance feedback to motor control functions
// based on tacho meter
interrupt VectorNumber_Vtpm2ovf void intTPM2OVF()
{
    byte tmp;

    // clear TPM2 timer overflow flag    
    tmp = TPM2SC_TOF;   // first, need to read from TPM2 timer overflow flag
    TPM2SC_TOF = 0;     // then, clear TPM2 timer overflow flag

/*  
    // DEBUG
    PTED_PTED0 = PTED_PTED0 ^ 1;
    PTED_PTED1 = PTED_PTED1 ^ 1;
*/
    
    if ((leftMotor != MOTOR_STATUS_STOP) && (rightMotor != MOTOR_STATUS_STOP)) {
        // balance the speeds of motors when both motors are moving
        ControlSpeed();
    }
}


// ISR to monitor a left motor speed based on tacho meter
interrupt VectorNumber_Vtpm2ch0 void intTPM2CH0()
{
    byte tmp;
    static word oldLeft = 0;

    // clear TPM2 channel 0 flag    
    tmp = TPM2C0SC_CH0F;    // first, need to read from TPM2 channel 0 flag bit
    TPM2C0SC_CH0F = 0;      // then, clear TPM2 channel 0 flag
    
    diffLeft = TPM2C0V - oldLeft;
    oldLeft = TPM2C0V;
    
    if (travelDistance > 0) {
        // check travelDistance variable and decrement if it is greater than zero
        travelDistance--;
    }
}


// ISR to monitor a right motor speed based on tacho meter
interrupt VectorNumber_Vtpm2ch1 void intTPM2CH1()
{
    byte tmp;
    static word oldRight = 0;

    // clear TPM2 channel 1 flag    
    tmp = TPM2C1SC_CH1F;    // first, need to read from TPM2 channel 1 flag bit
    TPM2C1SC_CH1F = 0;      // then, clear TPM2 channel 1 flag
    
    diffRight = TPM2C1V - oldRight;
    oldRight = TPM2C1V;
    
    if (travelDistance > 0) {
        // check travelDistance variable and decrement if it is greater than zero
        travelDistance--;
    }
  
}