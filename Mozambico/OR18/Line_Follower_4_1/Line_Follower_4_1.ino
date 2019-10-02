
    //LINE FOLLOWER
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
    #define NERO 300
    #define BIANCO 80
    
    // Direzioni Servo
    #define Fermo 90
    #define Sinistra_Avanti 0
    #define Sinistra_Dietro 180
    #define Destra_Avanti 180
    #define Destra_Dietro 0
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
    bool Nero_Destra;
    bool Nero_Avanti;
    bool Nero_Sinistra;
    
    class Infrarossi {
      public:
        int Ultima_Lettura;
        int Pin_digitale;
        int Pin_analogico;
        Infrarossi(int digitale, int analogico) {
          Ultima_Lettura = -1;
          Pin_digitale = digitale;
          Pin_analogico = analogico;
          pinMode(digitale, INPUT);
          pinMode(analogico, INPUT);
        }
        int Get_Ultima_Lettura() {
          return Ultima_Lettura;
        }
        int Leggi() {
          if (digitalRead(Pin_digitale)) {
            Ultima_Lettura = -1;
          }
          else if (!(digitalRead(Pin_digitale))) {
            Ultima_Lettura = analogRead(Pin_analogico);
          }
        }
        bool Valore_Tra(int minimo, int massimo) {
          if (Ultima_Lettura > minimo && Ultima_Lettura < massimo) {
            return true;
          }
          else return false;
        }
        bool IsWhite() {
          if (Ultima_Lettura < BIANCO) return true;
          else return false;
        }
        bool IsBlack() {
          if (Ultima_Lettura > NERO) return true;
          else return false;
        }
    };
    
    /* I vari servo per i motori */
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
      //  gradazione = 95;
      //gradazione -= 90;
      //gradazione = gradazione - 10;
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
      //  gradazione = 85;
      //gradazione = gradazione + 10;
      Ruota_Destra.write(gradazione); /* Destra da 90 a 180 */
    }
    
    /* Gli infrarossi ritornano valori tra 0 e 900, 900 è nero, -1 è solo un valore di debug
      per indicare che il sensore non è stato in grado di leggere nulla, se il pin digitale del sensore
      è zero allora il sensore è collegato, altrimenti non è collegato / non è detecato da arduino */
    Infrarossi* Destra;
    Infrarossi* Avanti;
    Infrarossi* Sinistra;
    
    void setup() {
      // put your setup code here, to run once:
      /*Serial.begin(9600);*/
      Ruota_Sinistra.attach(8); //TODO: Controllare i pin dei servo
      Ruota_Destra.attach(9); //TODO: Controllare i pin dei servo
      Destra = new Infrarossi(7, A1);
      Avanti = new Infrarossi(5, A2);
      Sinistra = new Infrarossi(3, A3);
      goForwards();
      Serial.begin(9600);
      delay(500);
    }
    
    void loop() {
      followTheLine();
      delay(10);
      /*
        Serial.print("Lettura di destra: ");
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
    
    
    void followTheLine() {
    
      Valore_Destra = Destra->Leggi();
      Valore_Sinistra = Sinistra->Leggi();
      Valore_Avanti = Avanti->Leggi();
    
      Serial.println (Destra->Leggi());
      Serial.println(Sinistra->Leggi());
      Serial.println(Avanti->Leggi());
    
      In_Range_Destra = Destra->Valore_Tra(BIANCO, NERO);
      In_Range_Avanti = Avanti->Valore_Tra(BIANCO, NERO);
      In_Range_Sinistra = Sinistra->Valore_Tra(BIANCO, NERO);
    
      Bianco_Destra = Destra->IsWhite();
      Bianco_Avanti = Avanti->IsWhite();
      Bianco_Sinistra = Sinistra->IsWhite();
    
      Nero_Destra = Destra->IsBlack();
      Nero_Avanti = Avanti->IsBlack();
      Nero_Sinistra = Sinistra->IsBlack();
    
      if (Nero_Destra == HIGH && Nero_Sinistra == HIGH && Nero_Avanti == HIGH && STATO3 == 0)
      {
        goForwards();
        delay(100);//100
        if (Nero_Avanti || Nero_Sinistra || Nero_Destra)
        {
          goto uscita;
        }
        else
        {
          //    goBackwards();
          if (STATO <= 5) STATO = 10;
          else if (STATO > 10) STATO = 5;
    
          STATO3 = 1;
        }
    
      }
    uscita:
      if (Nero_Destra == LOW || Nero_Sinistra  == LOW || Nero_Avanti == LOW)
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
    
          if (Nero_Avanti == HIGH && Nero_Destra == HIGH && Bianco_Sinistra == HIGH)
          {
            STATO = 2;
            STATO2 = 0;
          }
    
          if (In_Range_Avanti == HIGH && Nero_Destra == HIGH && Bianco_Sinistra == HIGH)
          {
            STATO = 3;
            STATO2 = 0;
          }
    
          if (Nero_Destra == HIGH && Bianco_Avanti == HIGH && Bianco_Sinistra == HIGH)
          {
            STATO = 4;
            STATO2 = 0;
          }
    
          if (Bianco_Sinistra == HIGH && Bianco_Avanti == HIGH && (Nero_Destra == HIGH || In_Range_Destra == HIGH))
          {
            STATO = 5;
    
            STATO2 = 1;
          }
    
          if (Bianco_Sinistra == HIGH && Bianco_Avanti == HIGH && Bianco_Destra == HIGH)
          {
          if(STATO2 == 0)
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
            if (Nero_Destra == HIGH && Bianco_Sinistra == HIGH) {
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
        case 0: //VAI LEGGERMENTE A DESTRA
          VAI_DESTRA(1);
          delay(100);
          break;
        case 2: //VAI A DESTRA
          VAI_DESTRA(2);
          delay(100);
          break;
        case 3: //VAI A DESTRA +
          VAI_DESTRA(3);
          break;
        case 4: //VAI A DESTRA ++
          VAI_DESTRA(4);
          break;
        case 5: //VAI A DESTRA +++
          VAI_DESTRA(5);
          break;
        case 6: //VAI LEGGERMENTE A SINISTRA
          goForwards();
          //delay(100);
          break;
        case 7: //VAI A SINISTRA
          VAI_SINISTRA(2);
          //delay(100);
          break;
        case 8: //VAI A SINISTRA+
          VAI_SINISTRA(3);
          break;
        case 9: //VAI A SINISTRA++
          VAI_SINISTRA(4);
          break;
        case 10: //VAI A SINISTRA ++++
          VAI_SINISTRA(5);
          break;
        case 11:
    
          break;
      }
      /*switch (STATO)
        {
    
        case 0: //2 SENSORI NERI A DESTRA... VAI A DESTRA
        if(In_Range_Destra)
        {
        STATO = 5;
        break;
        }
        if(In_Range_Avanti)
        {
        STATO = 6;
        break;
        }
    
        if(Bianco_Avanti)
        {
        STATO = 7;
        break;
        }
        VAI_DESTRA(2);
        break;
    
        case 1: //SENSORE CENTRALE NERO E SINNISTRO A META.. VAI LEGGERMENTE A SINISTRA
        if(Nero_Sinistra)
        {
        STATO = 2;
        break;
        }
        if(In_Range_Destra)
        {
        STATO = 5;
        break;
        }
        VAI_SINISTRA(1);
        break;
    
    
        case 2: // 2 SENSORE SINISTRI NERI.. VAI A SINISTRA
        if(In_Range_Sinistra)
        {
        STATO = 1;
        break;
        }
    
        if(In_Range_Avanti)
        {
        STATO = 3;
        break;
        }
        VAI_SINISTRA(2);
        break;
    
        case 3:  //SENSORE SINISTRA NERO E CENTRO A META.. VAI A SINISTRA +
        if(Nero_Avanti)
        {
        STATO = 2;
        break;
        }
    
        if(Bianco_Avanti)
        {
        STATO = 4;
        break;
        }
        VAI_SINISTRA(3);
        break;
    
        case 4: //SENSORE SINISTRA NERO E CNETRO BIANCO.. VAI A SINISTRA ++
        if(In_Range_Avanti)
        {
        STATO = 3;
        break;
        }
    
        if(In_Range_Sinistra)
        {
        STATO = 9;
        break;
        }
        VAI_SINISTRA(4);
        break;
    
    
        case 5: //SENSORE DESTRO META E CENTRO NERO.. VAI LEGGERMENTE A DESTRA
        if(In_Range_Sinistra)
        {
        STATO = 1;
        break;
        }
    
        if(Nero_Destra)
        {
        STATO = 0;
        break;
        }
        VAI_DESTRA(1);
        break;
    
    
        case 6: //SESNORE DESTRO NERO E CENTRO META.. VAI DESTRA +
        if(Nero_Avanti)
        {
        STATO = 0;
        break;
        }
    
        if(Bianco_Avanti)
        {
        STATO = 7;
        break;
        }
        VAI_DESTRA(3);
        break;
    
    
        case 7: //SENSORE DESTRO NERO E CENTRO BIANCO.. VAI DESTRA ++
        if(In_Range_Avanti)
        {
        STATO = 6;
        break;
        }
        if(In_Range_Destra)
        {
        STATO = 8;
        break;
        }
        VAI_DESTRA(4);
        break;
    
    
        case 8: //SENSORE DESTRO META E CENTRO BIANCO.. FERMO E DESTRA
        if(Nero_Destra)
        {
        STATO = 7;
        break;
        }
        if(Bianco_Destra)
        {
        VAI_DESTRA(5);
        break;
        }
        VAI_DESTRA(4);
        break;
    
    
        case 9: // SENSORE SINISTRO META E BIANCO CENTRO.. FERMO E SINISTRA
        if(In_Range_Sinistra)
        {
        STATO = 4;
        break;
        }
        if(Bianco_Sinistra)
        {
        VAI_SINISTRA(5);
        break;
        }
        VAI_SINISTRA(4);
        break;
    
        }*/
    }
    }

