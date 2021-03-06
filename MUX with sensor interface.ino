#include <ESP8266WiFi.h>
WiFiClient tcpClient;
#define MUX_A D4
#define MUX_B D3
#define MUX_C D2
#define ANALOG_INPUT A0
#include "dht.h"
#define dht_apin D0
dht DHT;
#define Relay D7
#define buzzer D6
#define LED D5
char *ssid = "ONE";       // maximum allowable ssid length is 27, as one terminating null character is reserved
char *password = "12341234";

void setup() {
  //Deifne output pins for Mux
  pinMode(MUX_A, OUTPUT);
  pinMode(MUX_B, OUTPUT);
  pinMode(MUX_C, OUTPUT);
  pinMode(Relay, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(115000);
  WiFi.begin(ssid, password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
}

void changeMux(int c, int b, int a) {
  if (a == 0) {
    digitalWrite(MUX_A, LOW);
  } else {
    digitalWrite(MUX_A, HIGH);
  }
  if (b == 0) {
    digitalWrite(MUX_B, LOW);
  } else {
    digitalWrite(MUX_B, HIGH);
  }
  if (c == 0) {
    digitalWrite(MUX_C, LOW);
  } else {
    digitalWrite(MUX_C, HIGH);
  }

}

void loop() {
  
  if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Lost WiFi connection.  Restarting");
      delay(50);
      ESP.restart();  // this is software reset which may not work all the time
   }

  delay(1000);
  //_____TEMPERATURE,HUMIDITY______//
  float value;
  changeMux(0, 0, 0);
  value = analogRead(ANALOG_INPUT);
  DHT.read11(dht_apin);
  Serial.print(DHT.humidity);
  Serial.print("%");
  Serial.print(" <=> ");
  Serial.print(DHT.temperature);
  Serial.print("C");
  Serial.print(" ||| ");
  //_______TOUCH________//
  float value1;
  changeMux(0, 0, 1);
  value1 = analogRead(ANALOG_INPUT);
  if (value1 > 500) {
    Serial.print("Touch");
    Serial.print(" ||| ");
    digitalWrite(Relay, HIGH);
  }
  else {
    Serial.print("No Touch");
    Serial.print(" ||| ");
    digitalWrite(Relay, LOW);

  }
  //________MOTION_______//
  float value2;
  changeMux(0, 1, 0);
  value2 = analogRead(ANALOG_INPUT);
  if (value2 > 500) {
    Serial.print("Motion");
    digitalWrite(LED, HIGH);
    Serial.print(" ||| ");
  }
  else {
    Serial.print("No Motion");
    digitalWrite(LED, LOW);
    Serial.print(" ||| ");
  }

  //________MQ-2________//
  float value3;
  changeMux(0, 1, 1);
  value3 = analogRead(ANALOG_INPUT);
  if (value3 > 350) {
    Serial.print("Toxic Gas");
    Serial.print(" ||| ");
    tone(buzzer, 1000);
    delay(200);
    noTone(buzzer);
  }
  else {
    Serial.print("Good Gas");
    Serial.print(" ||| ");
  }

  //_______FLAME__________//
  float value4;
  changeMux(1, 0, 0);
  value4 = analogRead(ANALOG_INPUT);
  if (value4 > 500) {
    Serial.print("Fire");
    Serial.print(" ||| ");
  }
  else {
    Serial.print("No Fire");
    Serial.print(" ||| ");
  }
  /////////////////////////////////////////////////////////////////
  float value5;
  changeMux(1, 0, 1);
  value5 = analogRead(ANALOG_INPUT);
  Serial.print(value5);
  Serial.print("\t");
  /////////////////////////////////////////////////////////////////
  float value6;
  changeMux(1, 1, 0);
  value6 = analogRead(ANALOG_INPUT);
  Serial.print(value6);
  Serial.print("\t");
  /////////////////////////////////////////////////////////////////
  float value7;
  changeMux(1, 1, 1);
  value7 = analogRead(ANALOG_INPUT);
  Serial.println(value7);
}
