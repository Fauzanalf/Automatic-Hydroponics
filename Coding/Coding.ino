#include "DHT.h"

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
#define TdsSensorPin A1
#define DHTPIN 18
#define DHTTYPE DHT11
#define flowPin 19
GravityTDS gravityTds;

DHT dht(DHTPIN, DHTTYPE);

const int ph_Pin  = A0;
float Po = 0;
float PH_step;
int nilai_analog_PH;
double TeganganPh;
int suhu;
int kelembaban;
//untuk kalibrasi
float PHasam = 3.1;
float PHnetral = 2.7;

int tdsValue = 0;
String kirim="";
int siklus[]={400,600,800,1000};
int angkasiklus=-1;

volatile int flow_frequency; // Measures flow sensor pulses
int l_minute;
unsigned long currentTime;
unsigned long oldTime;
float calibrationFactor = 7.5;


#include <LiquidCrystal.h>


// defines variables
long durationUtama, durationA, durationB; // variable for the duration of sound wave travel
int distanceUtama, distanceA, distanceB, percentageA, percentageB,percentageUtama; // variable for the distance measurement

const int relayUtama=8, relayA=10, relayB=11, relaySelang=9;

void flow () // Interrupt function
{
   flow_frequency++;
}

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
  pinMode (ph_Pin, INPUT);
  digitalWrite(relayUtama, HIGH);
  digitalWrite(relayA, HIGH);
  digitalWrite(relayB, HIGH);
  digitalWrite(relaySelang, HIGH);

  pinMode(flowPin, INPUT);
  digitalWrite(flowPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(flowPin), flow, FALLING); // Setup Interrupt
  currentTime = millis();
  oldTime=currentTime;
  
  lcd.init();
  lcd.backlight();
  
  
//  lcd.setCursor(2, 0);
//  lcd.print("Kelompok 2.2");
  
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization

  dht.begin();

    ppmUtama();
    volumeUtama();
    volumeA();
    volumeB();
    phSensor();
    dhtsensor();
    flowsensor();
    hasil();
//    pengiriman();
}
void loop() {
  if (angkasiklus<=3){
    ppmUtama();
    volumeUtama();
    volumeA();
    volumeB();
    phSensor();
    dhtsensor();
    flowsensor();
    hasil();
    if(percentageUtama<=20){
      digitalWrite(relayUtama,HIGH);
      delay(500);
      while(percentageUtama<=80){ //isi sampe penuh
        digitalWrite(relaySelang, LOW);
        delay(5000);
        digitalWrite(relaySelang, HIGH);
        delay(2000);
        ppmUtama();
        volumeUtama();
        volumeA();
        volumeB();
        phSensor();
        dhtsensor();
        flowsensor();
        hasil();
      }
      angkasiklus++;
    }

    if(angkasiklus<0){
      angkasiklus=0;
    }

    delay(500);
    
    while(percentageA<=15 || percentageB<=15){
      digitalWrite(relayUtama, HIGH);
      digitalWrite(relayA,HIGH);
      digitalWrite(relayB,HIGH);
      delay(500);
      volumeA();
      volumeB();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Pupuk Habis!");
      lcd.setCursor(0, 2);
      lcd.print("Bak A: ");
      lcd.setCursor(7, 2);
      lcd.print(percentageA);
      lcd.setCursor(10, 2);
      lcd.print("%");
    
      lcd.setCursor(0, 3);
      lcd.print("Bak B: ");
      lcd.setCursor(7, 3);
      lcd.print(percentageB);
      lcd.setCursor(10, 3);
      lcd.print("%");
    }

    delay(500);
    
    if(tdsValue<siklus[angkasiklus]){
      digitalWrite(relayUtama, HIGH);
      while(tdsValue<siklus[angkasiklus]){
        hasil();
        digitalWrite(relayA,LOW);
        digitalWrite(relayB,LOW);
        delay(10000);
        digitalWrite(relayA,HIGH);
        digitalWrite(relayB,HIGH);
        delay(2500);
        ppmUtama();
        volumeUtama();
        volumeA();
        volumeB();
        phSensor();
        dhtsensor();
        flowsensor();
        hasil();
      }
      digitalWrite(relayUtama, LOW);
      delay(2000);
    }
    digitalWrite(relayUtama, LOW);
    delay(1000);
  }
  else{
    digitalWrite(relayUtama, HIGH);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Siap Panen!");
  }
  pengiriman();
}

