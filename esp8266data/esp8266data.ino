#include <ESP8266WiFi.h>
#include "ThingSpeak.h" 

char ssid[] = "EFRANANDI-DI15";   
char pass[] = "aiueo123456";   
int keyIndex = 0;            
WiFiClient  client;

unsigned long myChannelNumber = 1531842;
const char * myWriteAPIKey = "T1EU0NP1C2BX8AJF";

// Initialize our values
int sendppm = 0;
float sendph=0;
int siklus=-1;
String myStatus = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  
  
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  
}

void loop() {
  // put your main code here, to run repeatedly:
    // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect");
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }


  if(Serial.available()){
    String msg = "";
    while(Serial.available()){
      msg += char(Serial.read());
      delay(50);
    }
//
//    Serial.print(msg);
    sendppm=splitString(msg,';',0).toInt();
    sendph=splitString(msg,';',1).toFloat();
    siklus=splitString(msg,';',2).toInt();

    
  }

  // set the fields with the values

  ThingSpeak.setField(1, sendppm);
  ThingSpeak.setField(2, sendph);

  if(siklus==0){
    myStatus=String("Siklus pertama");
  }else if(siklus==1){
    myStatus=String("Siklus Kedua");
  }else if(siklus==2){
    myStatus=String("Siklus Ketiga");
  }else if(siklus==3){
    myStatus=String("Siklus Keempat");
  }else if(siklus>3){
    myStatus=String("Siap Panen");
  }else{
    myStatus=String("tidak dalam siklus");
  }


  // set the status
  ThingSpeak.setStatus(myStatus);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  delay(20000); // Wait 20 seconds to update the channel again

}

String splitString(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
