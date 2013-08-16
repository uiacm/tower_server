/*
 * Test file for the towerlights project. The purpose of this file is to test
 * the towerlights LED Control boards.
 *
 * For testing purposes, connect Port B to the Data port, Port D to the 
 * Select1 port, and Port A to the Select2 port on the towerlights board.
 * 
 * This test program turns on each room full, in sequence, then turns off each
 * room in sequence. 
 * 
 * To set up the board for this test, place a jumper in any position evenly
 * divisible by 3 (0, 3, 6, 9) in the RED column, place a jumper in position
 * (1, 4, 7, 10, etc) in the GREEN column, and place a jumper in position
 * (2, 5, 8, 11, etc) in the BLUE column.
 *            
 *
 */

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
// #include <stdio.h>
#include "twrlights.h"

// Prototypes because I don't have "twrlights.h"
void helicopter(int nrooms);

/* avr-specific data definitions. 
 *
 * The following counters are used to time iterations
 */

uint8_t cyclecnt = 0;  // counts 1000's of iterations
int cntr = 0;          // counts each cycle

/* This variable keeps track of which PWM clock phase we are on. Updated
 * by the timer ISR.
 */
uint8_t clkphase = 0;

/* The following routine does the actual output of the data to the
 * towerlights boards. It outputs a byte to Port B, then toggles a select
 * line, starting with line 0 and going through 15. This routine assumes
 * the first 8 lines are on Port D, the next seven lines are on Port A
 */

void outdata(uint8_t clknum)
   {
    volatile uint8_t select;
//  volatile uint8_t select1;
    
    uint8_t i;

    select = 1;
//  select1 = 1;
    for(i = 0; i < 8; i++)
      {
       PORTB = outarray[i][clknum]; // put data into Port B 
       PORTD = select;              // toggle select bit in PORT D
//     PORTD = select1;              // toggle select bit in PORT D
//     select1 = 0;
       PORTD = 0;
//     PORTD = select1;
       select = select << 1;
//     select1 = select;
      }

    select = 1;
//  select1 = 1;
    for(i = 8; i < 15; i++)
      {
       PORTB = outarray[i][clknum];   // put data into Port B 
       PORTA = select;        // toggle select bit in PORT A
//     PORTA = select1;        // toggle select bit in PORT A
//     select1 = 0;
       PORTA = 0;
//     PORTA = select1;
       select = select << 1;
//     select1 = select;
       PORTB = 0;
      }

   } // END outdata



/* wait-loop-based delay routine
 */

void delay (unsigned int dly)
  {
   volatile uint16_t i,j;

   for(j = 0; j< 10; j++)
      for(i = dly; i != 0; i--) ;

  } // END delay


/* Interrupt service routine for timer 0
 * Each time the timer goes off, we want to output the next set of PWM
 * bits to the towerlights board. 
 */

  
ISR(TIMER0_OVF_vect) 
  {
   outdata(clkphase);

   clkphase++;
   if(clkphase > 6) clkphase = 0;

   // the rest of this is unnecessary, except to count the number of
   // values that are output. Comment this out if this counting isn't 
   // necessary

/*    cntr++; // count this cycle
    if(cntr >= 1000)
      {
       cyclecnt++;
       PORTC = ~cyclecnt;
       cntr = 0;
      }
*/
  } // END ISR


int main(void)
 {
    uint8_t i, j, k, m, n;
    uint16_t speed;
    int nrooms;

// Set up ports A-D for output

    DDRA = 0xff;
    DDRB = 0xff;
    DDRC = 0xff;
    DDRD = 0xff;

// initialize the select ports
    PORTD = 0xff;
    PORTA = 0;

    PORTC = 0x00;  // set counter port to all ones, to signal start
    delay(5000U);
//  delay(65000U);
//  delay(65000U);
//  delay(65000U);
  
    cyclecnt = 0;
    PORTC = ~cyclecnt;  // initialize counter port to 0

// Set up timer 0 for a 2mS interrupt
// At 8MHz, this requires an x64 prescalar value (period = (N *256)/fosc)

   TCCR0 = (1<<CS01)|(1<<CS00);  // Timer clock = Sysclk/64
   TIFR  = 1<<TOV0;              // Clear TOV0, any pending interrupts
   TIMSK = 1<<TOIE0;             // Enable Timer0 Overflow interrupt


// Clear input array

    for(i = 0; i < 120; i++)
       inarray.linear[i] = 0;

    // turn each room on full blast
//    outroom(0, 255,255,255);
//    newframe();
//    delay(65000U);


// This loop is used to time how long it takes to update the LED's, starting
// with data from the inarray that will normally come from the uart
// Don't use timers for this test

/*    cli();       // Clear interrupt bit, so we don't use timer
    while (1)
      {
       docycle();

       cntr++; // count this cycle
       if(cntr >= 1000)
         {
          cyclecnt++;
          PORTC = ~cyclecnt;
          cntr = 0;
         }

      }
*/

// if the above code is commented out, we will do this

   sei();    // turn on interrupts
   nrooms = 22;  // number of rooms involved

 
   while(1)
     {
       cyclecnt = 0;
       PORTC = ~cyclecnt;
       speed = 8000U;
		
       for(j = 255; j > 1; j--)
            {
             outroom(i,   j,  0,  0);  // Red
             newframe();
             delay(speed);
            }

          for(j = 255; j > 1; j--)
            {
             outroom(i,   j,  j,  j);  // white
             newframe();
             delay(speed);
            }

		delay(speed);		    
	
		helicopter(8);  
	} // END while(1)

    return(0); 


}  // END main


/* Helicopter does a signle bright light in front on full brightness, this light
	then runs in a loop like a single helicopter blade */ 
void helicopter(int nrooms){
	int i;
	uint8_t speed = 800;
		
	for( i = 0; i < nrooms; i++ ){
		cyclecnt++;
		PORTC = ~cyclecnt;
		outroom( i, 255, 255, 255 );
		outroom( i-1, 116, 116, 166 );	
		outroom( i-2, 50, 50, 59 );
		outroom( i-3, 0, 0, 0);
		newframe();
		delay(speed);
	}
	
}


/* colorCopter does the same as helicopter ( a racing light in a loop)
   except the lead light is white and the trailing lights are in decending
   order so it goes White->Blue->Yellow->Red->Off. Kinda like the leading
   light is white hot and cooling off from there. */
void colorCopter( int nrooms ){
	int i;
	uint8_t speed = 800;

	for( i = 0; i < nrooms; i++ ){
		cyclecnt++;
		PORTC = ~cyclecnt;
		outroom( i, 255, 255, 255 );
		outroom( i-1, 0, 0, 166 );
		outroom( i-2, 83, 83, 0 );
		outroom( i-3, 45, 0, 0 );
		outroom( i-4, 0, 0, 0 );
		newframe();
		delay( speed );
	}
}

// TODO finish up multicopter, will have multiple running lights
// I need to think of a way to effectivly calculate where the opposite
// lightbar is if the system is in a loop layout.
/*
void multiCopter( int nrooms ){
	int i, j;
	uint8_t speed = 800;
	
	for( i = 0; i < nrooms; i++ ){
		cyclecnt++;
		PORTC = ~cyclecnt;
		outroom( i, 255, 255, 255 );
		outroom( j, 255, 255, 255 );	
		outroom( i-1, 116, 116, 116 );	
		outroom( j-2, 116, 116, 116 );
		outroom( i-2, 50, 50, 59 );
		outroom( i-3, 0, 0, 0);
		newframe();
	}
} */
