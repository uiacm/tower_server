/** 
 * driver.h
 *
 * Provides methods for interfacing with the low level hardware
 */

#ifndef DRIVER_H
#define DRIVER_H

extern void leds_setup();
extern int leds_openAnimation(char *filename);
extern void leds_play();
extern void leds_stop();

#endif
