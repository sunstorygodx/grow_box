#include <SoftwareSerial.h>
SoftwareSerial softSerial(8, 9); // RX, TX>

#include <iarduino_DHT.h>
iarduino_DHT sensor(6); 

#include <OneWire.h>

OneWire ds(10);
int temp = 0; // Глобальная переменная для хранения значение температуры с датчика DS18B20
long lastUpdateTime = 0; // Переменная для хранения времени последнего считывания с датчика
const int TEMP_UPDATE_TIME = 1000; // Определяем периодичность проверок

#include <MQ135.h>
#define analogPin A3 // аналоговый выход MQ135 подключен к пину A0 Arduino
MQ135 gasSensor = MQ135(A3); // инициализация объекта датчика




#include <Wire.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd (0x27, 16, 2);

#include <EEPROM.h>


#include "RTClib.h"
RTC_DS1307 rtc;
#define LOAD_TIME 1; 

#include "OneButton.h" 
OneButton button1(2, true); 
OneButton button2(3, true);
OneButton button3(4, true);
OneButton button4(5, true);



char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int dataYear;
int dataMonth;
int dataMinute;
int dataHour;
int dataSecond;
int dataDay;

int DAY;
int NIGHT;

long int lastTimeRTC;
long int lastTimeButtons;
long int lastTimeDist;

int light1menu;


int openMenu;
int menuRaw;
int menuRaw2;
int menuRaw3;
int menuRaw4;

int menuTimeRaw;

int dayCont;

byte dayBegin;
byte dayEnd;
byte nightBegin;
byte nightEnd;



boolean switchLcd = 0;
boolean openSet = 0;

byte dayTemp;
int nightTemp;

byte dayHum;
byte nightHum;

byte dayLight1;
byte nightLight1;

byte dayLight2;
byte nightLight2;

byte dayLight3;
byte nightLight3;

byte dayFan1;
byte nightFan1;

byte dayFan2;
byte nightFan2;

byte dayHour;
byte nightHour;

int displ;
long int lastTimeDispl;
int humi;
byte dayContin;
long int lastTime1;
long int lastTime2;
long int lastTime3;
long int lastTime4;
long int lastTime5;
long int lastTime6;
long int lastTime7;
int ppm;
void setup () {
  Serial.begin(9600);
  softSerial.begin(9600);
  Serial.println("Starting ...");  
  lcd.init();
  lcd.backlight();        //LCD
  rtc.begin();  
    if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    }//clock
    
      //rtc.adjust(DateTime(__DATE__, __TIME__));                        //time set
   
  button1.attachClick(click1);       //1
  button2.attachLongPressStart(longPressStart1);
  button2.attachDuringLongPress(longPress1);
  
  button2.attachClick(click2);          //2
  button2.attachLongPressStart(longPressStart2);       //buttons
  button2.attachDuringLongPress(longPress2);

  button3.attachClick(click3);       //3

  button4.attachClick(click4);      //4
  button4.attachLongPressStart(longPressStart4);

  dayTemp = EEPROM.read(0);
  nightTemp = EEPROM.read(1);
  dayHum = EEPROM.read(2);
  nightHum = EEPROM.read(3);
  dayLight1 = EEPROM.read(4);
  nightLight1 = EEPROM.read(5);
  dayLight2 = EEPROM.read(6);
  nightLight2 = EEPROM.read(7);
  dayLight3 = EEPROM.read(8);
  nightLight3 = EEPROM.read(9);
  dayBegin = EEPROM.read(10);
  nightBegin = EEPROM.read(11);
  dayFan1 = EEPROM.read(12);
  nightFan1 = EEPROM.read(13);
  dayFan2 = EEPROM.read(14);
  nightFan2 = EEPROM.read(15);
  dist();

  //test
  Serial.println(String("Температура днем") + dayTemp);
  Serial.println(String("Температура ночью") + nightTemp);
  Serial.println(String("влажность днем") + dayHum);
  Serial.println(String("влажность ночью") + nightHum);
  Serial.println(String("Днем включены") + dayLight1 + dayLight2 + dayLight3);
  Serial.println(String("Днем выключены") + nightLight1 + nightLight2 + nightLight3);
  Serial.println(String("ночь начало") + nightBegin);
  Serial.println(String("день начало") + dayBegin);
  
}

