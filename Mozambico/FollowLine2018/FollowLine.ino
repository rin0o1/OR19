/*  LIBRERIA PID
   Algoritmo basato sulla correzione dell’errore dato un INPUT al fine di ragiungere il setpoint desiderato controllando l’OUTPUT

   Esempio:
   Se ti riferissi ad un automobile in cui l’INPUT è la velocità ed il setpoint è la velocità a cui desideri andare l’OUTPUT è l’angolo da imporre al pedale dell’accelleratore.

   Costruttore:
   Input: di tipo double, è la variabile contenente la rilavazione eseguita dal sistema
   Output: di tipo double, è la variabile con cui imponi un comando al sistema
   Setpoint: di tipo double, è il punto di arrivo desiderato
   Kp, Ki, Kd: di tipo double >= 0,
        rappresentano le tre costati mediante cui l’algoritmo modifica il proprio comportamento per raggiungere il punto desiderato
   Direction: puoi definirla DIRECT o REVERSE e determina la direzione in cui si sposterà l’output in funzione dell’errore rilevato per raggiungere il setpoint
*/

//Librerie
#include <Servo.h>      //servo motori
#include <PID_v1.h>     //proporzionale integrale derivato

//Parametri Libreria PID
#define PIN_INPUT 0
#define PIN_OUTPUT 3
double Setpoint, Input, Output;
//Specify the links and initial tuning parameters
double Kp = 2, Ki = 5, Kd = 1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

#pragma region PIN COMPONENTISTICA

//Ruote
#define PIN_RUOTA_SINISTRA  8
#define PIN_RUOTA_DESTRA  9

//Infrarossi
#define PIN_INFRA_DESTR_DIG  7    //staccare ?
#define PIN_INFRA_DESTR_ANAG  A1
#define PIN_INFRA_AV_DIG  5    //staccare ?
#define PIN_INFRA_AV_ANAG  A2
#define PIN_INFRA_SINIST_DIG  3    //staccare ?
#define PIN_INFRA_SINIST_ANAG  A3

#pragma endregion

//Valori Infrarossi Colori
#define NERO 300
#define BIANCO 80

//Direzioni Servo (gradi)
#define SERV_STOP 90
#define SERV_SINIST_AV 0
#define SERV_SINIST_DIETR 180
#define SERV_DEST_AV 180
#define SERV_DEST_DIETR 0

//? (Presunti stati led infrarossi)
int STATO = 0;
int STATO2 = 0;
int STATO3 = 0;

//? (altri presunti valori led infrarossi)
int Valore_Destra = 0;
int Valore_Sinistra = 0;
int Valore_Avanti = 0;

//? (Deve sterzare perchè non più in range) forse
bool In_Range_Destra;
bool In_Range_Avanti;
bool In_Range_Sinistra;

//Bool B&N
bool Bianco_Destra;
bool Bianco_Avanti;
bool Bianco_Sinistra;
bool Nera_Destra;
bool Nero_Avanti;
bool Nero_Sinistra;

//classe componente infrarossi casereccia
class Infrarossi {

  private:
    int Ultima_Lettura;
    int Pin_digitale;
    int Pin_analogico;

  public:
    //Costruttore parametri pin dig e analog
    Infrarossi(int digitale, int analogico) {
      Ultima_Lettura = -1;          //valore di debug
      Pin_digitale = digitale;
      Pin_analogico = analogico;
      pinMode(digitale, INPUT);
      pinMode(analogico, INPUT);
    }

    //Funzione int ritorna ultimo valore
    int Get_Ultima_Lettura() {
      return Ultima_Lettura;
    }

    //Funzione int ritorna ultimo   (staccare bool?)
    int Leggi() {
      if (digitalRead(Pin_digitale)) {
        Ultima_Lettura = -1;
      }
      else if (!(digitalRead(Pin_digitale))) {
        Ultima_Lettura = analogRead(Pin_analogico);
      }
    }

    //Funzione bool ritorna true se il valore del IR è compreso tra min e max(exc) 
    bool Valore_Tra(int minimo, int massimo) {
      return (Ultima_Lettura >= minimo && Ultima_Lettura < massimo) ? true : false;
    }

