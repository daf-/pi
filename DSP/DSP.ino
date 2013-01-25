/* Cheap algorithm to detect amplitude spikes coming from piezo pads,
 * and send an OSC message over USB with pad and velocity information.
 *
 * Dan Friedman and Tom Kearney, Jan 2013
 */

// TODO:
// * implement OSC or serial messaging to integrate with Max/MSP
// * implement half-windowing algorithm, where we report a hit after half of the window,
//   since the peak amplitude has likely been acheived by then. don't report any hits for
//   the next half window. This should increase response time.
// * experiment with different threshold and window size values.
// * why can't we declare startTimes and maxValues as startTimes[numberOfPads] and maxValues[numberOfPads]?

int numberOfPads = 5;
unsigned long curTime;
int curValue;
int startTimes[5];
int maxValues[5];
int threshold = 100;
unsigned long windowSize = 20;
  
void setup() {  
  for (int i=0; i<numberOfPads; i++) {
    startTimes[i] = 0;
    maxValues[i] = 0;
    pinMode(i,INPUT);
  }
  pinMode(13, OUTPUT);  // enable LED for testing
}

void loop(){
  for (int i=0; i<numberOfPads; i++) {
    curTime = millis();
    curValue = analogRead(i);
    //cases
    if (curTime - startTimes[i] > windowSize) {  //we are past the window
      if (maxValues[i] > 0) { //exiting window, send a message
        //TODO: construct OSC message
        // test: light up led on attack
        digitalWrite(13, HIGH);
        delay(5);
        digitalWrite(13, LOW);
        maxValues[i] =- 1;
      }
      if (curValue > threshold) {  //opening a window
        startTimes[i] = curTime;
        maxValues[i] = curValue;
      }
    } else if(curValue > maxValues[i]) {  //in a window and greater than previous max
      maxValues[i] = curValue;
    }
  }
}