void loop () {
    if (millis() - lastTimeRTC > 1000){
    lastTimeRTC = millis();
    DateTime now = rtc.now();
    dataHour = now.hour();
    dataMinute = now.minute();
    dataSecond = now.second();                        //time every 1 sec
    dataDay = now.day();
    dataMonth = now.month();
    dataYear = now.year();
    dataTime();
    dayOrNight();
    if (switchLcd == 0){
    lcd.setCursor(0, 1);
    lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
    }
    
   }
   if (millis() - lastTimeButtons > 10){
    lastTimeButtons = millis();
    button1.tick();
    button2.tick();
    button3.tick();                                       //buttons every 10ms
    button4.tick();
   // Serial.println("button delay");  
   }
    

   if (millis() - lastTimeDist > 2000){
    lastTimeDist = millis();
                                                            //return temp/hum every 2000ms
       detectTemperature(); // Определяем температуру от датчика DS18b20
       Serial.println(temp); // Выводим полученное значение температуры

        ppm = gasSensor.getPPM(); // чтение данных концентрации CO2
        Serial.println(ppm); // выдача в последовательный порт
     
     dist();
   }

  

  if (millis() - lastTimeDispl > 500){
    if (displ == 1 && openMenu == 0){
      lcd.setCursor(2,0);
      lcd.print(temp);
      lcd.setCursor(2,1);
      lcd.print(humi);
      lcd.setCursor(11,1);
      lcd.print(ppm);
      lcd.setCursor(5,0);
      lcd.print("^");
      lcd.setCursor(6,0);
      if (DAY == 1){
        lcd.print(dayTemp);
      }
      if (NIGHT == 1){
        lcd.print(nightTemp);                         //return temp/hum on LCD
      }
      lcd.setCursor(5,1);
      lcd.print("^");
      lcd.setCursor(6,1);
      if (DAY == 1){
        lcd.print(dayHum);
      }
      if (NIGHT == 1){
        lcd.print(nightHum);
      }
    }
  }
}
void click1() {
  Serial.println("Button 1 click.");
  if (openMenu == 0 && switchLcd == 0){disp();}
  
  if (menuRaw >= 11){ menuRaw = -1;}
  if (openMenu == 1){menuRaw = ++menuRaw; menuRaw2 = 0;  menu();}

  if (menuRaw2 > 80){menuRaw2 = -1;}
  if (openMenu == 2){menuRaw2 = ++menuRaw2; menu2();}

  if (menuRaw3 >= 1){menuRaw3 = -1;}
  if (openMenu == 2){menuRaw3 = ++menuRaw3; menu2();}

    
}
void longPress1() {
  Serial.println("Button 1 longPress..."); 
  if (menuRaw2 > 80){menuRaw2 = -1;}
  if (openMenu == 2){menuRaw2 = ++menuRaw2; menu2();}         //2
}

void longPressStart1() {
   Serial.println("Button 1 longPress start");
   if (menuRaw2 > 80){menuRaw2 = -1;}
   if (openMenu == 2){menuRaw2 = ++menuRaw2; menu2();}
}

//-----------------------------------
//-------------------------------------------------------------------
void click2() {
  Serial.println("Button 2 click.");
  if (menuRaw <= 0){menuRaw = 11;}
  if (openMenu == 1){menuRaw = --menuRaw; menuRaw2 = 0; menu();}

  if (menuRaw2 < -1 ){menuRaw2 = 80;}
  if (openMenu == 2){menuRaw2 = menuRaw2 - 1; menu2();}

  if (menuRaw4 >= 1){menuRaw4 = -1;}
  if (openMenu == 2){menuRaw4 = ++menuRaw4; menu2();}
  
}
void longPress2() {
  Serial.println("Button 2 longPress...");
  if (menuRaw2 < -1 ){menuRaw2 = 80;}
  if (openMenu == 2){menuRaw2 = menuRaw2 - 1; menu2();}         //2
}
void longPressStart2() {
  Serial.println("Button 2 longPress start");
  if (menuRaw2 < -1 ){menuRaw2 = 80;}
  if (openMenu == 2){menuRaw2 = menuRaw2 - 1; menu2();}
}
//-------------------------------------------------------------------
void click3() {
  Serial.println("Button 3 click.");
  escape();                                              //3
}
//-------------------------------------------------------------------
void click4() {
  Serial.println("Button 4 click.");
  if (openMenu == 1){menu2();}
  if (openMenu ==1){ menuRaw3 = 0;}
}
void longPressStart4() {
  Serial.println("Button 4 longPress start");
  menuRaw = 0;
  menu();
  if (displ == 1){
    lcd.clear();
    menu();
  }
}



void dataTime(){
    Serial.print(dataHour);
    Serial.print(":");
    Serial.print(dataMinute);
    Serial.print(":");
    Serial.print(dataSecond);
    Serial.print(" ");
    
    Serial.println();
    startScreen();
}