    //Funzione bool ritorna true se il valore è inferiore al colore bianco 
    bool IsWhite() {
      return (Ultima_Lettura < BIANCO) ? true : false;
    }

    //Funzione bool ritorna true se il valore è superiore al colore nero
    bool IsBlack() {
      return (Ultima_Lettura > NERO) ? true : false;
    }

};

//I vari servo per i motori
Servo Ruota_Destra;
Servo Ruota_Sinistra;

/* Gli infrarossi ritornano valori tra 0 e 900, 900 è nero, -1 è solo un valore di debug
  per indicare che il sensore non è stato in grado di leggere nulla, se il pin digitale del sensore
  è zero allora il sensore è collegato, altrimenti non è collegato / non è detecato da arduino */
Infrarossi* InfraDestra;
Infrarossi* InfraAvanti;
Infrarossi* InfraSinistra;

void Stop() {
  Ruota_Sinistra.write(SERV_STOP);
  Ruota_Destra.write(SERV_STOP);
}

void Go_Avanti() {
  Ruota_Sinistra.write(SERV_SINIST_AV);
  Ruota_Destra.write(SERV_DEST_AV);
}

void Go_Dietro() {
  Ruota_Sinistra.write(SERV_SINIST_DIETR);
  Ruota_Destra.write(SERV_DEST_DIETR);
}

void Vai_Sinistra(int quanto) {
  int gradazione;
  Ruota_Destra.write(SERV_DEST_AV);
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
  Ruota_Sinistra.write(gradazione);
}


void Vai_Destra(int quanto) {
  int gradazione;
  Ruota_Sinistra.write(SERV_SINIST_AV);
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
  Ruota_Destra.write(gradazione);     //InfraDestra da 90 a 180
}

