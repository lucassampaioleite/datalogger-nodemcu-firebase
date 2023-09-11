#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//provide the token generation process info and RTDB payload printing info and other helper functions. 
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

//network credentials
#define WIFI_SSID "XXXXXXXXXXXXXXXXXXX"
#define WIFI_PASSWORD "XXXXXXXXXXXXXXXXXXX"

//firebase project API Key
#define API_KEY "AIzaXXXXXXXXXXXXXXXXXXX"

//email and password for authentication
#define USER_EMAIL "XXXXXXXXXXXXXXXXXXX"
#define USER_PASSWORD "XXXXXXXXXXXXXXXXXXX"

//RTDB URL
#define DATABASE_URL "https://XXXXXXXXXXXXXXXXXXX.firebaseio.com/"

//firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;

//user UID
String uid;

//database main path (will receive user UID)
String databasePath;

//database child nodes
String sensor1 = "/sensor1";
String sensor2 = "/sensor2";
String timePath = "/timestamp";

//parent Node (will receive the timestamp)
String parentPath;

//define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//variable to save current epoch time
int timestamp;

//timer variables (send new readings every ten seconds)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 10000;

//initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

//gets current epoch time
unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}

long mockSensorValue(){
  //mock a number between 30 and 40
  return rand() % 10 + 30;
}

void setup(){
  Serial.begin(115200);

  initWiFi();
  timeClient.begin();

  //key and credential assignments
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  
  //token generation
  config.token_status_callback = tokenStatusCallback; //addons/TokenHelper.h

  //maximum retry of token generation
  config.max_token_generation_retry = 5;

  //initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  //getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  //print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  //update database path
  databasePath = "/UsersData/" + uid + "/readings";
}

void loop(){

  //checking the time and sending the data to RTDB
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    //get current timestamp
    timestamp = getTime();
    Serial.print ("time: ");
    Serial.println (timestamp);

    parentPath= databasePath + "/" + String(timestamp);

    json.set(sensor1.c_str(), String(mockSensorValue()));
    json.set(sensor2.c_str(), String(mockSensorValue()));
    json.set(timePath, String(timestamp));
    
    //sends the data and prints the result on the serial
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }
}
