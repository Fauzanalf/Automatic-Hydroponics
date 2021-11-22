#include <EEPROM.h>
#include "GravityTDS.h"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#define echoPinUtama 3 // echo ultrasonik bak utama
#define trigPinUtama 2 // trigger ultrasonik bak utama
#define echoPinA 5 // echo ultrasonik pupuk A
#define trigPinA 4 // trigger ultrasonik pupuk A
#define echoPinB 7 // echo ultrasonik bak pupuk B
#define trigPinB 6 // trigger ultrasonik bak pupuk B

#define phSensorPin A0
#define TdsSensorPin A1
GravityTDS gravityTds;

float calibration_value = 31.49;
int phval = 0; 
unsigned long int avgval; 
int buffer_arr[10],temp;
int tdsValue = 0;
String kirim="";
int siklus[]={400,600,800,1000};
int angkasiklus=-1;

// defines variables
long durationUtama, durationA, durationB; // variable for the duration of sound wave travel
int distanceUtama, distanceA, distanceB, percentageA, percentageB,percentageUtama; // variable for the distance measurement

const int relayUtama=8, relayA=9, relayB=10, relaySelang=11;

void setup() {
  Serial.begin(9600);
  Serial3.begin(115200);
  pinMode(trigPinUtama, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPinUtama, INPUT); // Sets the echoPin as an INPUT
  pinMode(trigPinA, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPinA, INPUT); // Sets the echoPin as an INPUT
  pinMode(trigPinB, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPinB, INPUT); // Sets the echoPin as an INPUT

  pinMode(relayUtama, OUTPUT);
  pinMode(relayA, OUTPUT);
  pinMode(relayB, OUTPUT);
  pinMode(relaySelang, OUTPUT);
  digitalWrite(relayUtama, HIGH);
  digitalWrite(relayA, HIGH);
  digitalWrite(relayB, HIGH);
  digitalWrite(relaySelang, HIGH);

  lcd.init();
  lcd.backlight();
  
  
//  lcd.setCursor(2, 0);
//  lcd.print("Kelompok 2.2");
  
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization
  
}
void loop() {
  if (angkasiklus<=3){
    ppmUtama();
    volumeUtama();
    volumeA();
    volumeB();
//    phSensor();
    if(percentageUtama<=10){
      while(percentageUtama<=80){ //isi sampe penuh
        digitalWrite(relaySelang, LOW);
        delay(5000);
        digitalWrite(relaySelang, HIGH);
        volumeUtama();
        hasil();
      }
      angkasiklus++;
    }
    digitalWrite(relayUtama, LOW);
    if(percentageA<=20 || percentageB<=20){
      digitalWrite(relayUtama, HIGH);
      volumeA();
      volumeB();
      hasil();
    }

    hasil();

    if(tdsValue<siklus[angkasiklus]){
      while(tdsValue<siklus[angkasiklus]){
        digitalWrite(relayA,LOW);
        digitalWrite(relayB,LOW);
        delay(2000);
        digitalWrite(relayA,HIGH);
        digitalWrite(relayB,HIGH);
        ppmUtama();
        hasil();
      }
    }
  }else{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("siap panen");
  }
  pengiriman();
}

void ppmUtama(){
  gravityTds.update();  //sample and calculate
  tdsValue = gravityTds.getTdsValue();  // then get the value
  Serial.print(tdsValue);
  Serial.println(" ppm");

  kirim = "";
  kirim+=tdsValue;
  Serial3.println(kirim);
  delay(500);
}

void volumeUtama(){
  digitalWrite(trigPinUtama, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinUtama, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinUtama, LOW);
  durationUtama = pulseIn(echoPinUtama, HIGH);
  distanceUtama = durationUtama * 0.034 / 2; 
  Serial.print("Distance Utama: ");
  Serial.print(distanceUtama);
  Serial.println(" cm");
  if(distanceUtama>=50){
    percentageUtama=0;
  }else if(distanceUtama<6){
    percentageUtama=100;
  }
  else{
    percentageUtama=100-(100*(distanceUtama-6)/44);
  }
  
}

void volumeA(){
  digitalWrite(trigPinA, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinA, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinA, LOW);
  durationA = pulseIn(echoPinA, HIGH);
  distanceA = durationA * 0.034 / 2; 
  Serial.print("Distance A: ");
  Serial.print(distanceA);
  Serial.println(" cm");
  if(distanceA>=30){
    percentageA=0;
  }else if(distanceA<6){
    percentageA=100;
  }else{
    percentageA=100-(100*(distanceA-6)/26);
  }
  Serial.print(percentageA);
  Serial.println(" %");
}

void volumeB(){
  digitalWrite(trigPinB, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinB, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinB, LOW);
  durationB = pulseIn(echoPinB, HIGH);
  distanceB = durationB * 0.034 / 2; 
  Serial.print("Distance B: ");
  Serial.print(distanceB);
  Serial.println(" cm");
  if(distanceB>=30){
    percentageB=0;
  }else if(distanceB<6){
    percentageB=100;
  }
  else{
    percentageB=100-(100*(distanceB-6)/26);
  }
  Serial.print(percentageB);
  Serial.println(" %");
}

//void phSensor(){
//  for(int i=0;i<10;i++) 
// { 
//  buffer_arr[i]=analogRead(phSensorPin);
//  delay(30);
// }
//  for(int i=0;i<9;i++)
//  {
//    for(int j=i+1;j<10;j++)
//    {
//      if(buffer_arr[i]>buffer_arr[j])
//    {
//    temp=buffer_arr[i];
//    buffer_arr[i]=buffer_arr[j];
//    buffer_arr[j]=temp;
//   }
//  }
// }
// avgval=0;
// for(int i=2;i<8;i++)
// avgval+=buffer_arr[i];
// float volt=(float)avgval*5.0/1024/6;
// float ph_act = -5.70 * volt + calibration_value;
// Serial.print("pH Val:");
// Serial.println(ph_act);
// delay(1000);
//}

void hasil(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("tds: ");
    lcd.setCursor(6,0);
    lcd.print(tdsValue);
    lcd.setCursor(10,0);
    lcd.print("ppm");
    
    lcd.setCursor(0, 1);
    lcd.print("bak utama: ");
    lcd.setCursor(11, 1);
    lcd.print(percentageUtama);
    lcd.setCursor(14, 1);
    lcd.print(" %");
    
    lcd.setCursor(0, 2);
    lcd.print("bak A: ");
    lcd.setCursor(9, 2);
    lcd.print(percentageA);
    lcd.setCursor(12, 2);
    lcd.print(" %");
    
    lcd.setCursor(0, 3);
    lcd.print("bak B: ");
    lcd.setCursor(9, 3);
    lcd.print(percentageB);
    lcd.setCursor(12, 3);
    lcd.print(" %");
  
}

void pengiriman(){
  kirim = "";
  kirim+=tdsValue;
//  kirim=kirim+tdsValue+";"+angkasiklus;
  Serial3.println(kirim);
  delay(500);
  
}
