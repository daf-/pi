#include <MIDI.h>

/* Cheap algorithm to detect amplitude spikes coming from piezo pads,
 * and send an OSC message over USB with pad and velocity information.
 *
 * Instead of waiting a full 'window' to report a hit,
 * we report the hit halfway through a window, and then wait
 * for the rest of the window to pass before we look for more hits.
 *
 * Dan Friedman and Tom Kearney, Jan 2013
 */

/* TODO:
 * Why does serial stop sending notes????
 */

#define LED 13

unsigned long curTime;
int curValue;
unsigned long startTime;
int maxValue;
int threshold = 100;
unsigned long windowSize = 10;  // milliseconds

void setup()
{
  startTime = 0;
  maxValue = 0;

  pinMode(0, INPUT);
  pinMode(LED, OUTPUT);

  MIDI.begin();
  Serial.begin(115200);
}

void loop()
{
  curTime = millis();
  curValue = analogRead(0);

  // report a hit if we're more than halfway through a window and haven't yet reported
  if ( curTime - startTime < windowSize && curTime - startTime >= windowSize/2
    && maxValue > 0 )
  {
    int vel = (int) 127 * ( (float) (maxValue / (1023.0 - threshold)) );
    MIDI.sendNoteOn(42, vel, 1);
    digitalWrite(LED, HIGH);
    delay(1);
    MIDI.sendNoteOff(42, 0, 1);
    digitalWrite(LED, LOW);
    //      Serial.println(vel);
    //      delay(windowSize/2);
    //      MIDI.sendNoteOff(42, 127, 1);
    maxValue = -1;
  }
  // record max value if we need to
  else if ( curTime - startTime < windowSize/2 && curValue > maxValue )
  {
    maxValue = curValue;
  }
  // start a window only if we've exited a 'full' window and are above the threshold
  else if ( curTime - startTime > windowSize && curValue > threshold )
  {
    startTime = curTime;
    maxValue = curValue;
  }
}



