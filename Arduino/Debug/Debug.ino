int threshold = 100;

int curVal;
int curTime;

void setup()
{
  pinMode(0, INPUT);
  pinMode(13, OUTPUT);
  Serial.begin(115200);
}

void loop()
{
  curVal = analogRead(0);
  if ( curVal > threshold )
  {
    Serial.println(curVal);
  }
}
