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

int nPads = 5;
unsigned long curTime;
int curValue;
unsigned long startTimes[5];
int maxValues[5];
int threshold = 100;
unsigned long windowSize = 20;  // milliseconds

void setup()
{
  for (int i=0; i<nPads; i++)
  {
    startTimes[i] = 0;
    maxValues[i] = 0;
    pinMode(i, INPUT);
  }
  
  pinMode(LED, OUTPUT);

  MIDI.begin();
  Serial.begin(115200);
}

void loop()
{
//  delay(5);
  for (int i=0; i<1; i++)
  {
    curTime = millis();
    curValue = analogRead(i);

    // report a hit if we're more than halfway through a window and haven't yet reported
    if ( curTime - startTimes[i] < windowSize && curTime - startTimes[i] >= windowSize/2
      && maxValues[i] > 0 )
    {
      int vel = (int) 127 * ( (float) (maxValues[i] / (1023.0 - threshold)) );
      MIDI.sendNoteOn(42, vel, 1);
      digitalWrite(LED, HIGH);
      delay(1);
      MIDI.sendNoteOff(42, 0, 1);
      digitalWrite(LED, LOW);
//      Serial.println(vel);
//      delay(windowSize/2);
//      MIDI.sendNoteOff(42, 127, 1);
      maxValues[i] = -1;
    }
    // record max value if we need to
    else if ( curTime - startTimes[i] < windowSize/2 && curValue > maxValues[i] )
    {
      maxValues[i] = curValue;
    }
    // start a window only if we've exited a 'full' window and are above the threshold
    else if ( curTime - startTimes[i] > windowSize && curValue > threshold )
    {
      startTimes[i] = curTime;
      maxValues[i] = curValue;
    }
  }
}

