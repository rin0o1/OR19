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

  FUNZIONAMENTO IR VECCHIO
  Gli infrarossi ritornano valori tra 0 e 900, 900 è nero, -1 è solo un valore di debug
  per indicare che il sensore non è _iState in grado di leggere nulla, se il pin digitale del sensore
  è zero allora il sensore è collegato, altrimenti non è collegato / non è detecato da arduino

  FUNZIONAMENTO ALGORITMO
  Di seguito indicherò i sensori con sx, top e dx per sinistra, avanti e destra. Con sensori ON intendo la rilevazione del colore nero.
  Robot parte in posizione neutra con i sensori sulla linea nera, di conseguenza tutti attivati. Deve andare avanti proseguendo e avendo prevalentemente il sensore al mezzo sul nero
  e gli altri sul bianco. Per la curvatura verso il sensore opposto ON, modulare l'angolo con l'impiego della cache per verificare da quanto sta uscendo fuori
  per evitare robot a zig-zag.

  Riguardare logica curve a 90 gradi.
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

#pragma region PIN

//Ruote
#define PIN_RUOTA_SINISTRA  10
#define PIN_RUOTA_DESTRA  9

//Infrarossi
#define PIN_INFRA_DESTR_DIG  2
#define PIN_INFRA_AV_DIG 0
#define PIN_INFRA_SINIST_DIG 1

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

//enumeratore di stato indicante l'orientamento teorico della linea rispetto al robot
enum eState{ osx, xsx, sx, top, dx, xdx, odx };     //out sinistra, extreme sinistra, sinistra, avanti, destra, extreme destra, out destra

//Cache stati precedenti (gioco di tempo/impatto per curve anomale)
eState _eState;
eState _eCache;   //verificare necessarietà

//Bool Nero colore
bool _bNeroDestra;
bool _bNeroAvanti;
bool _bNeroSinistra;

//classe componente infrarossi casereccia
class Infrarossi {

  private:
    int _iPinDig;

  public:
    //Costruttore parametri pin analog
    Infrarossi(int _digitale) {
      _iPinDig = _digitale;
      pinMode(_iPinDig, INPUT);
    }

    //Funzione bool ritorna true se il sensore vede nero
    bool IsBlack() {
      return digitalRead(_iPinDig);
    }

};

//I vari servo per i motori
Servo Ruota_Destra;
Servo Ruota_Sinistra;

//dichiarazione oggetti infrarossi
Infrarossi* InfraDestra;
Infrarossi* InfraAvanti;
Infrarossi* InfraSinistra;

//Procedura ferma entrambe le ruote
void Stop() {

  Ruota_Sinistra.write(SERV_STOP);
  Ruota_Destra.write(SERV_STOP);
}

//Procedura avanti con entrambe le ruote
void Go_Avanti() {

  Ruota_Sinistra.write(SERV_SINIST_AV);
  Ruota_Destra.write(SERV_DEST_AV);
}

//Procedura retromarcia con entrambe le ruote
void Go_Dietro() {

  Ruota_Sinistra.write(SERV_SINIST_DIETR);
  Ruota_Destra.write(SERV_DEST_DIETR);
}

//Procedura setta e avvia svolta a sx a seconda della proriotà data (xsx o sx)
void Vai_Sinistra(eState _incisione) {

  int _iGradazione;
  Ruota_Destra.write(SERV_DEST_AV);
  switch (_incisione) {
    case osx:
      _iGradazione = 70;      //cammina-, curva+
      break;

    case xsx:
      _iGradazione = 50;
      break;

    case sx:
      _iGradazione = 10;      //cammina+, curva-
      break;
  }

  Ruota_Sinistra.write(_iGradazione);
}

//Procedura setta e avvia svolta a dx a seconda della priorità data (xdx o dx)
void Vai_Destra(eState _incisione) {

  int _iGradazione;
  Ruota_Sinistra.write(SERV_SINIST_AV);
  switch (_incisione) {
    case odx:
      _iGradazione = 110;     //cammina-, curva+
      break;

    case xdx:
      _iGradazione = 130;
      break;

    case dx:
      _iGradazione = 170;     //cammina+, curva-
      break;
  }

  Ruota_Destra.write(_iGradazione);
}

//Procedura principale in loop
void FollowTheLine() {

  _bNeroDestra = InfraDestra->IsBlack();
  _bNeroAvanti = InfraAvanti->IsBlack();
  _bNeroSinistra = InfraSinistra->IsBlack();

  if ((_bNeroSinistra  && _bNeroAvanti && _bNeroDestra) || (!_bNeroSinistra && _bNeroAvanti && !_bNeroDestra))
    _eState = top;
  else if(!_bNeroSinistra && !_bNeroAvanti && !_bNeroDestra)
      _eState = (_eCache == xsx || _eCache == sx) ? osx : odx;
  else {

    if(_bNeroSinistra && !_bNeroAvanti && !_bNeroDestra)
      _eState = xsx;     //andare a sinistra (mod+)
    else if(_bNeroSinistra && _bNeroAvanti && !_bNeroDestra)
      _eState = sx;     //andare a sinistra (mod)
    else if(!_bNeroSinistra && _bNeroAvanti && _bNeroDestra)
      _eState = dx;     //andare a sinistra (mod)
    else if(!_bNeroSinistra && !_bNeroAvanti && _bNeroDestra)
      _eState = xdx;     //andare a sinistra (mod+)
  }

  //switch gestore del moto del robot
  switch (_eState) {
    case osx:
      Vai_Sinistra(_eState);      //mod++
      break;

    case xsx:
      Vai_Sinistra(_eState);     //mod+
      break;

    case sx:
      Vai_Sinistra(_eState);     //mod
      break;

    case top:
      Go_Avanti();    //vai avanti con sensori captanti nero ovunque o solo davanti
      break;

    case dx:
      Vai_Destra(_eState);     //mod
      break;
    
    case xdx:
      Vai_Destra(_eState);     //mod+
      break;

    case odx:
      Vai_Destra(_eState);      //mod++
      break;
  }

  _eCache = _eState;    //verificare necessarietà
}

void setup() {
  Serial.begin(9600);
  Ruota_Sinistra.attach(PIN_RUOTA_SINISTRA);
  Ruota_Destra.attach(PIN_RUOTA_DESTRA);
  InfraDestra = new Infrarossi(PIN_INFRA_DESTR_DIG);
  InfraAvanti = new Infrarossi(PIN_INFRA_AV_DIG);
  InfraSinistra = new Infrarossi(PIN_INFRA_SINIST_DIG);
  delay(500);
}

void loop() {
  FollowTheLine();
  delay(10);
  //debug seriale
  Serial.println("Dx -> " + InfraDestra->IsBlack());
  Serial.println("Centro -> " + InfraAvanti->IsBlack());
  Serial.println("Sx -> " + InfraSinistra->IsBlack());
  delay(1000);
}
