
    #include <Servo.h>
    Servo leftWheel;                   
    Servo rightWheel;      
    #define wheelStopValue 90
    #define leftWheelFordwardValue 0
    #define leftWheelBackwardsValue 180
    #define rightWheelFordwardValue 180
    #define rightWheelBackwardsValue 0                                                                                                                      
    #define pinLeftWheel            8   /*   Left servo Weel          */
    #define pinRightWheel           9   /*   Right servo Weel         */

 #define LarghezzaCorsia 23.5
 #define LarghezzaRobot 11.2
 
//   #define pinSensorIRLeft         2     Left infrared sensor     */
//   #define pinSensorIRRight        3     Right infrared sensor    */
//    #define pinSensorLDRLeft       A2   /*   Left light sensor        */
//    #define pinSensorLDRRight      A3   /*   Right light sensor       */
    #define pinUSTri                4   /*   Ultrasound trigger       */
    #define pinUSEch                5   /*   Ultrasound echo          */
//    #define pinHead                11   /*   Ultrasound servo         */
//    #define pinBuzzer              12   /*   Boozer                   */
//    #define pinsensoredestra       7
//    #define pinsensorefronte       5
//    #define pinsensoresinistra     4  

         
    enum
    {
      INIZIALIZZAZIONE = 0,
      LETTURA = 1,
      ATTESA = 2
    };
    enum {
      GIRA_DESTRA = 0,
      GIRA_SINISTRA = 1,
      VAI_DRITTO = 2
    };
    
    class Sensore_suono {
     public:
      int Trigger;
      int Echo;
      long TEMPO_PRECEDENTE;
      long TEMPO_PRECEDENTE_2;
      int STATO;
      long TEMPO_TRASCORSO;
      float ULTIMA_DISTANZA; 
      void leggi() {
            digitalWrite(Trigger, LOW);        //CONTROLLARE SE FUNZIONA
            delayMicroseconds(2); 
            digitalWrite(Trigger, HIGH);
            delayMicroseconds(10); 
            digitalWrite(Trigger, LOW);
     float  duration = pulseIn(Echo, HIGH);
     float  distance = (duration*.0343)/2;
            ULTIMA_DISTANZA = distance;
      }
    };

   
     void stopWheels() {
      leftWheel.write(wheelStopValue);
      rightWheel.write(wheelStopValue);
    }
    
    void goForwards() 
    {
      leftWheel.write(leftWheelFordwardValue);
      rightWheel.write(rightWheelFordwardValue);
      
    }

     void goLeft() {
      leftWheel.write(wheelStopValue);
      rightWheel.write(rightWheelFordwardValue);
    }
    
    void goRight() {
      leftWheel.write(leftWheelFordwardValue);
      rightWheel.write(wheelStopValue);
    }
    
    
    void MISURAZIONI_SENSORI (Sensore_suono x)
    {
      switch (x.STATO)
      {
      case INIZIALIZZAZIONE:
      
          digitalWrite(x.Trigger,HIGH);
          delayMicroseconds(15);
          digitalWrite(x.Trigger,LOW);     
          x.STATO = LETTURA; 
          x.TEMPO_PRECEDENTE = micros();
          break;
     
      case LETTURA:
        
          if(digitalRead(x.Echo) == LOW)
            {
    
              
              x.TEMPO_PRECEDENTE_2 = micros();
              x.TEMPO_TRASCORSO = (x.TEMPO_PRECEDENTE_2-x.TEMPO_PRECEDENTE)/1000;
              x.ULTIMA_DISTANZA = x.TEMPO_TRASCORSO/58;
              x.STATO = ATTESA;
            }
            break;
      case ATTESA:
            if((micros() - x.TEMPO_PRECEDENTE_2) >= 58000)
              {
                  x.STATO = INIZIALIZZAZIONE;
              }
              break;
      }
    }
    Sensore_suono* DESTRAo = new Sensore_suono();
    Sensore_suono* FRONTEo = new Sensore_suono();
    Sensore_suono* SINISTRAo = new Sensore_suono();
    Sensore_suono DESTRA = *DESTRAo;
    Sensore_suono FRONTE = *FRONTEo;
    Sensore_suono SINISTRA = *SINISTRAo;
     
    float Limite;
    void setup() 
    {
      //associazione OGGETTO.sensori numero del pin
     DESTRA.Trigger = 3;
     DESTRA.Echo = 6;
     FRONTE.Trigger = 12;
     FRONTE.Echo = 13;
     SINISTRA.Trigger = 10;
     SINISTRA.Echo = 11;
     
     SINISTRA.STATO = INIZIALIZZAZIONE;
     FRONTE.STATO = INIZIALIZZAZIONE;
     DESTRA.STATO = INIZIALIZZAZIONE;
     leftWheel.attach(8);
     rightWheel.attach(9);
    Serial.begin(9600);
    pinMode(DESTRA.Trigger, OUTPUT);
    pinMode(DESTRA.Echo, INPUT);
    pinMode(SINISTRA.Trigger, OUTPUT);
    pinMode(SINISTRA.Echo, INPUT);
    pinMode(FRONTE.Trigger, OUTPUT);
    pinMode(FRONTE.Echo, INPUT);
     Limite=(LarghezzaCorsia-LarghezzaRobot)/4;
    }
    //int STATO = 2;
    
    void loop() 
    {
       DESTRA.leggi();
       FRONTE.leggi();
       SINISTRA.leggi();

      /*if (SINISTRA.ULTIMA_DISTANZA>80) SINISTRA.ULTIMA_DISTANZA=80;
      if (DESTRA.ULTIMA_DISTANZA>80) DESTRA.ULTIMA_DISTANZA=80;*/
    
      Serial.print("SX: ");
      Serial.print(SINISTRA.ULTIMA_DISTANZA);
      Serial.print("\tCT: ");        
      Serial.print(FRONTE.ULTIMA_DISTANZA);
      Serial.print("\tDX: ");
      Serial.println(DESTRA.ULTIMA_DISTANZA);   
      
       
     


    
/*VAFf_DRITTO:*/
     if(FRONTE.ULTIMA_DISTANZA > Limite && DESTRA.ULTIMA_DISTANZA > Limite && SINISTRA.ULTIMA_DISTANZA > Limite )
       {
         goForwards();
         delay(150);
        }
       else if((FRONTE.ULTIMA_DISTANZA <= Limite && SINISTRA.ULTIMA_DISTANZA <DESTRA.ULTIMA_DISTANZA) || (SINISTRA.ULTIMA_DISTANZA <= Limite && FRONTE.ULTIMA_DISTANZA >Limite) ||(FRONTE.ULTIMA_DISTANZA > Limite && DESTRA.ULTIMA_DISTANZA >Limite && SINISTRA.ULTIMA_DISTANZA> DESTRA.ULTIMA_DISTANZA))
       {
       //STATO = GIRA_DESTRA;
       goRight();
       delay(150);
       goForwards();
       delay(20);
       stopWheels();
        }
        
        else if((FRONTE.ULTIMA_DISTANZA <= Limite && SINISTRA.ULTIMA_DISTANZA >DESTRA.ULTIMA_DISTANZA) || (DESTRA.ULTIMA_DISTANZA <= Limite && FRONTE.ULTIMA_DISTANZA >Limite) || (FRONTE.ULTIMA_DISTANZA>Limite && SINISTRA.ULTIMA_DISTANZA> Limite && DESTRA.ULTIMA_DISTANZA> SINISTRA.ULTIMA_DISTANZA ))
             {       
             //STATO = GIRA_SINISTRA;
              goLeft();
              delay(200);
              goForwards();
              delay(20);
              stopWheels();
             }
             
         }
   
    
