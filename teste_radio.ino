#define IN1 7
#define IN2 6
#define IN3 5 
#define IN4 4
#define enA 3
#define enB 2

int ch1_value;
int ch2_value;
int ch1_mapped_value;
int ch2_mapped_value;

void setup() {
  Serial.begin(9600);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
}

void loop() {
  ch1_value = pulseIn(A0, HIGH) - 1420;
  ch2_value = pulseIn(A1, HIGH) - 1420;
  deadZoneCh1(ch1_value);
  if(pulseIn(A0,HIGH)>500){
  triggerResponse(ch1_value,ch2_value);
  }
  else{
    failSafe();
  }
  //chanelValueTest(ch2_value,10);
}

void failSafe(){
    digitalWrite(IN1,LOW);digitalWrite(IN2,LOW);
    digitalWrite(IN3,LOW);digitalWrite(IN4,LOW);
}

void chanelValueTest(int chanelChoice,int sampleDelay){
  Serial.println(chanelChoice);
  if(sampleDelay > 0){
    delay(sampleDelay);
  }
}

void deadZoneCh1(int ch1_arg){
  if(ch1_arg>-10 && ch1_arg<20){
    digitalWrite(IN1,LOW);digitalWrite(IN2,LOW);
    digitalWrite(IN3,LOW);digitalWrite(IN4,LOW);
    //Serial.print("parado");
  }
}

void triggerResponse(int ch1_arg,int ch2_arg){
  ch1_mapped_value = abs(int(ch1_arg*0.48));
  ch2_mapped_value = abs(int(ch2_arg*0.45));
  Serial.println(ch1_mapped_value-ch2_mapped_value);
  if(ch1_arg>20){
    analogWrite(enA, ch1_mapped_value);
    analogWrite(enB, ch1_mapped_value - ch2_mapped_value);
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  }
  else if(ch1_arg < -10){
    analogWrite(enA, ch1_mapped_value);
    analogWrite(enB, ch1_mapped_value);
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
    Serial.println(ch1_mapped_value);
  }
}
