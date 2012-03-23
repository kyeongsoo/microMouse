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
  ICGC1   = 0x78;   // select external crystal;
  ICGC2   = 0x30;   // multiply by 10; ICGC1 and ICGC2 specify 20 MHz bus clock
    
  SCI1BD = 0x0082;  // 9600 baud with the bus clock of 20 MHz
  SCI1C2  = 0x0C;   // Turn on transmitter and receiver.

/*
    // SCI1BD = 0x000D; // 9600 baud with the bus clock of 2 MHz
    SCI1BDH = 0x00
    SCI1BDL = 0x1A; // 9600 baud with the bus clock of 4 MHz (of AW60 demo board)
    SCI1C2 = 0b00001100;    // turn on TX/RX with polling
*/
}


// receive a character from SCI port
byte SCIReceiveChar()
{
  byte ch;
  
  while (SCI1S1_RDRF != 1) {
    // wait for data
  }
  ch = SCI1S1;  // clear the RDRF flag
  ch = SCI1D;   // read the character
  return ch;
}


// send a character to SCI port
void SCISendChar(char ch)
{
  while (SCI1S1_TDRE != 1){
    // wait for output buffer empty
  }
  SCI1D = ch;   // send the character
}


// receive a character from and echo it to SCI port
byte SCIGetChar(void)
{
    byte ch;
    ch = SCIReceiveChar();
    SCISendChar(ch);
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


// send newline and return characters
void SCISendNewline()
{
    SCISendStr("\r\n");
}
