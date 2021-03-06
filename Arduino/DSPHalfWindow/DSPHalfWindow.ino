#include <MIDI.h>

/* Cheap algorithm to detect amplitude spikes coming from piezo pads,
 * and send an OSC message over USB with pad and velocity information.
 *
 * Instead of waiting a full 'window' to report a hit,
 * we report the hit a fraction of a way through the window, and then wait
 * for the rest of the window to pass before we look for more hits.
 *
 * "Hits" are currently MIDI notes, begining at C2 (pad 0), and increasing
 * by half step.
 *
 * Dan Friedman and Tom Kearney, Jan 2013
 */
 
/* TODO:
 * Why can't we use nPads in declaration of arrays?
 */

int nPads = 6;                 // number of drum pads to consider
unsigned long curTime;         // the current time every iteration of loop()
int curValue;                  // the current analog value of of each piezo pad each time through loop()
unsigned long startTimes[6];   // the most recent time a window was opened for each pad
int maxValues[6];              // keeps the current maximum amplitude for each pad
int threshold = 100;           // ignore all values less than the threshold
unsigned long windowSize = 20; // length of a window in milliseconds
int fraction = 4;              // fraction of a window we wait before reporting the velocity of a hit

void setup()
{
  for (int i=0; i<nPads; i++)
  {
    startTimes[i] = 0;
    maxValues[i] = 0;
    pinMode(i, INPUT);
  }

  MIDI.begin();
  Serial.begin(115200);
}

void loop()
{
  for (int i=0; i<nPads; i++)
  {
    curTime = millis();
    curValue = analogRead(i);

    // report a hit if we're more than halfway through a window and haven't yet reported
    if ( curTime - startTimes[i] < windowSize && curTime - startTimes[i] >= windowSize/fraction
      && maxValues[i] > 0 )
    {
      int vel = (int) 127 * ( (float) (maxValues[i] / (1023.0 - threshold)) );
      int note = 36 + i;
      MIDI.sendNoteOn(note, vel, 1);
      MIDI.sendNoteOff(note, 0, 1);
      maxValues[i] = -1;
    }
    // record max value if we need to
    else if ( curTime - startTimes[i] < windowSize/fraction && curValue > maxValues[i] )
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

