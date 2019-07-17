/* XL-Buzz Wire for Arduino
 * ======================================= 
 * Built by: CPU - Computer Pool Unterland 
 * Contributors: Richy (richard@gaun.eu)
 * Info: https://computerpool.at
 * v 0.1 - 05.05.2019
*/
// Autor: Astner A.
// Erstellt: 06.07.2019 --> Subroutines auslagern 
// Geändert: 

#ifndef __lib_XXL_HotWire_h
    #define __lib_XXL_HotWire_h

void init_T1_interrupt();					// Initialisierung für den Timer1 Interrupt
void timerIsr(void);						// Funktionsaufruf bei Timer1 Interrupt
void startGame();
void startCountdown();
void printTime();
void wireHit();
void goalReached();
void resetDisp();
void Fire2012WithPalette();


void colorWipe(uint32_t c, uint8_t wait);		// Fill the dots one after the other with a color
void colorWipe_more(uint8_t wait);
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
void theaterChase(uint32_t c, uint8_t wait);	//Theatre-style crawling lights.
void theaterChaseRainbow(uint8_t wait);			//Theatre-style crawling lights with rainbow effect
uint32_t Wheel(byte WheelPos);					// Input a value 0 to 255 to get a color value.
												// The colours are a transition r - g - b - back to r.

#endif
