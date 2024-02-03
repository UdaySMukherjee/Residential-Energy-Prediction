

#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

#include "ZMPT101B.h"
#include "ACS712.h"


ZMPT101B voltageSensor(33);

ACS712 ACS(35, 3.3, 4096, 66);
char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values
float voltage = 0;
float current = 0;
int crosscount = 0;
int climb_flag = 0;
int val[100];
int max_v = 0;
float VmaxD = 0;
float VeffD = 0;
float Veff = 0;

//int crosscount = 0;
//int climb_flag = 0;
int cal[100];
int max_c = 0;
float CmaxD = 0;
float CeffD = 0;
float Ceff = 0;



void setup() {
  Serial.begin(115200);  //Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
   
  ACS.autoMidPoint();
  voltageSensor.calibrate();
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  float V = voltageSensor.getVoltageAC();
  float I = ACS.mA_AC();
  float P = V * (I/1000);
  float E = P*3600;

  Serial.println(String("V = ") + V + " ~V");
  Serial.println(String("I = ") + (I/1000) + " ~A");
  Serial.println(String("P = ") + P + " ~KW");
  Serial.println(String("E = ") + E + " ~KWh");


  
  // set the fields with the values
  ThingSpeak.setField(1, V);
  ThingSpeak.setField(2, I/1000);
  ThingSpeak.setField(3, P);
  ThingSpeak.setField(4, E);  

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }

  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  delay(1000); // Wait 20 seconds to update the channel again
}
