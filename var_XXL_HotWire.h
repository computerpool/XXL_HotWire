/* XL-Buzz Wire for Arduino
 * ======================================= 
 * Built by: CPU - Computer Pool Unterland 
 * Contributors: Richy (richard@gaun.eu)
 * Info: https://computerpool.at
 * v 0.1 - 05.05.2019
*/
// Autor: Richy (richard@gaun.eu)
// Geändert: 06.07.2019 --> Subroutines auslagern 
// Geändert: 07.07.2019 --> Einbindung von Routinen für WS2812B
// Geändert: 08.07.2019 --> Wechsel von Nano auf MEGA2560 wegen zu geringem Flashspeicher

#include <FastLED.h>
#include <Adafruit_NeoPixel.h>

#ifndef __var_XL_Buzz_Wire_h
    #define __var_XL_Buzz_Wire_h


bool me_started = false;
bool me_reset = false;
const String info = "CPU-Heisser Draht";
const String version = "v 0.2 - 07.05.2019";

uint8_t scrollSpeed = 60;    							// default frame delay value
uint16_t scrollPause = 2000; 							// in milliseconds
byte WheelPos = 0;

// SETTINGS
Button  btnStart(7);     								// Start-Button
#define start_pin 7
Button  btnWireHit(8);   								// Wire-Hit
Button  btnGoal(9);      								// Reached Goal

// DISPLAY-Settings
// Define the number of devices we have in the chain and the hardware interface
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 16
#define CLK_PIN   11
#define DATA_PIN  12
#define CS_PIN    10


// Defines WS2812B



#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define NUM_LEDS    60

#define BRIGHTNESS  52
#define FRAMES_PER_SECOND 60

bool gReverseDirection = false;

CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
#define LED_PIN     2					// for Adafruit_NeoPixel
#define NUMPIXELS      60
int delayval = 100; 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

#define T1_interrupt 100				// Timer1 Interrupt 100 uSec --> 0,100mS
static byte sound = 0;								// Dauer für den Piepston
const int ledPin = 13; 									// integrated LED (debugging only)
const int Piezzo = 22;									// Pin für die Signalausgabe bei Fehler

// GAME-SETTINGS
int iCountDownStart = 3; 								// Number of CountDown-Seconds
int iMaxHitsAllowed = 3; 								// Max. Hits allowed

// Global message buffers shared by Serial and Scrolling functions
#define  BUF_SIZE  75

// Display Messages in Idle-State
const char* arrMessages[] = { "CPU-Heisser Draht!!",
                              "Drueck' den Button!",
							  "Versuch dein Glueck",
							  "Ja, gut in der Zeit",
							  "ein wenig schneller",
							  "wird langsam Zeit!!",
							  "Autsch, das tat weh"};
                          
int iCurMessage = 0;
int t_showText = 0;
bool new_light = false;
char curMessage[BUF_SIZE] = {arrMessages[iCurMessage]}; // holds current Message in text-Zone

// GLOBAL VARS
int iCountDown = iCountDownStart; 
char cCountDown[5];

// Buffer Current Game-State
#define game_idle 0
#define game_countdown 1
#define game_started 2
#define game_ended 3
static uint8_t curState = 0;  // 0=idle / 1=countdown / 2=game started / 3=game ended

static uint32_t iCountdownStartBuffer = 0; // millis() buffer
static uint32_t lastTime = 0; // millis() buffer for Stopwatch

char cTime[9]; // Char for Clock in Timer

int iHitCount = 0; // Wire-Hit Counter


bool toggle = false;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
unsigned long prev_mqttMillis = 0;
unsigned long prev_muxMillis = 0;
unsigned long prev_dispMillis = 0;
unsigned long prev_resetMillis = 0;
unsigned long prev_resetgame = 0;


int prev_second = 0;
int interval = 1000;           				// Blinkinterval (milliseconds)
int muxinterval = 200;           			// Multiplexinterval (milliseconds)
int anzeigedauer = 1000;					// Anzeigedauer auf dem Grafikdisplay
int reset_start = 3000;						// Starttaster 3 Sekunden drücken für Reset
int reset_game = 5000;						// Goto Idle-State nach Ziel erreicht

#endif
