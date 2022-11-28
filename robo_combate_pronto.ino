#define LF 3 //motor esquerdo para frente
#define LB 7 //motor esquerdo para traz
#define RF 9 //motor direito para frente
#define RB 8 //motor direito para traz
#define PWML 5 //pwm  esquerda
#define PWMR 6 //pwm  direita
#define MIN_PULSE_LENGTH 1000 // Minimum pulse length in µs
#define MAX_PULSE_LENGTH 2000 // Maximum pulse length in µs
#include <Servo.h> //biblioteca do ESC
Servo ESC; //define o ESC

int ch1Value; // é para frente/traz
int ch2Value; // controla a direção
int ch3Value; //botao esquerdo (inverter controle)
int ch4Value; //botao direito (ligar/desligar arma)
int maxTrig = 505; // max frente traz
int minTrig = -517; // min frente traz
int maxWheel = 472; // max esquerda direita
int minWheel = -456; // min esquerda direita
int minDeadPoint = -45; // min ponto morto
int maxDeadPoint = 45; // max ponto morto
const int maxWeaponSpeed = 1250;
int Weapon = MIN_PULSE_LENGTH; 
unsigned long timeAntes = 0;// marca o tempo da rampa de aceleração da arma
unsigned long time2antes = 0;// marca o tempo de receber o sinal
int armaOff;
int reverseOff;


