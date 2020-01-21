
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include "ThingSpeak.h"
#include "secrets.h"

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;
ESP8266WebServer server;
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

bool ota_flag = true;
uint16_t time_elapsed = 0;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

int mnumber = 0;
int snumber = 0;
int zp=0; 
int zw=0;                   // pivot for wifi setup loop
int led = 2;                // the pin that the LED is atteched to '''sensor light'''output   d4
int sensor = 13;              // the pin that the sensor data is atteched to d7
int state = LOW;             // by default, no motion detected
int val = 0; 
long timek = 0; 
int y = 0;
int x = 0;
long rssi = 0;
int tank_empty = 5;             // the pin that sens tank water level data is atteched to d6
int tank_full = 4;
int tankdata = 0;
int tankmotor = 0;
const int analog_ip = A0;
int inputVal  = 0;

void setup() {
//***********************************************
//Serial.begin(115200);  // Initialize serial
  pinMode(16, OUTPUT);   // motor on light
//WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
 // pinMode(led, OUTPUT);      // initalize LED as an output
  pinMode(sensor, INPUT);    // initialize sensor as an input
// pinMode(tank_empty, INPUT);
// pinMode(tank_full, INPUT);
   pinMode(tankmotor, OUTPUT);       // pin 10 for water tank full signal to relay
//***********************************************
 pinMode(led, OUTPUT);
 digitalWrite(tankmotor , HIGH);
  digitalWrite(16, HIGH);
  digitalWrite(2, HIGH);
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
                                                       WiFi.begin(ssid, pass);
                                                      Serial.println("Connection Retrying...");
                                                      delay(1000);
                                                       if(zw==5){
                                                                                                 Serial.println("\nSkipped network connection.");
                                                                                                 break;
                                                                                         }
                                                     zw++;
                                                     }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("85Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("87Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("89Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("91Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("93Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("95End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/restart",[](){
    server.send(200,"text/plain", "Restarting...");
    delay(1000);
    ESP.restart();
  });
  
  server.on("/lighton",[](){
    server.send(200,"text/plain", "motionlight on..");    // this code will light on on http://192.168.43.229/light0n
    digitalWrite(led, LOW);
    delay(60000);
  });

  
  server.on("/lightoff",[](){
    server.send(200,"text/plain", "motionlight off..");    // // this code will light on on http://192.168.43.229/lightoff
    digitalWrite(led, HIGH);
    delay(60000);
  });

   server.on("/pumpon",[](){
    server.send(200,"text/plain", "pump on..");    // this code will light on on http://192.168.43.229/pumpon
    digitalWrite(tankmotor , LOW);
  });

  
  server.on("/pumpoff",[](){
    server.send(200,"text/plain", "pump off..");    // // this code will light on on http://192.168.43.229/pumpoff
    digitalWrite(tankmotor , HIGH);
    
  });
  
    server.on("/motoron",[](){
    server.send(200,"text/plain", "motor on..");    // this code will light on on http://192.168.43.229/motoron
    digitalWrite(16, LOW);
  });

  
  server.on("/motoroff",[](){
    server.send(200,"text/plain", "motor off..");    //  this code will light on on http://192.168.43.229/motoroff
    digitalWrite(16, HIGH);
  });
  
  server.on("/setflag",[](){
    server.send(200,"text/plain", "Setting flag...");  // restart when loop re start
    ota_flag = true;
    time_elapsed = 0;
  });

  server.begin();
}

