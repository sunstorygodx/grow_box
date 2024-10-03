#include <SoftwareSerial.h>
SoftwareSerial softSerial(8, 9); // RX, TX>
int com;

long int lastTime;
long int lastTime2;

void setup(){
  Serial.begin(9600);
  softSerial.begin(9600); // инициализация программного последовательного порта
pinMode (2, OUTPUT);    //temp
pinMode (3, OUTPUT); //hum
pinMode (4, OUTPUT);   // fan1
pinMode (5, OUTPUT);    //fan2
pinMode (10, OUTPUT);     //light1
pinMode (11 , OUTPUT);    //light2
pinMode (12, OUTPUT);    //light 3

digitalWrite(2, HIGH);
digitalWrite(3, HIGH);
digitalWrite(4, HIGH);
digitalWrite(5, HIGH);

digitalWrite(7, HIGH);
}

void loop(){
  if (softSerial.available()) {
   
   com = softSerial.read();
   Serial.println(com);
   if (com == 113 || com == 119){temp(); }     
   if (com == 114 || com == 101){humi();}
   if (com == 116 || com == 105){light1();}
   if (com == 121 || com == 111){light2();}
   if (com == 117 || com == 112){light3();}
   if (com == 97 || com == 115){fan1();}
   if (com == 100 || com == 102){fan2();}
 }
 if (millis() - lastTime > 300000){
  lastTime = millis();
  digitalWrite(7, LOW);
 }
 if (millis() - lastTime2 > 300500){
  lastTime2 = millis();
  digitalWrite(7, HIGH);
 }
}
void temp(){
  if (com == 113){
    digitalWrite(2, LOW);
    //Serial.println("ВКЛЮЧИТЬ TEMP");
  }
  if (com == 119){
    digitalWrite (2, HIGH);
    //Serial.println("ВЫКЛЮЧИТЬ TEMP");
  }
}
void humi(){
  if (com == 101){
  //Serial.println("ВКЛЮЧИТЬ HUMI");
  digitalWrite(3, LOW);
  }
  if (com == 114){
  //Serial.println("ВЫКЛЮЧИТЬ HUMI");
  digitalWrite(3, HIGH);
  }
}
void light1(){
  if (com == 116){
    digitalWrite(10, HIGH);
    //Serial.println("LIGHT 1 ON");
  }
  if (com == 105){
    digitalWrite(10, LOW);
    //Serial.println("LIGHT 1 OFF");
  }
}
void light2(){
  if (com == 121){
    digitalWrite(11, HIGH);
    Serial.println("LIGHT 2 ON");
  }
  if (com == 111){
    digitalWrite(11, LOW);
    Serial.println("LIGHT 2 OFF");
  }
}
void light3(){
  if (com ==117){
    digitalWrite(12, LOW);
    //Serial.println("LIGHT 3 OFF");
  }
  if (com == 112){
    digitalWrite(12, HIGH);
    Serial.println("LIGHT 3 ON");
  }
}
void fan1(){
  if (com == 97){
    digitalWrite(4, LOW);
    //Serial.println("FAN 1 ON");
  }
  if (com == 115){
    digitalWrite(4, HIGH);
    //Serial.println("FAN 1 OFF");
  }
}
void fan2(){
  if (com == 100){
    digitalWrite(5, LOW);
    //Serial.println("FAN 2 ON");
  }
  if (com == 102){
    digitalWrite(5, HIGH);
    //Serial.println("FAN 2 OFF");
  }
}