void setup() {
  Serial.begin (9600) ;
  pinMode(LF, OUTPUT);
  pinMode(LB, OUTPUT);
  pinMode(RF, OUTPUT);
  pinMode(RB, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(10, OUTPUT);
  ESC.attach(10,MIN_PULSE_LENGTH, MAX_PULSE_LENGTH); // (pin, min pulse width, max pulse width in microseconds)
  //armaOff = ch4Value; //
}

void loop() {
  ch1Value = pulseIn(A4, HIGH, 100000) - 1505; //frente/traz
  ch2Value = pulseIn(A3, HIGH, 100000) - 1483; //esquerda direita
  ch3Value = pulseIn(A0, HIGH, 100000); // botão inverter
  ch4Value = pulseIn(A1, HIGH, 100000); // botão arma
  
  if(ch4Value!=0 && armaOff == 0) armaOff = ch4Value; //A arma sempre começa desligada, o estado do switch ch4 encontrado ao iniciar o código é considerado como o estado desligado
  if(ch3Value!=0 && reverseOff == 0) reverseOff = ch3Value;
  //esse if só retornará true uma única vez, na condição que armaOff não tenha sido definida ainda e que o controle esteja ligado
  //Serial.print("ch1Value: ");Serial.println(ch1Value);
  //Serial.print("ch2Value: ");Serial.println(ch2Value);
  //Serial.print("ch3Value: ");Serial.println(ch3Value);
  //Serial.print("ch4Value: ");Serial.println(ch4Value);
  //int pct = map(Weapon,MIN_PULSE_LENGTH,MAX_PULSE_LENGTH, 0, 100);
  
  //Serial.print("Weapon: ");Serial.println(Weapon); //Serial.println("%");
   
  deadZone (ch1Value, ch2Value); // função ponto morto

  if ((ch4Value >1000) and (ch3Value >1000)and(ch1Value >-1505) and (ch2Value >-1483)) {
    Controler (ch1Value, ch2Value); // função para ajustar o controle
    weaponFunc(); // função para ajustar a arma
  }
  else {
    failSafe(); // fail safe
  }
  //delay(10);

}

void deadZone(int ch1Arg, int ch2Arg) {
  if ((ch1Arg > minDeadPoint) and (maxDeadPoint > ch1Arg) and (ch2Arg > minDeadPoint) and (maxDeadPoint > ch2Arg)) {
    digitalWrite(LF, LOW); digitalWrite(LB, LOW);
    digitalWrite(RF, LOW); digitalWrite(RB, LOW);
  }
}

void Controler(int ch1Arg, int ch2Arg) {
  
  int triggerSpeed = 0; // variável para frente/traz
  int turnIntensity = 0; //variável para esquerda direita

  if (isUpsidedown()) {
    ch1Arg = -1 * ch1Arg;
    ch2Arg = -1 * ch2Arg;
  }
  if (ch1Arg > maxDeadPoint) {  // se o carro estiver indo para frente:
    triggerSpeed = map(ch1Arg, maxTrig, maxDeadPoint, 255, 0);
    if (triggerSpeed >= 255) {
      triggerSpeed = 255;
    }
    if (triggerSpeed <= 0) {
      triggerSpeed = 0;
    }
    

    if (ch2Arg > maxDeadPoint) {  // se é para virar para direita
      if (!isUpsidedown()) {
        turnRight(ch2Arg, triggerSpeed);
      }
      else {
        turnLeft(ch2Arg, triggerSpeed);
      }
    }

    else if (ch2Arg < minDeadPoint) {  // se é para virar para esquerda
      if (!isUpsidedown()) {
        Serial.print("INDO P ESQUERDA");
        turnLeft(ch2Arg, triggerSpeed);
      }
      else {
        turnRight(ch2Arg, triggerSpeed);
      }
    }
    else {
      analogWrite(PWML, triggerSpeed);
      analogWrite(PWMR, triggerSpeed);
    }
    digitalWrite(LB, LOW); digitalWrite(LF, HIGH);
    digitalWrite(RB, LOW); digitalWrite(RF, HIGH);
  }
  else if (ch1Arg < minDeadPoint) { // se o carro estiver indo para traz:
    triggerSpeed = map(ch1Arg, minDeadPoint, minTrig, 0, 255);
    if (triggerSpeed >= 255) {
      triggerSpeed = 255;
    }
    if (triggerSpeed <= 0) {
      triggerSpeed = 0;
    }
    if (ch2Arg > maxDeadPoint) {
      if (!isUpsidedown()) {
        turnRight(ch2Arg, triggerSpeed);
      }
      else {
        turnLeft(ch2Arg, triggerSpeed);
      }
    }
    else if (ch2Arg < minDeadPoint) {
      if (!isUpsidedown()) {
        turnLeft(ch2Arg, triggerSpeed);
        
      }
      else {
        turnRight(ch2Arg, triggerSpeed);
      }
    }
    else {
      analogWrite(PWML, triggerSpeed);
      analogWrite(PWMR, triggerSpeed);
    }
    digitalWrite(LB, HIGH); digitalWrite(LF, LOW);
    digitalWrite(RB, HIGH); digitalWrite(RF, LOW);
  }

  else if ((ch1Arg >= minDeadPoint) and (ch1Arg <= maxDeadPoint)) {   // caso o carrinho não esta indo para frente ou traz, abilite o modo gira hehe
    //Serial.println("Spinning!");
    spin(ch2Arg, turnIntensity);
  }
}
boolean isUpsidedown() { // verifica se está de virado
  if (abs(ch3Value-reverseOff) > 30) {
    return true;
  }
  else {
    return false;
  }
}
void turnRight(int ch2Arg, int triggerSpeed)
{
  int slowerSpeed = map(ch2Arg, maxDeadPoint, maxWheel, triggerSpeed, 60);
  if (slowerSpeed >= 255) {
    slowerSpeed = 255;
  }
  if (slowerSpeed <= 0) {
    slowerSpeed = 0;
  }
  analogWrite(PWMR, slowerSpeed);  // essa roda gira numa intensidade mais lenta
  analogWrite(PWML, triggerSpeed);
}
void turnLeft(int ch2Arg, int triggerSpeed)
{
  int slowerSpeed = map(ch2Arg, minDeadPoint, minWheel, triggerSpeed, 60);
  if (slowerSpeed >= 255) {
    slowerSpeed = 255;
  }
  if (slowerSpeed <= 0) {
    slowerSpeed = 0;
  }
  analogWrite(PWML, slowerSpeed);  // essa roda gira numa intensidade mais lenta
  analogWrite(PWMR, triggerSpeed);
}
void spin(int ch2Arg, int turnIntensity) {
  if (ch2Arg < minDeadPoint) {
    turnIntensity = map(ch2Arg, minWheel, minDeadPoint, 255, 0);
    if (turnIntensity >= 255) {
      turnIntensity = 255;
    }
    if (turnIntensity <= 0) {
      turnIntensity = 0;
    }
    analogWrite(PWMR, turnIntensity);
    analogWrite(PWML, turnIntensity);
    digitalWrite(LB, LOW);
    digitalWrite(LF, HIGH);
    digitalWrite(RB, HIGH);
    digitalWrite(RF, LOW);
  }
  else if (ch2Arg > maxDeadPoint) {
    turnIntensity = map(ch2Arg, maxWheel, maxDeadPoint, 255, 0);
    if (turnIntensity >= 255) {
      turnIntensity = 255;
    }
    if (turnIntensity <= 0) {
      turnIntensity = 0;
    }
    analogWrite(PWMR, turnIntensity);
    analogWrite(PWML, turnIntensity);
    digitalWrite(LB, HIGH);
    digitalWrite(LF, LOW);
    digitalWrite(RB, LOW);
    digitalWrite(RF, HIGH);
  }
}
void failSafe() {
  digitalWrite(LB, LOW);
  digitalWrite(LF, LOW);
  digitalWrite(RB, LOW);
  digitalWrite(RF, LOW);
  Weapon = MIN_PULSE_LENGTH;
    ESC.writeMicroseconds(Weapon);
}
void weaponFunc() {

  if (isWeaponOn()) { 
    if (Weapon <= maxWeaponSpeed){
      if (millis() - timeAntes > 4){
        timeAntes = millis();
        Weapon+=8;
        ESC.writeMicroseconds(Weapon);
      }
    }
  }
  else {
    Weapon = MIN_PULSE_LENGTH;
    ESC.writeMicroseconds(Weapon);
  }
}
boolean isWeaponOn()
{
  if( abs(ch4Value-armaOff) < 30) return false; //o switch ch4 está no modo desligado, como o sinal ch4 tem pequenas variações, coloquei uma margem 
  return true;
  }
