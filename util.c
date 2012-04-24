///
/// @file       util.c
/// @author     Kyeong Soo (Joseph) Kim <k.s.kim@swansea.ac.uk>
/// @date       2012-02-21
///
/// @brief      Implements utility and miscellaneous functions.
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


//------------------------------------------------------------------------------
// Functions for bit handling
//------------------------------------------------------------------------------
// function to set a specific bit of an unsigned 8-bit integer
byte BitSet(byte Bit_position, byte Var_old)
{
    byte Var_new;
    Var_new = Var_old | (byte)(1<<Bit_position);
    return Var_new;
}


// function to clear a specific bit of an unsigned 8-bit integer
byte BitClear(byte Bit_position, byte Var_old)
{
    byte Var_new;
    Var_new = Var_old & (~(byte)(1<<Bit_position));
    return Var_new;
}


//--------------------------------------------------------
// Misc. functions
//--------------------------------------------------------
void Delay(int a)
{
    int b=0,c=0;
    for (b=0;b<a;b++){
        for(c=0;c<100;c++);
    }
}