void loop() {
  if(ota_flag)
               {
                  uint16_t time_start = millis();
                  while(time_elapsed < 30000)
                               {
                                     ArduinoOTA.handle();
                                     time_elapsed = millis()-time_start;
                                     delay(200);
                                     digitalWrite(led, !digitalRead(led));
                                      Serial.println("inside flag load software");
                                }
                  ota_flag = false;
               }
  server.handleClient();
  delay(1000);
  //************************************main code************************

  //this code keep the led on for 5 minute
  digitalWrite(tankmotor , HIGH);
  digitalWrite(16, HIGH);
  digitalWrite(2, HIGH);
  mnumber=digitalRead(16);

  
 
                                                           // update motor on status to 1 two times
  if(WiFi.status() != WL_CONNECTED){                       // Connect or reconnect to WiFi
  
                                          WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
                                          //delay(5000); 
                                          for (int i = 0; i <= 5; i++) {
                                                                      server.handleClient();
                                                                      delay(1000);
                                                                         } 
                                   }
 if(WiFi.status() != WL_CONNECTED){
                                          WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
                                          //delay(5000);
                                          for (int i = 0; i <= 5; i++) {
                                                                      server.handleClient();
                                                                      delay(1000);
                                                                         } 
                                  }
  x = ThingSpeak.writeField(myChannelNumber, 1, 1, myWriteAPIKey);                                      //update field 1 value to 1 .start motor update
  if(x == 200){
    Serial.println("Channel field 1 update successful....................1");
  }
  else{
    Serial.println("Problem updating field 1 channel. HTTP error code " + String(x));
  }


digitalWrite(16, LOW);                                                                                   //on the motor light and motor relay
Serial.println("digital read value of motor " + String(mnumber));
Serial.println("aquaponics  moter led  on for 5 minute relay 4 .............................");
Serial.println("Delay  4 minute 40 second motor on time 77 ");
//delay(28000); // 280000
for (int i = 0; i <= 280; i++) {
                               server.handleClient();
                               delay(1000);
                               }

x = ThingSpeak.writeField(myChannelNumber, 1, 1, myWriteAPIKey); 
 if(x == 200){
    Serial.println("Channel field 1 update successful...............2");
  }
  else{
    Serial.println("Problem updating field 1 channel. HTTP error code " + String(x));
  }
                                                                                                        //update fild 1 to value 1. and wait 20 seconds.off motor
Serial.println("Delay 20 second  motor on time 77 ");
                                                                                                    
//delay(25000);
for (int i = 0; i <= 25; i++) {
                               server.handleClient();
                               delay(1000);
                               }

digitalWrite(16, HIGH);                                                                                 //off the motor light and motor relay
Serial.println("Delay 20 second  motor relay line 88 ");

 

 timek=0;                                                                                              //chek motion sensor for 55 minute. time k change
 while(timek < 38)
                {
                   
                       rssi = WiFi.RSSI();
                      Serial.println("wifiwifiwifi " + String(rssi));
                      y = ThingSpeak.writeField(myChannelNumber, 3, rssi, myWriteAPIKey);                //update wifi signal strength
                      if(y == 200){
                                   Serial.println("wifi fild 3 signal strength load successful triger.");
                                }
                      else        {
                                   Serial.println("Problem fild 3 wifi signal strength load HTTP error code triger" + String(y));
                                }
                  
                      val = digitalRead(sensor);   // read sensor value
                      Serial.println(" waiting for restart from ota and value of sensorrrrrrrr " + String(val));

                      if (val == HIGH) {           // check if the sensor is HIGH
                                       digitalWrite(led, LOW);   // turn LED ON
                                       Serial.println("motion led on relay 1");
                                       Serial.println("Delay 20 second motion if high line 111 ");
                                       //delay(20000);

                                       for (int i = 0; i <= 20; i++) {
                                                                      server.handleClient();
                                                                       delay(1000);
                                                                     }
                                       //Serial.println("Delay 20 second motion if high line 111 ");
                                       
                                       y = ThingSpeak.writeField(myChannelNumber, 2, 10, myWriteAPIKey);

                                       if(y == 200){
                                                      Serial.println("motion field 2 (10) update successful.");
                                                    }
                                       else{
                                                      Serial.println("Problem motion (10) field 2 channel. HTTP error code " + String(y));
                                            }
                                       
                                      
    
                                    
                                     } 
                    else {
                            Serial.println("value of sensor else " + String(val));
                            digitalWrite(led, HIGH); // turn LED OFF
                            Serial.println("motion led off");
                            Serial.println("Delay 20 second motion else line 130");
                            //delay(20000);             // delay 20 second for update no motion

                             for (int i = 0; i <= 20; i++) {
                                                                      server.handleClient();
                                                                      delay(1000);
                                                           }
                            //Serial.println("Delay 20 second motion else line 130");
                            y = ThingSpeak.writeField(myChannelNumber, 2, 0, myWriteAPIKey);

                                       if(y == 200){
                                                      Serial.println("motion field 2 (0) update successful.");
                                                    }
                                       else{
                                                      Serial.println("Problem motion (0) field 2 channel. HTTP error code " + String(y));
                                            }
      
                            
                         }


                  Serial.println("Delay 20 second line 146 inside loop");
                  //delay(20000); 
                  for (int i = 0; i <= 20; i++) {
                                                                      server.handleClient();
                                                                      delay(1000);
                                                }                 
                  x = ThingSpeak.writeField(myChannelNumber, 1, 0, myWriteAPIKey);
                    if(x == 200){
                         Serial.println("Channel field 1 update successful. motor off signal");
                                }
                    else        {
                             Serial.println("motor off update error. HTTP error code " + String(x));
                                }
                                                       //********************WATER TANK ANALOG SIGNAL **************************
                                                       
                  inputVal = analogRead (analog_ip);                                                                  // Analog Values 0 to 1023
                  Serial.println("Anolog data recive is ......." + String(inputVal));
                  Serial.println("Delay 20 second line 146 inside loop motor relay2 on");
                  for (int i = 0; i <= 20; i++) {
                                                                      server.handleClient();
                                                                      delay(1000);
                                                } 
                   x = ThingSpeak.writeField(myChannelNumber, 4, inputVal, myWriteAPIKey);
                   if(x == 200){
                         Serial.println("Channel field 1 update successful. motor off signal");
                                }
                    else        {
                             Serial.println("motor off update error. HTTP error code " + String(x));
                             
                                }
                                
                 if(inputVal > 400) {
                                       digitalWrite(tankmotor , LOW);

                                       for (int i = 0; i <= 20; i++) {
                                                                      server.handleClient();
                                                                      delay(1000);
                                                                       } 
                                        x = ThingSpeak.writeField(myChannelNumber, 4, 1000, myWriteAPIKey);
                                        if(x == 200){
                                                       Serial.println("Channel field 4 update successful. main motor off signal");
                                                    }
                                        else        {
                                                       Serial.println("motor off update error. HTTP error code " + String(x));
                             
                                                    }
                                       
                                    }
                  else              {
                                       digitalWrite(tankmotor , HIGH);
                                    }
                           
                 
                                    
                                                      //********************WATER TANK ANALOG SIGNAL **************************
                  

                   Serial.println("number of motion sensor loop."+ String(timek));
                   timek++ ;
                    Serial.println("Delay 20 second timk++ ");
                    //delay(20000); 
                    for (int i = 0; i <= 20; i++) {
                                                                      server.handleClient();
                                                                      delay(1000);
                                                } 
                  
                  
                   }
  
  
  timek=0;
  Serial.println("man loop  end..........................................................nanloop");


//***********************************main code*************************

}