void FollowTheLine() {

  Valore_Destra = InfraDestra->Leggi();
  Valore_Sinistra = InfraSinistra->Leggi();
  Valore_Avanti = InfraAvanti->Leggi();

  Serial.println(InfraDestra->Leggi());
  Serial.println(InfraSinistra->Leggi());
  Serial.println(InfraAvanti->Leggi());

  In_Range_Destra = InfraDestra->Valore_Tra(BIANCO, NERO);
  In_Range_Avanti = InfraAvanti->Valore_Tra(BIANCO, NERO);
  In_Range_Sinistra = InfraSinistra->Valore_Tra(BIANCO, NERO);

  Bianco_Destra = InfraDestra->IsWhite();
  Bianco_Avanti = InfraAvanti->IsWhite();
  Bianco_Sinistra = InfraSinistra->IsWhite();

  Nera_Destra = InfraDestra->IsBlack();
  Nero_Avanti = InfraAvanti->IsBlack();
  Nero_Sinistra = InfraSinistra->IsBlack();

  if (Nera_Destra == HIGH && Nero_Sinistra == HIGH && Nero_Avanti == HIGH && STATO3 == 0)
  {
    Go_Avanti();
    delay(100);//100
    if (Nero_Avanti || Nero_Sinistra || Nera_Destra)
    {
      goto uscita;
    }
    else
    {
      //    Go_Dietro();
      if (STATO <= 5) STATO = 10;
      else if (STATO > 10) STATO = 5;

      STATO3 = 1;
    }

  }
uscita:
  if (Nera_Destra == LOW || Nero_Sinistra  == LOW || Nero_Avanti == LOW)
  {
    if (STATO3 == 0)
    {
      if (Nero_Avanti == HIGH && In_Range_Sinistra == HIGH && Bianco_Destra == HIGH)
      {
        STATO = 6;
        STATO2 = 0;
      }

      if (Nero_Avanti == HIGH && Nero_Sinistra == HIGH && Bianco_Destra == HIGH)
      {
        STATO = 7;
        STATO2 = 0;
      }

      if (In_Range_Avanti == HIGH && Nero_Sinistra == HIGH && Bianco_Destra == HIGH)
      {
        STATO = 8;
        STATO2 = 0;
      }

      if (Nero_Sinistra == HIGH && Bianco_Avanti == HIGH && Bianco_Destra == HIGH)
      {
        STATO = 9;
        STATO2 = 0;
      }

      if (Bianco_Destra == HIGH && Bianco_Avanti == HIGH && (Nero_Sinistra == HIGH || In_Range_Sinistra == HIGH))
      {
        STATO = 10;
        STATO2 = 2;
      }

      if (Nero_Avanti == HIGH && In_Range_Destra == HIGH && Bianco_Sinistra == HIGH)
      {
        STATO = 1;
      }

      if (Nero_Avanti == HIGH && Nera_Destra == HIGH && Bianco_Sinistra == HIGH)
      {
        STATO = 2;
        STATO2 = 0;
      }

      if (In_Range_Avanti == HIGH && Nera_Destra == HIGH && Bianco_Sinistra == HIGH)
      {
        STATO = 3;
        STATO2 = 0;
      }

      if (Nera_Destra == HIGH && Bianco_Avanti == HIGH && Bianco_Sinistra == HIGH)
      {
        STATO = 4;
        STATO2 = 0;
      }

      if (Bianco_Sinistra == HIGH && Bianco_Avanti == HIGH && (Nera_Destra == HIGH || In_Range_Destra == HIGH))
      {
        STATO = 5;

        STATO2 = 1;
      }

      if (Bianco_Sinistra == HIGH && Bianco_Avanti == HIGH && Bianco_Destra == HIGH)
      {
        if (STATO2 == 0)
        {
          STATO = 10;
        }
        if (STATO2 == 1)
        {
          STATO = 5;
        }

        if (STATO2 == 2)
        {
          STATO = 10;
        }
      }
      else
      {
        if (STATO == 5) {
          if (Nera_Destra == HIGH && Bianco_Sinistra == HIGH) {
            STATO3 = 0;
          }
        }

        else {
          if (Nero_Sinistra == HIGH && Bianco_Destra == HIGH) {
            STATO3 = 0;
          }
        }
      }

    }


    switch (STATO)
    {
      case 0: //VAI LEGGERMENTE A InfraDestra
        Vai_Destra(1);
        delay(100);
        break;
      case 2: //VAI A InfraDestra
        Vai_Destra(2);
        delay(100);
        break;
      case 3: //VAI A InfraDestra +
        Vai_Destra(3);
        break;
      case 4: //VAI A InfraDestra ++
        Vai_Destra(4);
        break;
      case 5: //VAI A InfraDestra +++
        Vai_Destra(5);
        break;
      case 6: //VAI LEGGERMENTE A SINISTRA
        Go_Avanti();
        //delay(100);
        break;
      case 7: //VAI A SINISTRA
        Vai_Sinistra(2);
        //delay(100);
        break;
      case 8: //VAI A SINISTRA+
        Vai_Sinistra(3);
        break;
      case 9: //VAI A SINISTRA++
        Vai_Sinistra(4);
        break;
      case 10: //VAI A SINISTRA ++++
        Vai_Sinistra(5);
        break;
      case 11:

        break;
    }

  }

}

void setup() {
  Ruota_Sinistra.attach(PIN_RUOTA_SINISTRA); //TODO: Controllare i pin dei servo
  Ruota_Destra.attach(PIN_RUOTA_DESTRA); //TODO: Controllare i pin dei servo
  InfraDestra = new Infrarossi(PIN_INFRA_DESTR_DIG, PIN_INFRA_DESTR_ANAG);
  InfraAvanti = new Infrarossi(PIN_INFRA_AV_DIG, PIN_INFRA_AV_ANAG);
  InfraSinistra = new Infrarossi(PIN_INFRA_SINIST_DIG, PIN_INFRA_SINIST_ANAG);
  Go_Avanti();
  Serial.begin(9600);
  delay(500);
}

void loop() {
  FollowTheLine();
  delay(10);
  /*
    Serial.print("Lettura di InfraDestra: ");
    Serial.print(Valore_Destra);
    Serial.print("\t\tLettura in avanti: ");
    Serial.print(Valore_Avanti);
    Serial.print("\t\tLettura di sinistra: ");
    Serial.print(Valore_Sinistra);
    /*Serial.print("\t\tSTATO3: ");
    Serial.print(STATO3);*/
  /*Serial.print("\t\tSTATO: ");
    Serial.println(STATO);*/


}
