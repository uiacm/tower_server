/*
 * Header file for the routines used by the towerlights project.  
 *
 * For testing purposes, connect Port B to the Data port and Port D to 
 * one of the Select ports on the towerlights board.
 *
 * This test program sends data to all 120 LEDs (15 bytes). The first byte
 * of data is for RED, the next is for GREEN, the next is BLUE.  
 *
 * To set up the board for this test, place a jumper in any position evenly
 * divisible by 3 (0, 3, 6, 9) in the RED column, place a jumper in position
 * (1, 4, 7, 10, etc) in the GREEN column, and place a jumper in position
 * (2, 5, 8, 11, etc) in the BLUE column.
 *
 */

#include <inttypes.h>
// #include <avr/pgmspace.h>
// #include <stdio.h>

typedef struct RGB RGB;
struct RGB
  {
   uint8_t red;
   uint8_t green;
   uint8_t blue;
  };

typedef union BUFF BUFF;
union BUFF
  {
   struct RGB rgb[40];
   uint8_t linear[120];
  };

 
// Global values (they are global for efficiency purposes.) These are defined
// in twrlights.c

extern union BUFF inarray;   // input array - this should get filled by uart 
                             // routines in AVR code

extern union BUFF bitarray;  // array of bits to send to towerlights board, 
                             // one bit at a time

extern uint8_t outarray[15][7]; // array holding actual bits to be sent to the
                             // towerlights boards.  

extern uint8_t bits[8];      // bits to send each PWM cycle for 1 of 8 
                             // brightness levels. 


/* Busy wait delay function
 */

void delay (unsigned int dly);

/* The following function converts an array of values 0-255 into an array
 * holding bits to send out to the towerlights board.
 */

void tobits();


/* This function determines the actual bytes that need to be sent to the
 * towerlights boards. It accepts the array of bits to be sent out and the
 * clock cycle number of the PWM (a value 0-6), and assembles the bits to be 
 * sent into 8 bit bytes that need to be sent to the towerlights boards. The
 * bytes are arranged as 8 bits of RED, then 8 bits of GREEN, and finally
 * 8 bits of BLUE
 *
 */

void makeoutdata (uint8_t clknum);


/* This routine determines all of the LED control bits that will need to
 * be output whenever a new frame is received. It should be called whenever
 * a new frame comes in from the uart or from the test function "outroom."
 */

void newframe();


/* The following routine does the output of the data, either simulated (as in
 * the PC version), or actual (as in the avr version).
 */

void outdata(uint8_t clknum);


/* This routine outputs one complete PWM cycle. It goes through 7 times to 
 * line, starting with line 0 and going through 15. 
 * complete one full PWM cycle of data. 
 */

void docycle();


/* This routine sets the bits necessary to turn on/off the LEDs in the
 * room that is specified.
 */

void outroom( uint8_t rmno, uint8_t red, uint8_t green, uint8_t blue);

