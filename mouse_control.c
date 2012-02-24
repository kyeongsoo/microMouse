///
/// @file       mouse_control.c
/// @author     Kyeong Soo (Joseph) Kim <k.s.kim@swansea.ac.uk>
/// @date       2012-02-21
///
/// @brief      Implements functions for mouse control.
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


void ControlMouse(MouseAction action)
{
    switch (action) {
    case MOUSE_ACTION_FORWARD:
        if (leftMotor != MOTOR_STATUS_FORWARD) {
            ControlMotor(leftMotor, MOTOR_ACTION_FORWARD);
        }
        if (rightMotor != MOTOR_STATUS_FORWARD) {
            ControlMotor(rightMotor, MOTOR_ACTION_FORWARD);
        }
        if (mouseStatus != MOUSE_STATUS_FORWARD) {
            mouseStatus = MOUSE_STATUS_FORWARD;
        }
        break;
    case MOUSE_ACTION_REVERSE:
        if (leftMotor != MOTOR_STATUS_REVERSE) {
            ControlMotor(leftMotor, MOTOR_ACTION_REVERSE);
        }
        if (rightMotor != MOTOR_STATUS_REVERSE) {
            ControlMotor(rightMotor, MOTOR_ACTION_REVERSE);
        }
        if (mouseStatus != MOUSE_STATUS_REVERSE) {
            mouseStatus = MOUSE_STATUS_REVERSE;
        }
        break;
    case MOUSE_ACTION_STOP:
        if (leftMotor != MOTOR_STATUS_STOP) {
            ControlMotor(leftMotor, MOTOR_ACTION_STOP);
        }
        if (rightMotor != MOTOR_STATUS_STOP) {
            ControlMotor(rightMotor, MOTOR_ACTION_STOP);
        }
        if (mouseStatus != MOUSE_STATUS_STOP) {
            mouseStatus = MOUSE_STATUS_STOP;
        }
        break;
    case MOUSE_ACTION_TURNLEFT:
        if (leftMotor != MOTOR_STATUS_STOP) {
            ControlMotor(leftMotor, MOTOR_ACTION_STOP);
        }
        if (rightMotor != MOTOR_STATUS_FORWARD) {
            ControlMotor(rightMotor, MOTOR_ACTION_FORWARD);
        }
        if (mouseStatus != MOUSE_STATUS_TURNLEFT) {
            mouseStatus = MOUSE_STATUS_TURNLEFT;
        }
        break;        

    case MOUSE_ACTION_TURNRIGHT:
        if (leftMotor != MOTOR_STATUS_FORWARD) {
            ControlMotor(leftMotor, MOTOR_ACTION_FORWARD);
        }
        if (rightMotor != MOTOR_STATUS_STOP) {
            ControlMotor(rightMotor, MOTOR_ACTION_STOP);
        }
        if (mouseStatus != MOUSE_STATUS_TURNRIGHT) {
            mouseStatus = MOUSE_STATUS_TURNRIGHT;
        }
        break;        
    case MOUSE_ACTION_TURNAROUND:
        if (leftMotor != MOTOR_STATUS_FORWARD) {
            ControlMotor(leftMotor, MOTOR_ACTION_FORWARD);
        }
        if (rightMotor != MOTOR_STATUS_REVERSE) {
            ControlMotor(rightMotor, MOTOR_ACTION_REVERSE);
        }
        if (mouseStatus != MOUSE_STATUS_TURNAROUND) {
            mouseStatus = MOUSE_STATUS_TURNAROUND;
        }
        break;        

    } // end of switch()
}