void dist(){
  
  
    
if (millis() - lastTime1 > 295){
  lastTime1 = millis();
  
  
  if (temp <= nightTemp && NIGHT == 1){
    softSerial.write("q");
    Serial.println("SEND DATA");
  }
   
  if (temp > nightTemp && NIGHT == 1){
    softSerial.write("w");
    Serial.println("SEND DATA OFF"); 
  }
  if (temp < dayTemp && DAY == 1){
  softSerial.write("q");
  }
  if (temp > dayTemp && DAY == 1){
    softSerial.write("w");
  }
}  
  
if (millis() - lastTime2 > 120){  
  lastTime2 = millis();
  sensor.read();
  humi = sensor.hum;
  Serial.println(humi);
  if (humi <= nightHum && NIGHT == 1){
  softSerial.write("e");
  }                                             
  if (humi > nightHum && NIGHT == 1){
    softSerial.write("r");
  }
  if (humi <= dayHum && DAY == 1){
    softSerial.write("e");
  }                                              //hum
  if (humi > dayHum && DAY == 1){
    softSerial.write("r");
  }
}

if (millis() - lastTime3 > 180){
  lastTime3 = millis();
  if (nightLight1 == 1 && NIGHT == 1){    //включить ночью
    softSerial.write("t");
  }
  if (nightLight1 == 0 && NIGHT == 1){    //выключить ночью(настр)
    softSerial.write("i");
  }                                           //LIGHT 1
  if (dayLight1 == 1 && DAY == 1){      //включить днем
    softSerial.write("t");
  }
  if (dayLight1 == 0 && DAY == 1){      //выключить днем (настр)
    softSerial.write("i");
}
}  
  
if (millis() - lastTime4 > 290){
  lastTime4 = millis();
  if (nightLight2 == 1 && NIGHT == 1){    //включить ночью
    softSerial.write("y");
  }
  if (nightLight2 == 0 && NIGHT == 1){    //выключить ночью(настр)
    softSerial.write("o");
  }                                               //LIGHT 2
  if (dayLight2 == 1 && DAY == 1){      //включить днем
    softSerial.write("y");
  }
  if (dayLight2 == 0 && DAY == 1){      //выключить днем (настр)
    softSerial.write("o");
  }
} 
  
if (millis() - lastTime5 > 341){
  lastTime5 = millis();
  if (nightLight3 == 1 && NIGHT == 1){    //включить ночью
    softSerial.write("u");
  }
  if (nightLight3 == 0 && NIGHT == 1){    //выключить ночью(настр)
    softSerial.write("p");
  }                                               //LIGHT 3
  if (dayLight3 == 1 && DAY == 1){      //включить днем
    softSerial.write("u");
  }
  if (dayLight3 == 0 && DAY == 1){      //выключить днем (настр)
    softSerial.write("p");
  }
}    

if (millis() - lastTime6 > 428){
  lastTime6 = millis();
  if (dayFan1 == 1 && DAY == 1){
    softSerial.write("a");
  }
  if (dayFan1 == 0 && DAY == 1){
    softSerial.write("s");
  }                                 // FAN 1
  if (nightFan1 == 1 && NIGHT == 1){
    softSerial.write("a");
  }
  if (nightFan1 == 0 && NIGHT == 1){
    softSerial.write("s");
  }
} 
  
if (millis() - lastTime7 > 350){
  lastTime7 = millis();
  if (dayFan2 == 1 && DAY == 1){
    softSerial.write("d");
  }
  if (dayFan2 == 0 && DAY == 1){
    softSerial.write("f");
  }                                         //FAN 2
  if (nightFan2 == 1 && NIGHT == 1){
    softSerial.write("d");
  }
  if (nightFan2 == 0 && NIGHT == 1){
    softSerial.write("f");
  }
}
}

  
void startScreen(){
    if (switchLcd == 0){
    openMenu = 0;
    lcd.setCursor(0,0);
    if (dataHour < 10 ){
      lcd.setCursor(0,0);
      lcd.print("0");
      lcd.setCursor(1,0);
      lcd.print(dataHour);
    }
      else {
      lcd.setCursor(0,0);  
      lcd.print(dataHour);
    }
    lcd.print(":");
    if (dataMinute < 10){
      lcd.setCursor(3, 0);
      lcd.print("0");
      lcd.setCursor(4, 0);
      lcd.print(dataMinute);
    }
    else{
      lcd.setCursor(3,0);
      lcd.print(dataMinute);
    }
    lcd.print(":");
    if (dataSecond < 10){
      lcd.setCursor(6,0);
      lcd.print("0");
      lcd.setCursor(7,0);
      lcd.print(dataSecond);
    }
    else{
      lcd.setCursor(6,0);
      lcd.print(dataSecond);
    }
    if (dataDay < 10){
      lcd.setCursor(11,0);
      lcd.print("0");
      lcd.setCursor(12,0);
      lcd.print(dataDay);
    }
    else {
      lcd.setCursor(11,0);
      lcd.print(dataDay);
    }
    lcd.setCursor(13,0);
    lcd.print("/");
    if (dataMonth < 10) {
      lcd.setCursor(14,0);
      lcd.print("0");
      lcd.setCursor(15,0);
      lcd.print(dataMonth);
    }
    else{
      lcd.setCursor(14,0);
      lcd.print(dataMonth);
    }
    lcd.setCursor(12,1);
    lcd.print(dataYear);
    }       
}
void escape(){
  if (openMenu == 1){
    switchLcd = 0;
    openMenu = 0;
    lcd.clear();
    startScreen();
  }
  if (openMenu == 2){
    EEPROM.update(0, dayTemp);
    EEPROM.write(1, nightTemp);
    EEPROM.write(2, dayHum);
    EEPROM.write(3, nightHum);
    EEPROM.write(4, dayLight1);
    EEPROM.write(5, nightLight1);
    EEPROM.write(6, dayLight2);
    EEPROM.write(7, nightLight2);
    EEPROM.write(8, dayLight3);
    EEPROM.write(9, nightLight3);
    EEPROM.write(10, dayBegin);
    EEPROM.write(11, nightBegin);
    EEPROM.write(12, dayFan1);
    EEPROM.write(13, nightFan1);
    EEPROM.write(14, dayFan2);
    EEPROM.write(15, nightFan2);
    menu();
  }
  if (openMenu == 0 && displ == 1){
    switchLcd = 0;
    displ = 0;
    lcd.clear();
    startScreen();
  }
}
void dayOrNight(){
 dayEnd = nightBegin;
 nightEnd = dayBegin;
  if (dataHour <= nightBegin && dataHour > nightEnd){
    NIGHT = 1;
    DAY = 0;
    Serial.println("NIGHT");
    dist();
  }
  
  if (dataHour >= nightBegin && dataHour < nightEnd){
    NIGHT = 1;
    DAY = 0;
    Serial.println("NIGHT");
    
    dist();
  }

  if (dataHour >= dayBegin && dataHour > dayEnd){
    DAY = 1;
    NIGHT = 0;
    Serial.println("DAY"); 
    dist();
    }
  if (dataHour < dayBegin && dataHour < dayEnd){
    DAY = 1;
    NIGHT = 0;
    Serial.println("DAY"); 
    dist();
  }
}

