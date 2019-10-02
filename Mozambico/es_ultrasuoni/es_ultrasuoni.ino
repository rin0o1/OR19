#include <NewPing.h>

//trig, echo
NewPing* prova = new NewPing(13, 12);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(prova->ping_cm());
  delay(2000);
}
