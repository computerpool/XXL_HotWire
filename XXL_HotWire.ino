/* XL-Buzz Wire for Arduino
 * ======================================= 
 * Built by: CPU - Computer Pool Unterland 
 * Contributors: Richy (richard@gaun.eu) / A.Astner
 * Info: https://computerpool.at
 * v 0.1 - 05.05.2019
*/
// Autor: Richy (richard@gaun.eu)
// Geändert: 06.07.2019 --> Subroutines auslagern 
// Geändert: 07.07.2019 --> Einbindung von Routinen für WS2812B
// Geändert: 08.07.2019 --> Wechsel von Nano auf MEGA2560 wegen zu geringem Flashspeicher
// Geändert: 11.07.2019 --> Einbindung WS2812B
// Geändert: 12.07.2019 --> Optimierung Ablaufsteuerung

#include <SPI.h>
#include <MD_Parola.h>  // https://github.com/MajicDesigns/MD_Parola
#include <MD_MAX72xx.h> // https://github.com/MajicDesigns/MD_MAX72XX
#include "Font_Data.h"  // Font for MD_Parola
#include <StopWatch.h>  
#include <Button.h>
#include "var_XXL_HotWire.h"
#include "library_XXL_HotWire.h"
#include <FastLED.h>
#include <Adafruit_NeoPixel.h>
#include <TimerOne.h>						// Timer1 Interrupt für Encoder und Piezzo
#include <avr/wdt.h>





// Mehr Infos: https://arduino-projekte.webnode.at/registerprogrammierung/watchdog-interrupt/

// DISPLAY-Settings
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;

StopWatch stopwatch; 									// Init Stop-Watch

void setup(void){
  
	init_T1_interrupt();					// Initialisierung Timer1 Interrupt  
	btnStart.begin();										// Set Buttons
	btnWireHit.begin();
	btnGoal.begin();

	pinMode(ledPin, OUTPUT); 							// Integrated LED
	pinMode(Piezzo, OUTPUT); 							// Soundausgabe

	Serial.begin(115200);									// Setup Serial
	Serial.println(info);									// Output Info
	Serial.println(version);


	strcpy(curMessage, "Nun geht's LOS !");
	P.begin(2); 											// Display-Setup 
	P.setZone(0, 0, 15);
	P.displayZoneText(0, curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
	P.displayReset(0);
	P.synchZoneStart();

	strip.begin();									// for Adafruit_NeoPixel
	strip.show(); 									// Initialize Adafruit_NeoPixel all pixels to 'off' 
	wdt_reset(); // Reset Watchdog Timer	
	wdt_enable(WDTO_4S);
    
}


// MAIN-Loop
void loop(void){
	
	wdt_reset();
	
	currentMillis = millis();							// laufende Systemzeit in Variable speichern

	if(currentMillis - previousMillis >= interval) {	
		previousMillis = currentMillis;
		toggle = toggle^1;
		digitalWrite(ledPin, toggle);  
	}
	
		
	if(btnStart.pressed() && !me_started && !me_reset){	// Check for Start-Botton 
		me_started = true;								// setze Merker "gestartet"
		me_reset = false;
		prev_resetMillis = currentMillis;				// Reset Timer Reset
		startCountdown();	
		Serial.println("Starttaster gedrueckt");
  	}
	
	if(digitalRead(start_pin)){							// Pullup --> aktiv LOW 	
		prev_resetMillis = currentMillis;
		me_reset = false;		
	}
	
	if(currentMillis - prev_resetMillis >= reset_start){// Starttaster > 3 Sekunden gedrückt
		curState = game_idle;							// setze den Heissen Draht in Idle-Mode
		me_reset = true;
		me_started = false;
		Serial.println("RESET Heisser Draht");
		
		resetDisp();
	}

	if(btnWireHit.pressed() && curState == game_started){// Wire Hit in active Game
		wireHit();
		iCurMessage = 6;
		sound = 1000;
	}

	if(btnGoal.pressed() && curState == game_started){	// Goal Reached
		goalReached();    
	}




	if (P.displayAnimate()){ 							// animates and returns true when an animation is completed

		boolean bAllDone = true;
		
		if(curState == game_idle){
			for (uint8_t i=0; i < MAX_ZONES && bAllDone; i++){
				bAllDone = bAllDone && P.getZoneStatus(i);
			}
		}

		
			  
		if (bAllDone){ 										// do something as all zones have completed
			switch (curState){
		  
				case game_idle: 								// Idle-Mode
					P.setZone(0, 0, 15); 							// use all elements for one big display

					// ToDo: use more elegant/flexible solution (
					iCurMessage ++;
					if(iCurMessage >= 3) {
						iCurMessage = 0;
					}
					strcpy(curMessage, arrMessages[iCurMessage]);
					P.displayReset(0);
				break;
		  
				case game_countdown: 							// Countdown-Mode
				
					// if (iCountDown == 0){
						// startGame();
						// break;
					// }
					
					if (currentMillis - lastTime >= 1000){
						lastTime = currentMillis;
						

						itoa(iCountDown, cCountDown, 10);
						Serial.println(iCountDown);
					
						P.displayReset(0);
						P.displayReset(1);
						
					  
						switch (iCountDown){
							case 3:
								Serial.println("colorWipe Red");
								colorWipe(strip.Color(255, 0, 0), 2); // Red
								sound = 500;
							break;
							case 2:
								Serial.println("colorWipe Blue");
								colorWipe(strip.Color(0, 0, 255), 2); // Blue
								sound = 500;
							break;		
							case 1:
								Serial.println("colorWipe Green");
								colorWipe(strip.Color(0, 255, 0), 2); // Green
								sound = 500;
							break;								
							case 0:
								Serial.println("colorWipe White");
								colorWipe(strip.Color(255, 255, 255), 2); // White RGBW
								sound = 1000;
								startGame();
							break;		
						}
						iCountDown--;
					}
					
				break;
		  
				case game_started: 								// Game-Mode
					if (millis() - lastTime >= 100){ 				// every 100ms
						printTime();
						
						t_showText ++;	
						if(t_showText > 20){
							t_showText = 0;
							iCurMessage++;
							new_light = true;
							if(iCurMessage > 5){
								iCurMessage = 3;
							}
						}							

						if(new_light){
							new_light = false;
														
							switch (iCurMessage){
								case 0:
									colorWipe(strip.Color(20, 50, 20), 2); 
								break;
								case 1:
									colorWipe(strip.Color(50, 20, 60), 2); 
								break;
								case 2:
									colorWipe(strip.Color(20, 50, 70), 2); 
								break;
								case 3:
									colorWipe(strip.Color(10, 30, 20), 2); 
								break;
								case 4:
									colorWipe(strip.Color(30, 90, 30), 2); 
								break;
								case 5:
									colorWipe(strip.Color(40, 80, 20), 2); 
								break;
								case 6:
									colorWipe(strip.Color(70, 70, 80), 2); 
								break;
							}							
						}

						

							
						
						
					}   
					P.displayReset(0);     
										
				break;
		  
				case game_ended: 								// Goal-Mode
				
					if(currentMillis - prev_resetgame >= reset_game){// Starttaster > 3 Sekunden gedrückt
						curState = game_idle;							// setze den Heissen Draht in Idle-Mode
						me_reset = true;
						me_started = false;
						Serial.println("RESET Heisser Draht nach Ziel erreicht");
						resetDisp();
					}
					// ToDo: Switch back to ide-mode after X-Time?!?
				break;
			}	// END switch (curState)
	  
		}
	 }
	
   
}
