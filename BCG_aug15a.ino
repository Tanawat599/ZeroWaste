#include "thingProperties.h"
#include <Arduino_ConnectionHandler.h>
#include <Arduino_MKRIoTCarrier.h>
#include <Servo.h>
#include <Keypad.h>
#include <Preferences.h>
#include <Arduino.h>
#include "HX711.h"
#include <Wire.h>
#include <WiFi.h>
#include <Arduino.h>
//#include <HTTPClient.h>
//#include <WiFiMulti.h>
#include <WiFiNINA.h>


MKRIoTCarrier carrier;
Preferences preferences; // Initialize Preferences
int Display = 3;

////////////////////Motor////////////////////
/*
int SENSOR_PIN = 0;
int RPWM_Output = 5;
int LPWM_Output = 6;
*/
Servo servo0;
Servo servo1;
Servo servo2;

const int in1Pin = 11;  
const int in2Pin = 10;
const int in3Pin = 7;
const int in4Pin = 6;
////////////////////Sensor////////////////////
int IrsensorPin0 = 0;
int IrsensorPin1 = 1;
int ProxyPin = 2;

const int LOADCELL_DOUT_PIN0 = 2;
const int LOADCELL_SCK_PIN0 = 3;

const int LOADCELL_DOUT_PIN1 = 4;
const int LOADCELL_SCK_PIN1 = 5;

HX711 scaleIron;
HX711 scalePlastic;

////////////////////Status////////////////////
int Irsensor0;
int Irsensor1;
int Door;
int Proxy;
int Iron;
int Plastic;
int timer = 0;
bool Enter = true;
bool Next = true;
int TimeNext = 0;
float previousIronWeight = 0;
float previousPlasticWeight = 0;
////////////////////Data////////////////////
String member[3] = {"21749", "22088", "21808"};
int scores[3] = {0, 0, 0};
String name[3] = {"Tanawat Chuthaphiromporn", "Thanawat sukamporn", "Wachirawit Tanleng"};
String RfidData[1] = {"54:EB:63:24"};

String G_member;
String G_name;
int G_scores;

String keyData;
String rfidData;

char ssid[] = "jet";
char pass[] = "0959202399";
String GAS_ID = "AKfycbwbFRBy52lX1GLJosUPAsgc0r_yS3Vl5DHAJTvYS8AtczlVJOGoS8_4JGBwpdMtbFTK";
//WiFiClientSecure client;
const char* host = "script.google.com";
unsigned long time_ms;
unsigned long time_1000_ms_buf;
unsigned long time_sheet_update_buf;
unsigned long time_dif;
#define UPDATE_INTERVAL_HOUR  (0)
#define UPDATE_INTERVAL_MIN   (0)
#define UPDATE_INTERVAL_SEC   (10)
#define UPDATE_INTERVAL_MS    ( ((UPDATE_INTERVAL_HOUR*60*60) + (UPDATE_INTERVAL_MIN * 60) + UPDATE_INTERVAL_SEC ) * 1000 )

