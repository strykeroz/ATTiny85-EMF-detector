/*
EMF Detector for charlieplexed ATTiny LED Bargraph v1.0
 5.12.2009
 original code/project by Aaron ALAI - aaronalai1@gmail.com
 modified for use w/ LED bargraph by Collin Cunningham - collin@makezine.com
 modified for ATTiny85 and Charlieplexed LEDs by Geoff Steele - geoffs_aus@hotmail.com
 
 ATTiny85 connections
 Leg  Function
 1    Reset, no connection
 2    D3 GREEN charlieplex block
 3    D4 ORANGE charlieplex block
 4    GND
 5    D0 WHITE charlieplex block
 6    D1 BLUE charlieplex block
 7    A1 probe
 8    +5V
 
 Tested on ATTiny85 running at 8MHz.  Utilises a 10 LED charlieplexed matrix
 
 */

#define NUMREADINGS 4                                // raise this number to increase data smoothing
// each block of 3 LEDs in the array is groupled by a common anode (+, long leg)
// for simplicity of wiring, using a colour code
#define GREEN 0
#define ORANGE 1
#define WHITE 2
#define BLUE 3

// pin definitions {GREEN, ORANGE, WHITE, BLUE}
const int charliePin[5] = {
  3, 4, 0, 1};

// Charlieplexed LED definitions (current flowing from-to pairs)
const int LED[20][2] = {
  {ORANGE, GREEN},                                    // 0 (GREEN GROUP)
  {WHITE, GREEN},                                     // 1
  {BLUE, GREEN},                                      // 2
  {GREEN, ORANGE},                                    // 3 (ORANGE GROUP)
  {WHITE, ORANGE},                                    // 4
  {BLUE, ORANGE},                                     // 5
  {GREEN, WHITE},                                     // 6 (WHITE GROUP)
  {ORANGE, WHITE},                                    // 7
  {BLUE, WHITE},                                      // 8 
  {GREEN, BLUE},                                      // 9 (BLUE GROUP)
  {ORANGE, BLUE},                                     // 10 NOT USED (but available if you want to)
  {WHITE, BLUE},                                      // 11 NOT USED (but available if you want to)
};

// LED variables
int previous = 0;                                     // the most recent LED lit

// pin definitions
const int probePin = A1;                              // ATTiny leg 7

// constants & variables for smoothing
int smoothedValue = 0;                                // probe reading that is displayed
const int senseLimit = 15;                            // raise this number to decrease sensitivity (up to 1023 max)

// threshholds for lighting each led
const int LEDgate[10] = {50, 150, 250, 350, 450, 550, 650, 750, 850, 950};

void setup() {
}

void loop() {
  int newValue = analogRead(probePin);                // take a reading from the probe
  if (newValue > 0) {                                 // ignoring zeroes
    newValue = constrain(newValue, 1, senseLimit);      // turn any reading higher than the senseLimit value into the senseLimit value
    newValue = map(newValue, 1, senseLimit, 1, 1023);   // remap the constrained value within a 1 to 1023 range
  
    // generate smoothed output value
    if(newValue > smoothedValue) 
      smoothedValue = smoothedValue + (newValue - smoothedValue) / NUMREADINGS; 
    else 
      smoothedValue = smoothedValue - (smoothedValue - newValue) / NUMREADINGS;
  }
  unsigned long loopUntil = millis() + 30;
  while(millis() <= loopUntil) {
    // drive charlieplexed bar graph
    for(int i = 0; i<10; i++)
      if (smoothedValue > LEDgate[i])  charlieON(i);  // if the value is over 50 ...light the first LED and so on ...
  }

} // rinse, repeat

// --------------------------------------------------------------------------------
// turns on LED #thisLED.  Turns off all LEDs if the value passed is out of range
//
void charlieON(int thisLED) {
  // turn off previous (reduces overhead, only switch 2 pins rather than 5)
  digitalWrite(charliePin[LED[previous][1]], LOW);   // ensure internal pull-ups aren't engaged on INPUT mode
  pinMode(charliePin[LED[previous][0]], INPUT);
  pinMode(charliePin[LED[previous][1]], INPUT);

  // turn on the one that's in focus
  if(thisLED >= 0 && thisLED <= 19) {
    pinMode(charliePin[LED[thisLED][0]], OUTPUT);
    pinMode(charliePin[LED[thisLED][1]], OUTPUT);
    digitalWrite(charliePin[LED[thisLED][0]], LOW);
    digitalWrite(charliePin[LED[thisLED][1]], HIGH);
  }
  previous = thisLED;
}

// --------------------------------------------------------------------------------
// turns off LED #thisLED.  
//
void charlieOFF(int thisLED) {
  digitalWrite(charliePin[LED[thisLED][1]], LOW);   // ensure internal pull-ups aren't engaged on INPUT mode
  pinMode(charliePin[LED[thisLED][0]], INPUT);
  pinMode(charliePin[LED[thisLED][1]], INPUT);
}



