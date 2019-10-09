/*
digitalWrite() invia bool elettrico
pinMode() imposta il pin
pulseIn() timer impulsi da low ad high vice versa
*/

//SUMO


#include <Servo.h>
#include <PID_v1.h>


//pid

#define PIN_INPUT 0
#define PIN_OUTPUT 3
double Setpoint, Input, Output;
//Specify the links and initial tuning parameters
double Kp = 2, Ki = 5, Kd = 1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);


// Valori Infrarossi
#define BIANCO 260
int irs = 4;
int irc = 2;
int ird = 3;


// Direzioni Servo
#define Fermo 90
#define Sinistra_Avanti 0
#define Sinistra_Dietro 180
#define Destra_Avanti 180
#define Destra_Dietro 0


#define trigPinDestra 6
#define echoPinDestra 5
#define trigPinSinistra 8
#define echoPinSinistra 7
#define trigPinCentro 11
#define echoPinCentro 12
/*
  #define Fermo 90
  #define Sinistra_Avanti 60
  #define Sinistra_Dietro 120
  #define Destra_Avanti 120
  #define Destra_Dietro 0*/

int STATO = 0;
int STATO2;
int STATO3 = 0;
int Valore_Destra = 0;
int Valore_Sinistra = 0;
int Valore_Avanti = 0;
bool In_Range_Destra;
bool In_Range_Avanti;
bool In_Range_Sinistra;
bool Bianco_Destra;
bool Bianco_Avanti;
bool Bianco_Sinistra;


class Sensor   /* Sensore ultrasuoni*/
{
  private:
    int Pin_Trig;
    int Pin_Echo;


  public:

    //ULTRASUONI
    Sensor(int pintrig, int pinecho)    //costruttore
    {
      SetPin(pintrig, pinecho);
      pinMode(pintrig, OUTPUT);   //imposto in output, manda mex
      pinMode(pinecho, INPUT);    //imposto in input, riceve signal
    }


    /*
    procedura setta pin ultrasuoni
    */
    void SetPin(int pintrig, int pinecho)
    {
      Pin_Trig = pintrig;
      Pin_Echo = pinecho;
    }


    //Funzione ritorna distanza tra ultra-ostacolo int64
    long GetDistance()
    {
      digitalWrite(Pin_Trig, LOW);
      delayMicroseconds(150);
      digitalWrite(Pin_Trig, HIGH);
      delayMicroseconds(150);
      digitalWrite(Pin_Trig, LOW);
      float  duration = pulseIn(Pin_Echo, HIGH);  //rileva tempo al rilevamento ostacolo
      float distance_m = (duration / 2) / 343.4;
      float distance_cm = distance_m / 10; //
      return distance_cm;

    }


    //
    bool ControlloAvversario()
    {
      if (GetDistance() < 78) 
        return true; 
      else 
        return false;
    }
};


/* Inizializzazione classi servo */
Servo Ruota_Destra;
Servo Ruota_Sinistra;

void stopWheels() {
  Ruota_Sinistra.write(Fermo);
  Ruota_Destra.write(Fermo);
}

void goForwards() {
  Ruota_Sinistra.write(Sinistra_Avanti);
  Ruota_Destra.write(Destra_Avanti);
}


void goBackwards() {
  Ruota_Sinistra.write(Sinistra_Dietro);
  Ruota_Destra.write(Destra_Dietro);
}

void VAI_SINISTRA(int quanto) {
  int gradazione;
  Ruota_Destra.write(Destra_Avanti);
  switch (quanto) {
    case 1:
      gradazione = 60;
      break;
    case 2:
      gradazione = 65;
      break;
    case 3:
      gradazione = 70;
      break;
    case 4:
      gradazione = 80;
      break;
    case 5:
      gradazione = 95;
      break;
  }
  //gradazione -= 90;
  Ruota_Sinistra.write(gradazione);
}



void VAI_DESTRA(int quanto) {
  int gradazione;
  Ruota_Sinistra.write(Sinistra_Avanti);
  switch (quanto) {
    case 1:
      gradazione = 130;
      break;
    case 2:
      gradazione = 120;
      break;
    case 3:
      gradazione = 115;
      break;
    case 4:
      gradazione = 105;
      break;
    case 5:
      gradazione = 85;
      break;
  }
  Ruota_Destra.write(gradazione); /* Destra da 90 a 180 */
}


void INDIETRO_DESTRA(int quanto) {
  int gradazione;
  Ruota_Sinistra.write(Sinistra_Dietro);
  switch (quanto) {
    case 1:
      gradazione = 60;
      break;
    case 2:
      gradazione = 70;
      break;
    case 3:
      gradazione = 80;
      break;
    case 4:
      gradazione = 85;
      break;
    case 5:
      gradazione = 95;
      break;
  }
  Ruota_Destra.write(gradazione); /* Destra da 90 a 180 */
}


void INDIETRO_SINISTRA(int quanto)
{
  int gradazione;
  Ruota_Destra.write(Destra_Dietro);
  switch (quanto) {
    case 1:
      gradazione = 120;
      break;
    case 2:
      gradazione = 110;
      break;
    case 3:
      gradazione = 100;
      break;
    case 4:
      gradazione = 95;
      break;
    case 5:
      gradazione = 85;
      break;
  }
  Ruota_Sinistra.write(gradazione); /* Destra da 90 a 180 */
}




void Intro()
{
  VAI_SINISTRA(5);    //direzione impercettibile        //la chiave sta nell' intro
  delay(100);
  VAI_SINISTRA(3);    //tendente a sx
  delay(1100);
  INDIETRO_DESTRA(3); //indietro lento destra
  delay(1100);
  INDIETRO_DESTRA(4);     //indietro dx lesto
  delay(600);
  INDIETRO_SINISTRA(3);    //indietro sx
  delay(800);
  stopWheels();

}
//Sensor* DESTRA = new Sensor(3, 6);
//Sensor* FRONTE = new Sensor(12, 13);
//Sensor* SINISTRA = new Sensor(10, 11);
void fuga() {

  goForwards();
  //delay(300);
  if (analogRead(irs) < BIANCO) {
    Bianco_Sinistra = HIGH;
    // delay(50);
  }
  else {
    Bianco_Sinistra = LOW;
    //  delay(50);
  }
  if (analogRead(irc) < BIANCO) {
    Bianco_Avanti = HIGH;
    // delay(50);
  }
  else {
    Bianco_Avanti = LOW;
    //  delay(50);
  }
  if (analogRead(ird) < BIANCO) {
    Bianco_Destra = HIGH;
    // delay(50);
  }
  else {
    Bianco_Destra = LOW;
  }
 // if (FRONTE->ControlloAvversario()) {
  //  delay(100);
 // }
  if (Bianco_Destra || Bianco_Avanti || Bianco_Sinistra) {
    goBackwards();
    delay(2000);
    int randomic = random(2999);
    if (random(50) % 2) {
      VAI_SINISTRA(5);
      delay(500);
    }
    else {
      VAI_DESTRA(5);
      delay(500);
    }
  }

}


void setup() {
  // put your setup code here, to run once:
  randomSeed(2340234);
  Serial.begin(9600);
  Ruota_Sinistra.attach(10); //TODO: Controllare i pin dei servo
  Ruota_Destra.attach(9); //TODO: Controllare i pin dei servo

  delay(5000);
  Intro();
}

void loop() {
  fuga();
}

