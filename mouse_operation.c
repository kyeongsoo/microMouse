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
            }
            else if (infraredFrontLeft && !infraredFrontRight) {
                // left sensor detects; avoid left obstacle

            }
            else if (!infraredFrontLeft && infraredFrontRight) {
                // right sensor detects; avoid right obstacle

            }
            else {
                // both sensors detect; avoid front obstacle
                ControlMouse(MOUSE_ACTION_STOP);
                ControlMouse(MOUSE_ACTION_REVERSE);
                ControlMouse(MOUSE_ACTION_TURNAROUND);	// 180 dgree turn
            }
        }
        else if (touchBarFrontLeft && !touchBarFrontRight) {
            // left bar is touched; avoid left obstacle
            ControlMouse(MOUSE_ACTION_STOP);
            ControlMouse(MOUSE_ACTION_REVERSE);
            ControlMouse(MOUSE_ACTION_TURNRIGHT);
        }
        else if (!touchBarFrontLeft && touchBarFrontRight) {
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


void LineFollowing ()
{
    byte fl, fr, rl, rr;
    byte flMax, frMax, rlMax, rrMax;
    byte flMin, frMin, rlMin, rrMin;
    byte flTH, frTH, rlTH, rrTH;
    byte tmp;

    mouseMode = MOUSE_MODE_OBSTACLE_AVOIDING;
    ControlMouse(MOUSE_ACTION_STOP);

    // first, record values from black surface
    // once you place your mouse on black surface, hit the front left touch bar
    while (touchBarFrontLeft == 0)
    {
    }
    flMax = ADCRead(0x01);
    frMax = ADCRead(0x00);
    rlMax = ADCRead(0x03);
    rrMax = ADCRead(0x02);
    ControlMouse(MOUSE_ACTION_FORWARD); // to indicate it's done
    Delay(500);
    ControlMouse(MOUSE_ACTION_STOP);

    // then you place your mouse on white surface, hit the front left touch bar
    while (touchBarFrontLeft == 0)
    {
    }
    flMin = ADCRead(0x01);
    frMin = ADCRead(0x00);
    rlMin = ADCRead(0x03);
    rrMin = ADCRead(0x02);
    ControlMouse(MOUSE_ACTION_FORWARD); // to indicate it's done
    Delay(500);
    ControlMouse(MOUSE_ACTION_STOP);
    
    // finally, set optimal thresholds for sensors
    flTH = (byte)(flMax/2.0 + flMin/2.0);
    frTH = (byte)(frMax/2.0 + frMin/2.0);
    rlTH = (byte)(rlMax/2.0 + rlMin/2.0);
    rrTH = (byte)(rrMax/2.0 + rrMin/2.0);

    for (;;) {
        // first move forward
        ControlMouse(MOUSE_ACTION_FORWARD);

        // update the status of each sensor
        tmp = ADCRead(0x01);
        fl = tmp < flTH ? 0 : 1;
        tmp = ADCRead(0x00);
        fr = tmp < frTH ? 0 : 1;
        tmp = ADCRead(0x03);
        rl = tmp < rlTH ? 0 : 1;
        tmp = ADCRead(0x02);
        rr = tmp < rrTH ? 0 : 1;

        if (fl == 1 && fr == 1 && rl == 1 && rr == 1) {
            // the mouse is on track (i.e., following the line correctly)
            ControlMouse(MOUSE_ACTION_FORWARD);
            Delay(500);
        }
        /* the rest of logical branches to be completed by each of you
        else if () {
        }
        */
    } // end of for() loop
}


void Combat()
{
    // fill your code here for the combat mode    
}


// debug mode with simple command-line interface
void Debug()
{
    byte command;

    // display a welcome message with a list of commands
    SCISendNewLine();
    SCISendStr("Welcome to the debug mode of EG-252 sample micromouse programme!\r\n");
    SCISendNewLine();
    SCISendNewLine();
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
        command = SCIGetChar();
        
        // TEST
        SCIDisplayBitString(command);
        SCIDisplayBitString((char)'F');
        SCIDisplayBitString((byte)'R');
        SCIDisplayBitString('S');
        if ((int)command == 70)
        {
            SCISendStr("Forward ... \r\n");
        }
        if ((command & 0b01111111) == 0x52)
        {
            SCISendStr("Reverse ... \r\n");
        }
        if ((int)command > 256) 
        {
            SCISendStr("Greater than 256\r\n");
        }
        else if ((int)command > 128) 
        {
            SCISendStr("Greater than 128\r\n");
        }
        else if ((int)command > 64)
        {
            SCISendStr("Greater than 64\r\n");
        }
        else
        {
            SCISendStr("Less than or eqal to 64\r\n");
        }
        // TEST

        switch(command) {
            case 70:  // ascii code for 'F'
                SCISendStr("Forward ... \r\n");
                break;
            case 82:  // ascii code for 'R'
                SCISendStr("Reverse ... \r\n");
                break;
            case 83:  // ascii code for 'S'
                SCISendStr("Stop ... \r\n");
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


// simple test mode for testing functions
void Test()
{
  int opDelay = 10000;
  
  while (1) {
    ControlMouse(MOUSE_ACTION_FORWARD);
    Delay(opDelay);
  /*
    ControlMouse(MOUSE_ACTION_FORWARD);
    Delay(opDelay);
    ControlMouse(MOUSE_ACTION_REVERSE);
    Delay(opDelay);
    ControlMouse(MOUSE_ACTION_TURNLEFT);
    Delay(opDelay);
    ControlMouse(MOUSE_ACTION_TURNRIGHT);
    Delay(opDelay);
    ControlMouse(MOUSE_ACTION_TURNAROUND);
    Delay(opDelay);
    ControlMouse(MOUSE_ACTION_STOP);
    Delay(opDelay);
    */
  }
}


// ADC test mode for line following sensors
void ADCTest()
{
    while (1)
    {
        ADCRead();
    }
}
