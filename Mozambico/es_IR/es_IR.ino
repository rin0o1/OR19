// Arduino Uno  -->   TCRT5000
// 5v           --->   VCC
// Grnd         --->   Grnd
// D8           --->   D0
const int pinIRd = 7;
const int pinLED = 13;    //debug
int IRvalueD = 0;

void setup()
{
  Serial.begin(9600);
  //se visualizza nero == 1, se bianco == 0 (false)
  pinMode(pinIRd, INPUT);
  pinMode(pinLED, OUTPUT);    //debug
}

void loop()
{
  Serial.print("\t Digital Reading=");
  Serial.println(IRvalueD);

  //lampeggio di debug
  if (IRvalueD == LOW) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  delay(1000);
  IRvalueD = digitalRead(pinIRd);
}
