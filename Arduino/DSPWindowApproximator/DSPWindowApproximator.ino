/* Estimates the duration of an amplitude 'window' from piezo sensors in a drum pad
 * (the time between going above the threshold value and then back below it).
 * Used to get a value for the windowSize variable in the main DSP program.
 * Also gauges the duration between the start of a spike and its peak amplitude.
 * Just tests the first analog input.
 *
 * Dan Friedman and Tom kearney, Jan 2013
 */

boolean window;
int threshold;
unsigned long start;
unsigned long maxVal[2];  // max[0] is time stamp, max[1] is value

void setup()
{
  window = false;
  threshold = 100;
  start = 0;
  maxVal[0] = 0;
  maxVal[1] = 0;
  pinMode(0, INPUT);
  Serial.begin(9600);
}

void loop()
{
  unsigned long time = millis();
  int value = analogRead(0);

  if (!window && value > threshold)
  {
    window = true;
    start = time;
  }
  else if (window && value > maxVal[1])
  {
    maxVal[0] = time;
    maxVal[1] = value;
  }
  else if (window && value < threshold)
  {
    window = false;
    int dur = time - start;
    Serial.println(dur);
//    String timeStr = String(maxVal[0]);
//    String valStr = String(maxVal[1]);
//    Serial.println(maxVal[0]+": "+maxVal[1]);
    int maxValTime = maxVal[0] - start;
    Serial.print(maxValTime);
    Serial.print(": ");
    Serial.print(maxVal[1]);
    Serial.println();
    maxVal[1] = 0;
  }
}
