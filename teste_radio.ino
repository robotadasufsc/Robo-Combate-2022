// veja https://ryanboland.com/blog/reading-rc-receiver-values
#define IN1 6
#define IN2 7
#define IN3 4
#define IN4 5
#define enA 8
#define enB 3
#define DOWN 10
#define CH3 8

int ch1_value; // é para frente/traz
int ch2_value; // controla a direção
int up_value;
int Weapon; //arma
////////////////////////////////
//valores do controle.. depende de como esta ajustado.
int maxTrig = 505; // é para frente/traz
int minTrig = -517; // controla a direção
int maxWheel = 472;
int minWheel = -456;
int minDeadPoint = -30;
int maxDeadPoint = 30;
//////////////////////////////////
void setup() {
  Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(DOWN, INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(CH3, INPUT);
}

void loop() {
  ch1_value = pulseIn(A0, HIGH) - 1505;
  ch2_value = pulseIn(A1, HIGH) - 1483;
  up_value = digitalRead(DOWN);
  Weapon = digitalRead (CH3);
  //Serial.print("3: "); Serial.println(pulseIn(A2, HIGH));
  //Serial.print("4: "); Serial.println(pulseIn(A3, HIGH)); Serial.println("");

  deadZoneCh1(ch1_value, ch2_value);
  if (pulseIn(A0, HIGH) > 700) {
    triggersResponses(ch1_value, ch2_value);
  }
  else {
    failSafe();
  }
}

void failSafe() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

void deadZoneCh1(int ch1_arg, int ch2_arg) {
  if ((ch1_arg > minDeadPoint) and (maxDeadPoint > ch1_arg) and (ch2_arg > minDeadPoint) and (maxDeadPoint > ch2_arg)) {
    digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
    //Serial.println("parado");


  }
}


void triggersResponses(int ch1_arg, int ch2_arg) {

  int trigger_speed = 0;
  int turn_intensity = 0; //definindo as variaveis
  if (isUpsidedown())
  {
    ch1_arg = -1 * ch1_arg;
    ch2_arg = -1 * ch2_arg;
  }
  Serial.print("1: "); Serial.println(ch1_arg);
  Serial.print("2: "); Serial.println(ch2_arg);
  Serial.print("up_value: "); Serial.println(up_value);


  if (ch1_arg > maxDeadPoint) {     ///// se o carro estiver indo para frente:
    trigger_speed = map(ch1_arg, maxTrig, maxDeadPoint, 255, 0);
    //Serial.print("trigger_speed: "); Serial.println(trigger_speed);
    if (trigger_speed >= 255) trigger_speed = 255; if (trigger_speed <= 0) trigger_speed = 0;
    if (ch2_arg > maxDeadPoint) {  // se é para virar para direita
      if (!isUpsidedown())
      {
        turnRight(ch2_arg, trigger_speed);
      }
      else
      {
        turnLeft(ch2_arg, trigger_speed);
      }
    }
    else if (ch2_arg < minDeadPoint) {  // se é para virar para esquerda
      if (!isUpsidedown())
      {
        turnLeft(ch2_arg, trigger_speed);
      }
      else
      {
        turnRight(ch2_arg, trigger_speed);
      }
    }
    else {
      analogWrite(enB, trigger_speed);
      analogWrite(enA, trigger_speed);
      //Serial.print("hi");
    }
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  }
  else if (ch1_arg < minDeadPoint) { ///// se o carro estiver indo para traz:
    trigger_speed = map(ch1_arg, minDeadPoint, minTrig, 0, 255);
    if (trigger_speed >= 255) trigger_speed = 255; if (trigger_speed <= 0) trigger_speed = 0;
    //Serial.print("trigger_speed: "); Serial.println(trigger_speed);
    if (ch2_arg > maxDeadPoint) {
      if (!isUpsidedown())
      {
        turnRight(ch2_arg, trigger_speed);
      }
      else
      {
        turnLeft(ch2_arg, trigger_speed);
      }
    }
    else if (ch2_arg < minDeadPoint) {
      if (!isUpsidedown())
      {
        turnLeft(ch2_arg, trigger_speed);
      }
      else
      {
        turnRight(ch2_arg, trigger_speed);
      }

    }
    else {
      analogWrite(enB, trigger_speed);
      analogWrite(enA, trigger_speed);
      // Serial.print("hi");
    }
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
    //Serial.println(trigger_speed);
  }

  else if ((ch1_arg >= minDeadPoint) and (ch1_arg <= maxDeadPoint)) {   //////////// caso o carrinho não esta indo para frente ou traz, abilite o modo gira hehe
    spin(ch2_arg, turn_intensity);
  }
}
void spin(int ch2_arg, int turn_intensity) {
  if (ch2_arg < minDeadPoint) {
    turn_intensity = map(ch2_arg, minWheel, minDeadPoint, 255, 0);
    if (turn_intensity >= 255) turn_intensity = 255; if (turn_intensity <= 0) turn_intensity = 0;
    analogWrite(enA, turn_intensity);
    analogWrite(enB, turn_intensity);
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);

  }
  else if (ch2_arg > maxDeadPoint) {
    turn_intensity = map(ch2_arg, maxWheel, maxDeadPoint, 255, 0);
    if (turn_intensity >= 255) turn_intensity = 255; if (turn_intensity <= 0) turn_intensity = 0;
    analogWrite(enA, turn_intensity);
    analogWrite(enB, turn_intensity);
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  }
}
void turnRight(int ch2_arg, int trigger_speed)
{
  int slower_speed = map(ch2_arg, maxDeadPoint, maxWheel, trigger_speed, 0);
  if (slower_speed >= 255) slower_speed = 255; if (slower_speed <= 0) slower_speed = 0;
  analogWrite(enA, slower_speed);  // essa roda gira numa intensidade mais lenta
  analogWrite(enB, trigger_speed);
}
void turnLeft(int ch2_arg, int trigger_speed)
{
  int slower_speed = map(ch2_arg, minDeadPoint, minWheel, trigger_speed, 0);
  if (slower_speed >= 255) slower_speed = 255; if (slower_speed <= 0) slower_speed = 0;
  analogWrite(enB, slower_speed);  // essa roda gira numa intensidade mais lenta
  analogWrite(enA, trigger_speed);
}
boolean isUpsidedown()
{
  if (up_value == 1) return true;
  return false;
}
