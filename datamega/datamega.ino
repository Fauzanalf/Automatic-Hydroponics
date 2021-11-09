#include <EEPROM.h>
#include "GravityTDS.h"
 
#define TdsSensorPin A1
GravityTDS gravityTds;
 
int tdsValue = 0;
String kirim="";
 
void setup()
{
    Serial.begin(9600);
    Serial3.begin(115200);
    gravityTds.setPin(TdsSensorPin);
    gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
    gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    gravityTds.begin();  //initialization
}
 
void loop()
{
  gravityTds.update();  //sample and calculate
  tdsValue = gravityTds.getTdsValue();  // then get the value
  Serial.println(tdsValue);
  kirim = "";
  kirim+=tdsValue;
  
  
  
  Serial3.println(kirim);
//  if(Serial3.available()){
//    String msg = "";
//    while(Serial3.available()){
//      msg += char(Serial.read());
//      delay(50);
//    }
//
//    Serial.println(msg);
//  }
    delay(500);
}
