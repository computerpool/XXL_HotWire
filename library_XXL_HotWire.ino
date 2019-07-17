/* XL-Buzz Wire for Arduino
 * ======================================= 
 * Built by: CPU - Computer Pool Unterland 
 * Contributors: Richy (richard@gaun.eu)
 * Info: https://computerpool.at
 * v 0.1 - 05.05.2019
*/
// Autor: Astner A.
// Erstellt: 06.07.2019 --> Subroutines auslagern 
// Geändert: 11.07.2019 --> Einbindung WS2812B

#include <FastLED.h>
#include <Adafruit_NeoPixel.h>
#include "var_XXL_HotWire.h"


///////////////////////////////////////////////////////////////////////
//////////////// Funktionsaufruf bei Timer1 Interrupt  ////////////////
///////////////////////////////////////////////////////////////////////
	void timerIsr(void) {
		if(sound){										// Zeit "Piepston" noch nicht abgelaufen
			sound --;
			digitalWrite(Piezzo, true);
		}
		else {
			sound = 0;
			digitalWrite(Piezzo, false);
		}  
	}

///////////////////////////////////////////////////////////////////////
///////// Funktion zur Initialisierung für den Drehimpulsgeber  ///////
///////////////////////////////////////////////////////////////////////	
	void init_T1_interrupt(){

		Timer1.initialize(T1_interrupt);
		Timer1.attachInterrupt(timerIsr); 
	}	// END void init_T1_interrupt()		



void startGame(){
  Serial.println("Start Game");
  curState = game_started; 								// Set Game-State
  iHitCount = 0; 										// Reset Hit-counter
  
  P.setZone(0, 0, 4);									// Init Zones
  P.setZone(1, 5, 15);
  P.setFont(0, numeric7Seg);

  stopwatch.start();
  
  strcpy(curMessage, "LOS!");
  
  strcpy(cTime, "00:00");
  P.displayZoneText(0, cTime, PA_CENTER, scrollSpeed, 0, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(1, curMessage, PA_CENTER, scrollSpeed, 0, PA_PRINT, PA_NO_EFFECT);
 
  iCurMessage = 3;
}


void startCountdown(){
  Serial.println("Start CountDown");
  curState = game_countdown; 							// Set State to Countdown-Mode
  iCountDown = iCountDownStart; 
  itoa(iCountDown, cCountDown, 10);
  
  lastTime = millis(); 									// Buffer First Time
    
  P.setZone(0, 0, 1);									// Init Zones
  P.setZone(1, 2, 15);

  strcpy(curMessage, "Start in");
  
  Serial.print("Start in ");
  Serial.println(iCountDown);									
  
  P.displayZoneText(0, cCountDown, PA_CENTER, scrollSpeed, scrollPause, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(1, curMessage, PA_CENTER, scrollSpeed, 0, PA_FADE, PA_NO_EFFECT);

  P.displayReset(0);
  P.displayReset(1);

  iCountdownStartBuffer = millis();						// start Timer
}


void printTime(){
  lastTime = millis();
  uint32_t  tmil;
  int tsec, tmin ;

  tmil = stopwatch.ms();								// Get and calulate Values
  tsec = tmil/1000;
  tmin = tsec/60;
  tmin %= 60;
  tsec %= 60;
  tmil %= 1000;
  
  String curTimer;
  curTimer = String(tmin) + ":";
  if (tsec <= 9){
	curTimer += "0";  									// Prepend 0 to seconds
  } 										
    
  curTimer += String(tsec) + "." + String(tmil)[0]; 
  curTimer.toCharArray(cTime, 75);
  
  // Serial.println(stopwatch.sec());
  // Serial.println(cTime);
  
  strcpy(curMessage, arrMessages[iCurMessage]);
        
  P.displayReset(0);
  P.displayReset(1);
}


void wireHit(){
  Serial.println("HIT!");
  iHitCount++;  

  String strHitCount = "HIT: " + iHitCount;
  strHitCount.toCharArray(curMessage, 30);  
  
  //strcpy(curMessage, strHitCount);
  P.displayReset(1);  
}


void goalReached(){
  Serial.println("Bravo!");
  curState = game_ended; 					// Set Mode to Goal reached
  prev_resetgame = currentMillis;
    
  // Stop Timer
  stopwatch.stop();

  strcpy(curMessage, "Bravo! Gut gemacht");
  P.displayReset(1);
  
}


void resetDisp(){
	

	while(1);										// Endlosschleife um den Watchdog auszulösen
	
}


// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100 
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120


void Fire2012WithPalette(){
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++){
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--){
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ){
      int y = random8(7);
      heat[y] = qadd8(heat[y], random8(160,255));
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++){
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if(gReverseDirection){
        pixelnumber = (NUM_LEDS-1) - j;
      } 
	  else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}



// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void colorWipe_more(uint8_t wait){
	  // Some example procedures showing how to display to the pixels:
	Serial.println("colorWipe Red");
	colorWipe(strip.Color(255, 0, 0), wait); // Red
	Serial.println("colorWipe Green");
	colorWipe(strip.Color(0, 255, 0), wait); // Green
	Serial.println("colorWipe Blue");
	colorWipe(strip.Color(0, 0, 255), wait); // Blue
	Serial.println("colorWipe White");
	colorWipe(strip.Color(0, 0, 0, 255), wait); // White RGBW
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*1; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}



//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