void disp(){
  displ = 1;
  switchLcd = 1;
  lcd.clear();
  lcd.print("T:");
  lcd.setCursor(0,1);
  lcd.print("H:");
  lcd.setCursor(11,0);
  lcd.print("CO2");
}

void menu(){
  openMenu = 1;
  switchLcd = 1;
  if (menuRaw < 2){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TEMP DAY");
  lcd.setCursor(0,1);
  lcd.print("TEMP NIGHT");
  }
  if (menuRaw == 0){
    lcd.setCursor(5,0);
    lcd.blink();
  }
  if (menuRaw == 1){
    lcd.setCursor(5,1);
    lcd.blink();
  }
  if (menuRaw >= 2){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("HUM DAY");
  lcd.setCursor(0,1);
  lcd.print("HUM NIGHT");
  }
  if (menuRaw == 2){
    lcd.setCursor(5, 0);
    lcd.blink();
  }
  if (menuRaw == 3){
    lcd.setCursor(5,1);
    lcd.blink();
  } 
  if (menuRaw >= 4){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LIGHT1");
  lcd.setCursor(0,1);
  lcd.print("LIGHT2");
  }
  if (menuRaw == 4){
    lcd.setCursor(6,0);
    lcd.blink();
  }
  if (menuRaw == 5){
    lcd.setCursor(6,1);
    lcd.blink();
  }
  if (menuRaw >= 6){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LIGHT3");
  lcd.setCursor(0,1);
  lcd.print("DAY START");
  }
  if (menuRaw == 6){
    lcd.setCursor(6,0);
    lcd.blink();
  }
  if (menuRaw == 7){
    lcd.setCursor(6,1);
    lcd.blink();
  }
  if (menuRaw >= 8){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("NIGHT START");
    lcd.setCursor(0,1);
    lcd.print("FAN 1");
  }
  if (menuRaw == 8){
    lcd.setCursor(7,0);
    lcd.blink();
  }
  if (menuRaw == 9){
    lcd.setCursor(6,1);
    lcd.blink();
  }
  if (menuRaw >=10){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("FAN 2");
    lcd.setCursor(0,1);
    lcd.print("---------");
  }
  if (menuRaw == 10){
    lcd.setCursor(6,0);
    lcd.blink();
  }
  if (menuRaw == 11){
    lcd.setCursor(0,1);
    lcd.blink();
  }
}

