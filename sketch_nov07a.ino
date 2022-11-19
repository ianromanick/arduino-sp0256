/* 
 * Copyright © 2022 Ian Romanick
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/**
 * \file sketch_nov07a.ino
 * Simple test program for SpeechChips.com SP0256 chip tester.
 *
 * Has modes for testing SP0256-AL2 and SP0256-017 (with and without
 * SPR-116 speech ROM).
 */

/* Uncomment the defines for the chips installed in your system. Without an
 * extra breakout board of some sort, the system won't have a SPR_116.
 */
//#define SP0256_AL2
//#define SP0256_017
//#define SPR_116

#if (defined(SP0256_AL2) + defined(SP0256_017)) != 1
#error "Pick exactly one of SP0256_AL2 or SP0256_0117."
#endif

// Without the SPR-016 ROM in the circuit, the SP0-256-117 can only say "Oh"
// through "Ten".  Saying "eleven" almost works, but hangs at the end.  None
// of the other words do anything.
#if defined(SP0256_017) && defined(SPR_116)
#define MAX_ADDR 35
#else
#define MAX_ADDR 10
#endif

/* Pinout:
 * 
 * Right:
 *  0 - NC
 *  1 - NC
 *  2 - A3
 *  3 - A2
 *  4 - A1
 *  5 - SE
 *  6 - ALD'
 *  7 - A4
 * 
 *  8 - A5
 *  9 - A6
 * 10 - SBYRESET' 
 * 11 - SBY
 * 12 - RESET'
 * 13 - TEST
 * 
 * Left:
 * A0 - LRQ
 * A1 - A7
 * A2 - A8
 */
char data[64];

#define _A1     4
#define _A2     3
#define _A3     2
#define _A4     7
#define _A5     8
#define _A6     9
#define _A7     A1
#define _A8     A2

#define _SE    5
#define ALD    6
#define SBYRST 10
#define SBY    11
#define RST    12
#define TST    13

void setup()
{
  // LOAD data
#ifdef SP0256_AL2
  int i = 0;
#if 1 // For "Hello world."
  data[i++] = 27; //HH1
  data[i++] = 7;  //EH
  data[i++] = 45; //LL
  data[i++] = 15; //AX
  data[i++] = 53; //OW
  data[i++] = 3;  //PA4
  data[i++] = 46; //WW
  data[i++] = 51; //ER1
  data[i++] = 45; //LL
  data[i++] = 1;  //PA2
  data[i++] = 21; //DD1
  data[i++] = 4;  //PA5
#else // For "Weird stuff."
  data[i++] = 0x30; // WH
  data[i++] = 0x13; // IY
  data[i++] = 0x3c; // ER
  data[i++] = 0x01; // PA2
  data[i++] = 0x15; // DD1

  data[i++] = 0x37; // SS
  data[i++] = 0x37; // SS  
  data[i++] = 0x02; // PA3
  data[i++] = 0x11; // TT1
  data[i++] = 0x1a; // UH
  data[i++] = 0x1a; // UH 
  data[i++] = 0x28; // FF
  data[i++] = 0x28; // FF
    
  data[i++] = 0x04; // PA5
#endif

  data[i++] = -1; //end
#else // For SP0256-017
  for (int i = 0; i <= MAX_ADDR; i++)
    data[i] = i;

  data[MAX_ADDR + 1] = -1;
#endif

  for (int p = 2; p < 11; p++)
    pinMode(p, OUTPUT);

  pinMode(SBYRST, OUTPUT);
  pinMode(ALD, OUTPUT);
  pinMode(RST, OUTPUT);
  pinMode(TST, OUTPUT);
  pinMode(_A7, OUTPUT);
  pinMode(_A8, OUTPUT);
  
  pinMode(SBY, INPUT);

  digitalWrite(TST, LOW);
  digitalWrite(ALD, HIGH);
  digitalWrite(_SE, HIGH);

  digitalWrite(RST, LOW);
  digitalWrite(SBYRST, LOW);
  delayMicroseconds(150);
  digitalWrite(SBYRST, HIGH);
  digitalWrite(RST, HIGH);
}

void loop() 
{
  for (int i = 0; data[i] != -1; i++) {
    // Set address
    digitalWrite(_A1, (data[i] >> 0) & 1 ? HIGH : LOW);
    digitalWrite(_A2, (data[i] >> 1) & 1 ? HIGH : LOW);
    digitalWrite(_A3, (data[i] >> 2) & 1 ? HIGH : LOW);
    digitalWrite(_A4, (data[i] >> 3) & 1 ? HIGH : LOW);
    digitalWrite(_A5, (data[i] >> 4) & 1 ? HIGH : LOW);
    digitalWrite(_A6, (data[i] >> 5) & 1 ? HIGH : LOW);
    digitalWrite(_A7, (data[i] >> 6) & 1 ? HIGH : LOW);
    digitalWrite(_A8, (data[i] >> 7) & 1 ? HIGH : LOW);

    // Set !ADL=0 for 2usec to tell the chip to read
    digitalWrite(ALD, LOW);
    delayMicroseconds(2);
    digitalWrite(ALD, HIGH);

    // Wait for SBY=1 (standby) to indicate chip is done speaking
    while (digitalRead(SBY) == 0);
  }

  delay(2000); // delay 2 sec between phrases
}
