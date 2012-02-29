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


void AvoidObstacle() {
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