void menu2(){
  openMenu = 2;
  if (menuRaw == 0){
    lcd.clear();                                              // temp DAY
    lcd.print("DAY TEMP _");
    dayTemp = menuRaw2;
    lcd.print(dayTemp);
  }
  if (menuRaw == 1){
    lcd.clear();                                              // temp NIGHT
    lcd.print("NIGHT TEMP _");
    nightTemp = menuRaw2;
    lcd.print(nightTemp);
  }
  if (menuRaw == 2){
    lcd.clear();                                              // hum DAY
    lcd.print("HUM NIGHT _");
    dayHum = menuRaw2;
    lcd.print(dayHum);
  }
  if (menuRaw == 3){
    lcd.clear();                                              // hum NIGHT
    lcd.print("HUM NIGHT _");
    nightHum = menuRaw2;
    lcd.print(nightHum);
  }
  if (menuRaw == 4){
    dayLight1 = menuRaw3;
    nightLight1 = menuRaw4;
    lcd.clear();
    lcd.print("LIGHT1:DAY_");                                      //light 1
    lcd.print(dayLight1);
    lcd.setCursor(0,1);
    lcd.print("LIGHT1:NIGHT_");
    lcd.print(nightLight1);
  }
  if (menuRaw == 5){
    dayLight2 = menuRaw3;
    nightLight2 = menuRaw4;
    lcd.clear();
    lcd.print("LIGHT2:DAY_");                                      //light 2
    lcd.print(dayLight2);
    lcd.setCursor(0,1);
    lcd.print("LIGHT2:NIGHT_");
    lcd.print(nightLight2);
  }
  if (menuRaw == 6){
    dayLight3 = menuRaw3;
    nightLight3 = menuRaw4;
    lcd.clear();
    lcd.print("LIGHT3:DAY_");                                      //light 3
    lcd.print(dayLight3);
    lcd.setCursor(0,1);
    lcd.print("LIGHT3:NIGHT_");
    lcd.print(nightLight3);
  } 
  if (menuRaw == 7){
    if (menuRaw2 > 24){menuRaw2 = 0;}
    dayBegin = menuRaw2;                                       //day
    lcd.clear();
    lcd.print("DAY BEGIN AT_");
    lcd.print(dayBegin); 
  }
  if (menuRaw == 8){
    if (menuRaw2 > 24){menuRaw2 = 0;}
    nightBegin = menuRaw2;                                       //night
    lcd.clear();
    lcd.print("NIGHT BEGIN AT_");
    lcd.print(nightBegin); 
}
 if (menuRaw == 9){
    dayFan1 = menuRaw3;
    nightFan1 = menuRaw4;
    lcd.clear();
    lcd.print("FAN 1:DAY_");                                      //FAN 1
    lcd.print(dayFan1);
    lcd.setCursor(0,1);
    lcd.print("FAN 1:NIGHT_");
    lcd.print(nightFan1);
 }

 if (menuRaw == 10){
    dayFan2 = menuRaw3;
    nightFan2 = menuRaw4;
    lcd.clear();
    lcd.print("FAN 2:DAY_");                                      //FAN 2
    lcd.print(dayFan2);
    lcd.setCursor(0,1);
    lcd.print("FAN 2:NIGHT_");
    lcd.print(nightFan2);
 }
 
 if (menuRaw == 11){
  switchLcd = 0;
  lcd.clear();
  startScreen();
 }
}
int detectTemperature(){
  byte data[2];
  ds.reset();
  ds.write(0xCC);
  ds.write(0x44);
  if (millis() - lastUpdateTime > TEMP_UPDATE_TIME)
  {
    lastUpdateTime = millis();
    ds.reset();
    ds.write(0xCC);
    ds.write(0xBE);
    data[0] = ds.read();
    data[1] = ds.read();
    // Формируем значение
    temp = (data[1] << 8) + data[0]; temp = temp >> 4;
  }
}
