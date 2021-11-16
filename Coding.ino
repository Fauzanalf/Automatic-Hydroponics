#include <EEPROM.h>
#include "GravityTDS.h"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#define echoPinUtama 2 // echo ultrasonik bak utama
#define trigPinUtama 3 // trigger ultrasonik bak utama
#define echoPinA 4 // echo ultrasonik pupuk A
#define trigPinA 5 // trigger ultrasonik pupuk A
#define echoPinB 6 // echo ultrasonik bak pupuk B
#define trigPinB 7 // trigger ultrasonik bak pupuk B


#define TdsSensorPin A1
GravityTDS gravityTds;
 
int tdsValue = 0;
String kirim="";

// defines variables
long durationUtama, durationA, durationB; // variable for the duration of sound wave travel
int distanceUtama, distanceA, distanceB; // variable for the distance measurement

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
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Kelompok 2.2");
  
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization
  
}
void loop() {

  ppmUtama();
  volumeUtama();

  while(distanceUtama>=20){ //isi sampe penuh
    digitalWrite(relaySelang, LOW);
    delay(2000);
    digitalWrite(relaySelang, HIGH);
  }

  if(tdsValue<=400){
    digitalWrite(relayA, LOW);
    digitalWrite(relayB, LOW);
    delay(2000);
    digitalWrite(relayUtama, LOW);
  }

  volumeA();
  volumeB();
  digitalWrite(relayUtama, LOW);
  if(distanceA>=20 || distanceB>=20){
    digitalWrite(relayUtama, HIGH);
  }
  
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
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(distanceUtama);
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
  lcd.setCursor(5, 0);
  lcd.print(distanceA);
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
  lcd.setCursor(10, 0);
  lcd.print(distanceB);
}
