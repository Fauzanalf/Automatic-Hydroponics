#include <ESP8266WiFi.h>
#include "ThingSpeak.h" 

char ssid[] = "G309";   
char pass[] = "audrey17";   
int keyIndex = 0;            
WiFiClient  client;

unsigned long myChannelNumber = 1531842;
const char * myWriteAPIKey = "T1EU0NP1C2BX8AJF";

// Initialize our values
int sendppm = 0;

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

  // set the fields with the values

  ThingSpeak.setField(1, sendppm);

  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  if(Serial.available()){
    String msg = "";
    while(Serial.available()){
      msg += char(Serial.read());
      delay(50);
    }

    Serial.print(msg);
    sendppm=msg.toInt();
  }
  
  delay(20000); // Wait 20 seconds to update the channel again

}
