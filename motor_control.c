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
    word pwm, tpm1, tpm2;
    // holding values to be transferred to TPM registers (TPM1C2V/TPM1C3V or TPM1C4V/TPM1C5V)
    
    if (motor == MOTOR_LEFT) {
        pwm = (word)((100-pwLeft)*(TPM1MOD/100));	// duty cycle is for the 'off' period due to H bridge configuration
    }
    else {        
        pwm = (word)((100-pwRight)*(TPM1MOD/100));	// duty cycle is for the 'off' period due to H bridge configuration
    }
    
    switch (action) {
    case MOTOR_ACTION_FORWARD:
        tpm1 = pwm;
        tpm2 = HIGH_WORD;
        status = MOTOR_STATUS_FORWARD;
        break;
    case MOTOR_ACTION_REVERSE:	
        tpm1 = HIGH_WORD;
        tpm2 = pwm;
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
    //int diff;
    word tmpLeft, tmpRight;

    /*
    diff = (int)diffLeft - (int)diffRight;
    
    if (diff != 0) {
        // adjust the speeds of left and right motors based on the difference
        //diff = diff / scaleFactor;
        diff /= 65535;
        tmpLeft = pwLeft - (word)diff;
        tmpRight = pwRight + (word)diff;
    */

    /*    
    if (diffLeft > diffRight) {
        diff = ((int)(diffLeft - diffRight));
        tmpLeft -= 2;
        tmpRight += 2;
    }
    else {
        diff = ((int)(diffRight - diffLeft));
        tmpLeft = pwLeft + (word)diff;
        tmpRight = pwRight - (word)diff;        
    }
    */
    if (diffLeft > diffRight) {
        tmpLeft = pwLeft - 2;
        tmpRight = pwRight + 2;
    }
    if (diffLeft < diffRight) {
        tmpLeft = pwLeft + 2;
        tmpRight = pwRight - 2;      
    }
     
    /*     
	// if the new value is greater than pwMax or smaller than pwMin, recover the original value from TPM1 channel value register;
	// otherwise, update the rigster with the new value.
	if (tmpLeft >= pwMin && tmpLeft <= pwMax) {
		pwLeft = tmpLeft;
	}
	if (tmpRight >= pwMin && tmpRight <= pwMax) {
		pwRight = tmpRight;
	}
	*/
	if (tmpLeft >= pwMax) {
		pwLeft = pwMax;
	}
	else if (tmpLeft <= pwMin) 
	{
	    pwLeft = pwMin;
	}
	else 
	{
	    pwLeft = tmpLeft;
	}
	if (tmpRight >= pwMax) {
		pwRight = pwMax;
	}
	else if (tmpRight <= pwMin) 
	{
	    pwRight = pwMin;
	}
	else 
	{
	    pwRight = tmpRight;
	}

	// finally call ControlMotor() to reflect the changed values of pwLeft and pwRight in PWM.
	if (leftMotor == MOTOR_STATUS_FORWARD)
	{
	    ControlMotor(MOTOR_LEFT, MOTOR_ACTION_FORWARD);
	}
	else if (leftMotor == MOTOR_STATUS_REVERSE)
	{
	    ControlMotor(MOTOR_LEFT, MOTOR_ACTION_REVERSE);
	}
	if (rightMotor == MOTOR_STATUS_FORWARD)
	{
	    ControlMotor(MOTOR_RIGHT, MOTOR_ACTION_FORWARD);
	}
	else if (rightMotor == MOTOR_STATUS_REVERSE)
	{
	    ControlMotor(MOTOR_RIGHT, MOTOR_ACTION_REVERSE);
    }
}
