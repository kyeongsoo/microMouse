///
/// @file       mouse_operation.c
/// @author     Kyeong Soo (Joseph) Kim <k.s.kim@swansea.ac.uk>
/// @date       2012-02-21
///
/// @brief      Implements functions for mouse operation modes.
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


void AvoidObstacle()
{
    mouseMode = MOUSE_MODE_OBSTACLE_AVOIDING;

    for (;;) {
        // first move forward
        ControlMouse(MOUSE_ACTION_FORWARD);

        // first, check the status of touch bars
        if (!touchBarFrontLeft && !touchBarFrontRight) {
            // neither is touched (i.e., both the values are zero)

            // then check the status of IF sensors
            if (!infraredFrontLeft && !infraredFrontRight) {
                // neither is touched (i.e., both the values are zero)
                // then, back to the loop
                break;
            }
            else if (infraredFrontLeft) {
                // left sensor detects; avoid left obstacle

            }
            else if (infraredFrontRight) {
                // right sensor detects; avoid right obstacle

            }
            else {
                // both sensors detect; avoid front obstacle
                ControlMouse(MOUSE_ACTION_STOP);
                ControlMouse(MOUSE_ACTION_REVERSE);
                ControlMouse(MOUSE_ACTION_TURNAROUND);	// 180 dgree turn
            }
        }
        else if (touchBarFrontLeft) {
            // left bar is touched; avoid left obstacle
            ControlMouse(MOUSE_ACTION_STOP);
            ControlMouse(MOUSE_ACTION_REVERSE);
            ControlMouse(MOUSE_ACTION_TURNRIGHT);
        }
        else if (touchBarFrontRight) {
            // right bar is touched; avoid right obstacle
            ControlMouse(MOUSE_ACTION_STOP);
            ControlMouse(MOUSE_ACTION_REVERSE);
            ControlMouse(MOUSE_ACTION_TURNLEFT);
        }
        else {
            // both bars are touched; avoid front obstacle
            ControlMouse(MOUSE_ACTION_STOP);
            ControlMouse(MOUSE_ACTION_REVERSE);
            ControlMouse(MOUSE_ACTION_TURNAROUND);	// 180 dgree turn
        }
    } // end of for() loop
}


// debug mode with simple command-line interface
void Debug()
{
    char command;

    // display a welcome message with a list of commands
    SCISendNewline();
    SCISendStr("Welcome to the debug mode of EG-252 sample micromouse programme!\r\n");
    SCISendNewline();
    SCISendNewline();
    SCISendStr("List of available commands:\r\n");
    SCISendStr("F\tForward\r\n");
    SCISendStr("R\tReverse\r\n");
    SCISendStr("S\tStop\r\n");
    SCISendStr("A\troate Anticlockwise\r\n");
    SCISendStr("C\trotate Clockwise\r\n");
    SCISendStr("V\tVeer left Clockwise\r\n");
    SCISendStr("B\tVeer right Clockwise\r\n");
    SCISendStr("+\tIncrement speed by 256 units\r\n");
    SCISendStr("-\tDecrement speed by 256 units\r\n");
    SCISendStr("D\tDisplay ADC value 7 through 0\r\n");
    SCISendStr("P\tDisplay PTA as binary number\r\n");

    while (1) {
        // display prompt and wait for a user input
        SCIDisplayPrompt();
        command = (char) SCIGetChar();
        // TEST
        if ((command & 0b01111111) == 'F') 
        {
                SCISendStr("Forward ... \r\n");
        }
        if ((command & 0b01111111) == 0x52)
        {
                SCISendStr("Forward ... \r\n");
        }
        // TEST
        switch(command) {
            case 'F':
                SCISendStr("Forward ... \r\n");
                break;
            case 'R':
                SCISendStr("Reverse ... \r\n");
                break;
            case 'S':
                break;
            case 'A':
                break;
            case 'C':
                break;        
            case '+':
                break;
            case '-':
                break;
            case 'D':
                break;
            case 'P':
                break;
            case 'V':
                break;
            case 'B':
                break;
            case '?':
                break;
            default:
                break;
        }
    }   // end of while ()
}