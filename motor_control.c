///
/// @file       motor_control.c
/// @author     Kyeong Soo (Joseph) Kim <k.s.kim@swansea.ac.uk>
/// @date       2012-02-21
///
/// @brief      Implements functions for motor control.
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


void ControlMotor(Motor motor, MotorAction action)
{
    MotorStatus status;
    word tpm1, tpm2;
    // holding values to be transferred to TPM registers (TPM1C2V/TPM1C3V or TPM1C4V/TPM1C5V)

    switch (action) {
    case MOTOR_ACTION_FORWARD:
        tpm1 = (word)(pwLeft*TPM1MOD/100);
        tpm2 = HIGH_WORD;
        status = MOTOR_STATUS_FORWARD;
        break;
    case MOTOR_ACTION_REVERSE:
        tpm1 = HIGH_WORD;
        tpm2 = (word)(pwRight*TPM1MOD/100);
        status = MOTOR_STATUS_REVERSE;
        break;
    case MOTOR_ACTION_STOP:
        tpm1 = HIGH_WORD;
        tpm2 = HIGH_WORD;
        status = MOTOR_STATUS_STOP;
        break;
    }

    if (motor == MOTOR_LEFT) {
        TPM1C2V=tpm1;
        TPM1C3V=tpm2;
        leftMotor = status;
    } else {
        TPM1C4V=tpm1;
        TPM1C5V=tpm2;
        rightMotor= status;
    }
}


// main speed control function called by TPM2 timer overflow ISR
void ControlSpeed(void)
{
    int corr, diff;
    word tmpLeft, tmpRight;
    
    diff = diffLeft - diffRight;
    
    if (diff != 0) {
        // adjust the speeds of left and right motors based on the difference
        diff = diff / scaleFactor;
        tmpLeft = pwLeft - diff;
        tmpRight = pwRight + diff;
        
        // if the new value is greater than pwMax or smaller than pwMin, recover the original value from TPM1 channel value register;
        // otherwise, update the rigster with the new value.
        if (tmpLeft >= pwMin || tmpLeft <= pwMax) {
            pwLeft = tmpLeft;
        }
        if (tmpRight >= pwMin || tmpRight <= pwMax) {
            pwRight = tmpLeft;
        }
    }
}