void setup() {
  Serial.begin(115200);
  delay(1000); 

  while (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, pass); //--> Connect to your WiFi router
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to WiFi");


  carrier.noCase();
  carrier.begin();
  carrier.display.fillScreen(ST77XX_BLACK);

  servo0.attach(2);
  servo1.attach(4);
  servo2.attach(0);
  servo0.write(0);
  servo1.write(0);
  servo2.write(0);

  //pinMode(RPWM_Output, OUTPUT);
  //pinMode(LPWM_Output, OUTPUT);
  pinMode(Display, OUTPUT);
  pinMode(IrsensorPin0, INPUT);
  pinMode(IrsensorPin1, INPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
/*
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
*/
  preferences.begin("scores", false);

  scores[0] = preferences.getInt("score1", 0);
  scores[1] = preferences.getInt("score2", 0);
  scores[2] = preferences.getInt("score3", 0);

  //digitalWrite(RPWM_Output, HIGH);  
  //digitalWrite(LPWM_Output, HIGH);

  servo1.writeMicroseconds(1450);

  scaleIron.set_scale(-500);
  scaleIron.begin(LOADCELL_DOUT_PIN0, LOADCELL_SCK_PIN0);
  scalePlastic.set_scale(-500);
  scalePlastic.begin(LOADCELL_DOUT_PIN1, LOADCELL_SCK_PIN1);

  Wire.begin(8);
  Wire.onReceive(receiveEvent); 
  Home();
}

void loop() {
  Irsensor0 = digitalRead(IrsensorPin0);
  Irsensor1 = digitalRead(IrsensorPin1);
  //ArduinoCloud.update();
  
  digitalWrite(Display, HIGH);
  //Identify();
  //Enter = true;
  //ironWeight = 10.8;
  //LoadCell();
  if (Irsensor1 == LOW) {
    //Continue();
    
    Identify();
    EnterNum();
    /*
    DoorClose();
    Compressed();
    Separate();
    Done();*/
    Serial.println("Done");
    delay(500);
    for(int i = 0; i < 10 ; i++){

    }
    rfidData = "";
    Enter = true;
  }
  else if (Irsensor1 == HIGH) {
    
    Enter = true;
  }
  //Enter = true;
  Serial.println(Irsensor1);
  //Serial.println(rfidData);
    //Serial.println("Key Data in Loop: " + keyData);
}

void receiveEvent(int howMany) {
  char receivedMessage[64] = "";
  int i = 0;

  while (Wire.available()) {
    receivedMessage[i++] = Wire.read();
  }

  receivedMessage[i] = '\0';
  String receivedData = String(receivedMessage);
  Decode(receivedData);
  //Serial.println(receivedData);
}

void Decode(String receivedData) {

  if (receivedData.startsWith("Key:")) {
    keyData = receivedData.substring(4, receivedData.indexOf(';'));
    Serial.println("Keypad Input: " + keyData);
 
    if(keyData == "A"){
      Next = false;  
      Serial.println("Continue");
    }

  } else if (receivedData.startsWith("RFID:")) {
    rfidData = receivedData.substring(5, receivedData.indexOf(';'));
    Serial.println("RFID Tag: " + rfidData);
  }
  //Serial.println("Current Key Data: " + keyData);
  Serial.println(rfidData);

}

void Identify() {
  while (Enter) {
    //LoadCell();
    for (int i = 0; i < 3; i++) {
      if (keyData == member[i] || rfidData == RfidData[i]) {
        scores[i] += 10;
        /*
        if (scaleIron.get_units() < previousIronWeight || scalePlastic.get_units() < previousPlasticWeight) {
          scores[i] -= 5; // Subtract 5 points if the weight is less
          Serial.println("Weight has decreased, subtracting score.");
        } else {
          scores[i] += 10; 
        }

        previousIronWeight = scaleIron.get_units();
        previousPlasticWeight = scalePlastic.get_units();
        */
        String scoreKey = "score" + String(i + 1);
        preferences.putInt(scoreKey.c_str(), scores[i]);
        Serial.println("Score for member " + member[i] + " is " + String(scores[i]));
/*
        if (scores[i] >= 100) {
          sendToGoogleSheet(name[i], scores[i]);
        }
*/      
        //G_member = member[i].toInt();
        G_member = member[i];
        G_name = name[i];
        G_scores = String(scores[i]);

        if( Scores => 200){
          update_google_sheet();
        }
        Enter = false;
        return;
      }
    }
  }
}

void Continue() {
      //LoadCell();
    for (int i = 0; i < 3; i++) {
      if (keyData == member[i] || rfidData == RfidData[i]) {
        scores[i] += 10;
        /*
        if (scaleIron.get_units() < previousIronWeight || scalePlastic.get_units() < previousPlasticWeight) {
          scores[i] -= 5; // Subtract 5 points if the weight is less
          Serial.println("Weight has decreased, subtracting score.");
        } else {
          scores[i] += 10; 
        }

        previousIronWeight = scaleIron.get_units();
        previousPlasticWeight = scalePlastic.get_units();
        */
        String scoreKey = "score" + String(i + 1);
        preferences.putInt(scoreKey.c_str(), scores[i]);
        Serial.println("Score for member " + member[i] + " is " + String(scores[i]));
/*
        if (scores[i] >= 100) {
          sendToGoogleSheet(name[i], scores[i]);
        }
*/      
        //G_member = member[i].toInt();
        G_member = member[i];
        G_name = name[i];
        G_scores = String(scores[i]);

        if( Scores => 200){
          update_google_sheet();
        }
        Enter = false;
        return;
      }
    }
  
}


void update_google_sheet()
{
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiSSLClient class to create TCP connections
  WiFiSSLClient client;
  const int httpPort = 443; 

  if (!client.connect(host, httpPort)) { //works!
    Serial.println("connection failed");
    return;
  }

  String url = "/macros/s/" + GAS_ID + "/exec?user=";

  url += G_member;

  url += "&gender=";
  url += "MR";

  url += "&fname=";
  url += G_name;
  
  url += "&score=";
  url += G_scores;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println();
  Serial.println("closing connection");
  delay(5000);
}


void DoorClose() {
  servo0.write(90);
  delay(1000);
  if(Irsensor0 == 0){
    Door = 0;
    while(!Irsensor0){
      Error();
      carrier.Buzzer.beep(800, 300);
      delay(500);
    }
  }
  servo0.write(0);
}

void Compressed() {
  /*
  analogWrite(RPWM_Output, 100); 
  delay(7000);
  analogWrite(RPWM_Output, 0); 
  delay(500);
  analogWrite(LPWM_Output, 100); 
  servo2.write(50);
  delay(7000);
  analogWrite(LPWM_Output, 0); 
  delay(500);
  */
  ///Linear Motor///
  digitalWrite(in1Pin,LOW);
  digitalWrite(in2Pin,HIGH);
  delay(8000);
  digitalWrite(in1Pin,HIGH);
  digitalWrite(in2Pin,LOW);
  delay(8000);
  ///Motor///
  digitalWrite(in3Pin,LOW);
  digitalWrite(in4Pin,HIGH);
  delay(2000);
  digitalWrite(in3Pin,HIGH);
  digitalWrite(in4Pin,LOW);
  delay(7000);
}

void Separate() {
  int Proxy = digitalRead(ProxyPin);
  if (Proxy == 0) {
    Iron += Iron;
    servo1.writeMicroseconds(3000);
    delay(600);
    servo1.write(90);
    delay(100);

    while(Irsensor1){
      servo1.writeMicroseconds(3000);
      delay(1000);
      servo1.write(90);
      delay(200);
    }
  } else {
    Plastic += Plastic;
    bool Irsensor1Sate;
    servo1.writeMicroseconds(0);
    delay(600);
    servo1.write(90);
    delay(100);

    while(Irsensor1){
      servo1.writeMicroseconds(0);
      delay(1000);
      servo1.write(90);
      delay(200);
    }
  }
}
void onMessageChange() {
  Serial.println(message);
  for (int i = 0; i < 3; i++) {
    if (message == member[i]) {
      if (i == 0) {
        preferences.putInt("score1", scores[i]);
      } else if (i == 1) {
        preferences.putInt("score2", scores[i]);
      } else if (i == 2) {
        preferences.putInt("score3", scores[i]);
      }
      
      message = "Score for member " + member[i] + " is " + String(scores[i]);
      return;
    }
  }
  if(message == "Number of Metal Cans"){
    message = String(Iron);
  }
  else if(message == "Number of Plastic Bottle"){
    message = String(Plastic);
  }
  else if(message == "Test"){
    message = "Hello";
  }
}
void LoadCell(){
  ironWeight = scaleIron.get_units();
  plasticWeight = scalePlastic.get_units();
}

void Continue(){
  TimeNext++;
  if(TimeNext >= 15){
    rfidData = "";
    TimeNext = 0;
    Next = true;
    Enter = false;
  }
  if(TimeNext < 15){
    Enter = true;
  }
}