void dhtsensor(){
  suhu=dht.readTemperature();
  kelembaban=dht.readHumidity();
}

void ppmUtama(){
  int tdsTemp=0;
  gravityTds.update();  //sample and calculate
  tdsTemp = gravityTds.getTdsValue(); 
  tdsValue=4*tdsTemp;// then get the value
  Serial.print(tdsValue);
  Serial.println(" ppm");

//  kirim = "";
//  kirim+=tdsValue;
//  Serial3.println(kirim);
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

void phSensor(){
  nilai_analog_PH = analogRead(ph_Pin);
  TeganganPh = 3.3 / 1024.0 * nilai_analog_PH;
  PH_step = (PHasam - PHnetral) / 3;
  Po = 7.00 + ((PHnetral - TeganganPh) / PH_step);     //Po = 7.00 + ((teganganPh7 - TeganganPh) / PhStep);
  if(Po>8){
    Po=8;
  }else if(Po<6){
    Po=6;
  }
  
  Serial.print("Nilai PH cairan: ");
  Serial.println(Po, 2);
  delay(3000);
}

void flowsensor(){
  if((millis() - oldTime) > 1000) {
    l_minute = 60*((1000.0 / (millis() - oldTime)) * flow_frequency) / calibrationFactor;
    oldTime=millis();
    flow_frequency=0;
  }
}

void hasil(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("TDS: ");
    lcd.setCursor(5,0);
    lcd.print(tdsValue);
    lcd.setCursor(9,0);
    lcd.print("ppm");

    lcd.setCursor(13,0);
    lcd.print("pH: ");
    lcd.setCursor(17,0);
    lcd.print(Po,1);

    lcd.setCursor(0, 1);
    lcd.print("Utama: ");
    lcd.setCursor(7, 1);
    lcd.print(percentageUtama);
    lcd.setCursor(10, 1);
    lcd.print("%");

    lcd.setCursor(12, 1);
    lcd.print("F: ");
    lcd.setCursor(15, 1);
    lcd.print(l_minute);
    lcd.setCursor(17, 1);
    lcd.print("L/H");
    
    lcd.setCursor(0, 2);
    lcd.print("Bak A: ");
    lcd.setCursor(7, 2);
    lcd.print(percentageA);
    lcd.setCursor(10, 2);
    lcd.print("%");
    
    lcd.setCursor(0, 3);
    lcd.print("Bak B: ");
    lcd.setCursor(7, 3);
    lcd.print(percentageB);
    lcd.setCursor(10, 3);
    lcd.print("%");

    lcd.setCursor(12,2);
    lcd.print("T: ");
    lcd.setCursor(15,2);
    lcd.print(suhu);
    lcd.setCursor(18,2);
    lcd.print("C");

    lcd.setCursor(12,3);
    lcd.print("H: ");
    lcd.setCursor(15,3);
    lcd.print(kelembaban);
    lcd.setCursor(18,3);
    lcd.print("%");
  
}

void pengiriman(){
  kirim = "";
  kirim+=tdsValue;
  kirim+=";";
  kirim+=Po;
  kirim+=";";
  kirim+=angkasiklus;
  kirim+=";";
  kirim+=percentageUtama;
  kirim+=";";
  kirim+=percentageA;
  kirim+=";";
  kirim+=percentageB;
  kirim+=";";
  kirim+=suhu;
  kirim+=";";
  kirim+=kelembaban;
  kirim+=";";
  kirim+=l_minute;
  
  Serial3.println(kirim);
  delay(500);
  
}
