///
/// @file       serial_interface.c
/// @author     Kyeong Soo (Joseph) Kim <k.s.kim@swansea.ac.uk>
/// @date       2012-03-09
///
/// @brief      Serial interface routines
///
/// @copyright  Copyright (C) 2012 Swansea University. All rights reserved.
///
/// @copyright  This software is written and distributed under the GNU General
///             Public License Version 2 (http://www.gnu.org/licenses/gpl-2.0.html).
///             You must not remove this notice, or any other, from this software.
///


#include "mouse.h"	// for the declaration of types, constants, variables and functions


// setup SCI module
void SCISetup()
{
/*
    ICGC1   = 0b01110100;   // select external crystal;
    ICGC2   = 0x30;   // multiply by 10; ICGC1 and ICGC2 specify 20 MHz bus clock
*/    
    SCI2BD = 0x000D;    // 9600 baud with the bus clock of 2 MHz
    SCI2C2 = 0b00001100;    // Turn on TX (TE=1) and RX (RE=1) with polling

/*
// SCI2BD = 0x000D; // 9600 baud with the bus clock of 2 MHz
SCI2BDH = 0x00
SCI2BDL = 0x1A; // 9600 baud with the bus clock of 4 MHz (of AW60 demo board)
SCI2C2 = 0b00001100;    // turn on TX/RX with polling
*/
}


// receive a character from SCI port
byte SCIReceiveChar()
{
    byte ch;
  
    while (SCI2S1_RDRF != 1) {
        // wait for data
    }
    ch = SCI2S1;  // clear the RDRF flag
    ch = SCI2D;   // read the character
    return ch;
}


// send a character to SCI port
void SCISendChar(char ch)
{
    while (SCI2S1_TDRE != 1){
        // wait for output buffer empty
    }
    SCI2D = ch;   // send the character
}


// receive a character from and echo it to SCI port
byte SCIGetChar(void)
{
    byte ch;
    ch = SCIReceiveChar();
    SCISendChar(ch);
    SCISendNewLine();
    return ch;
}


// send a string to SCI port
void SCISendStr(char *str)
{
    while (*str != '\0') {
        SCISendChar(*str);
        str++;
    }
}


// display a promp message
void SCIDisplayPrompt()
{
    SCISendStr("\r\n");
    SCISendStr("> ");
}


// display bit representation of a byte/character
void SCIDisplayBitString(char ch)
{
    int i;
    char bit;
    
    for (i = 0; i < 8; i++) {
        bit = (ch & (0x80 >> i)) ? '1' : '0';
        SCISendChar(bit);
    }
    SCISendNewLine();
}


// send newline and return characters
void SCISendNewLine()
{
    SCISendStr("\r\n");
}
