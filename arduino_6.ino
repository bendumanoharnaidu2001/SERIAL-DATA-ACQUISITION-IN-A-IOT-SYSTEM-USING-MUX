#include <ESP8266WiFi.h>
#include "DHTesp.h"
DHTesp dht;
#include <PubSubClient.h>

#define ANALOG_INPUT A0
#define MUX_A D4
#define MUX_B D3
#define MUX_C D2
#define dht_apin D0
#define LED D5
#define buzzer D6
#define Relay D7
#define LED1 D1
int sensor = 16;


WiFiClientSecure wifiClient;

char *ssid = "ONE";    
char *password = "12341234"; 
const char* mqtt_username = "manu";
const char* mqtt_password = "1234";

const char caCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFLTCCBBWgAwIBAgISA9IpPfLosIlc0KD2XKS6SnYpMA0GCSqGSIb3DQEBCwUA
MDIxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQD
EwJSMzAeFw0yMTEwMjAxMTU4MTdaFw0yMjAxMTgxMTU4MTZaMBYxFDASBgNVBAMT
C21hbnVpb3QueHl6MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsWWt
9Y2C9NUvfI0AFYpV2HWOdVj0JzBwgNMapd67C8ebunEBXokFjF8U6iJXQnhwZuS8
TxzlpD8otQrHoeIfuOXQ/Ae06MpB4GswYry44nsi9YvJU2gKdfiCIAuykiDmZKb7
gzpAXT9au8vft7ldEm9tnmDceVdvO/ZbAB5iQ1qxJ9B09/0YAdK5OQx2Q47HGK38
Ukea/YijinReTNsnwNdyUBTUCaXnl+8DSc+HxaXBLD2BmhroJ9Pk5uOluUnofdcL
FZsCI+apUFz4my4D9g6zPNYRBdZf4xWcLgM0J9EMmLOtXGIAeTBxE0yJ1dFh7S+a
jjmNjOIH/d/gcMrqewIDAQABo4ICVzCCAlMwDgYDVR0PAQH/BAQDAgWgMB0GA1Ud
JQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjAMBgNVHRMBAf8EAjAAMB0GA1UdDgQW
BBSH6Wwroyi2cWlhwOSSyX2pi5kSezAfBgNVHSMEGDAWgBQULrMXt1hWy65QCUDm
H6+dixTCxjBVBggrBgEFBQcBAQRJMEcwIQYIKwYBBQUHMAGGFWh0dHA6Ly9yMy5v
LmxlbmNyLm9yZzAiBggrBgEFBQcwAoYWaHR0cDovL3IzLmkubGVuY3Iub3JnLzAn
BgNVHREEIDAeggttYW51aW90Lnh5eoIPd3d3Lm1hbnVpb3QueHl6MEwGA1UdIARF
MEMwCAYGZ4EMAQIBMDcGCysGAQQBgt8TAQEBMCgwJgYIKwYBBQUHAgEWGmh0dHA6
Ly9jcHMubGV0c2VuY3J5cHQub3JnMIIBBAYKKwYBBAHWeQIEAgSB9QSB8gDwAHYA
RqVV63X6kSAwtaKJafTzfREsQXS+/Um4havy/HD+bUcAAAF8ncfqkgAABAMARzBF
AiEAntxar8h1FdIK8o3If0Inrz2J13/3AeY6ElfOX5KGlq4CIGFtTG7O9Xrmefqe
59LRT0WWKHqWnxIqcsfS55zMMHlSAHYAb1N2rDHwMRnYmQCkURX/dxUcEdkCwQAp
Bo2yCJo32RMAAAF8ncfrOwAABAMARzBFAiA7pwybpfn0fNw4p8sGHgMQNCJ5Nzmn
IOK4SyOmXl7tjgIhAMDPDr0ij9Fel3gyrq3GSTFelPuNVo1bDYeOuemM6mcOMA0G
CSqGSIb3DQEBCwUAA4IBAQCyD85rW7MnNxfPyUlWRiYwVQSm3Q7akFvkapaGeWIt
Ik9ADQ/sATGAj/YSqPFASyYSOGtFrzRPG6tutWYDcycA7Fwf3kkves1jM1tz+kEn
pT4gWWf+t0a6TJDdx9J2ftUs4SpvCZy5y5ss92keKl07V5B7zF1XtJNwfnpCb/Dd
tmcDvW9agh2adx5ACz9hwGm+3phUEjOrNfRQUPgNJyZ4B4RqQtPIIBsMIJ4o4TCJ
k0uaurBwU1dcrAwzQoLbfMJ6VBLSeNl6mVDT9KAa04FN9ycmnTvb/WVI71ox5gG7
pSHNNjntmopE9XIqNZPEygVhWrFpRIfUg+pC/EZNy03H
-----END CERTIFICATE-----
)EOF";

const uint8_t mqttCertFingerprint[] = {0x96, 0x6D, 0x27, 0xF0, 0xEC, 0xC3, 0x9C, 0x35, 0x66, 0x72, 0x7F, 0x55, 0x0B, 0x12, 0x85, 0x90, 0x65, 0xB8, 0x1F, 0x4D};
X509List caCertX509(caCert); 

const char* mqtt_server = "manuiot.xyz";
const char* clientID = "MyDevice";
PubSubClient client(mqtt_server, 8883, wifiClient); // 1883 is the secure listener port for the Broker

void setup()
{
  Serial.begin(115200);
  Serial.println();

  pinMode(D7, OUTPUT);
  digitalWrite(D7, HIGH);
  pinMode(D2, OUTPUT);     // MUX_C
  pinMode(D3, OUTPUT);     // MUX_B
  pinMode(D4, OUTPUT);     // MUX_A
  pinMode(D5, OUTPUT);     // LED
  pinMode(D6, OUTPUT);     // BUZZER
  pinMode(D1, OUTPUT);
  dht.setup(16, DHTesp::DHT11); // D0
  pinMode(sensor, INPUT);


  // Connect to the WiFi
  WiFi.begin(ssid, password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  wifiClient.setTrustAnchors(&caCertX509);
  wifiClient.setFingerprint(mqttCertFingerprint); 

  // setCallback sets the function to be called when a message is received.
  client.setCallback(ReceivedMessage);

  // Connect to MQTT Broker
  if (Connect()) {
    Serial.println("Connected Successfully to MQTT Broker!");
  }
  else {
    Serial.println("Connection Failed!");
  }



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

  float motion_sensor;
  float touch_sensor;
  float gas_sensor;
  float flame_sensor;
  float temperature, humidity;
  char msg[256];

  // check WiFi status
  // if WiFi is not connected, powercycle the device
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Lost WiFi connection.  Restarting");
    delay(50);
    ESP.restart();  // this is software reset which may not work all the time
  }

  // If the connection is lost, try to connect again
  if (!client.connected()) {
    Connect();
  }
  // client.loop() just tells the MQTT client code to do what it needs to do itself (i.e. check for messages, etc.)
  client.loop();
  /////////////////////////////////////////////////////////////////////

  changeMux(0, 0, 1);
  float value1 = analogRead(ANALOG_INPUT);
  touch_sensor = read_touch_sensor();
  Serial.print(touch_sensor);
  if (touch_sensor > 500)
  {
    mqtt_client_publish("LED", "ON");
    Serial.print("Touch");  Serial.print(" ||| ");  
  }
  else
  {
    Serial.print("Not Touch"); Serial.print(" ||| ");
  }

  /////////////////////////////////////////////////////////////////////
  changeMux(0, 1, 0);
  float value2 = analogRead(ANALOG_INPUT);
  motion_sensor = read_motion_sensor();
  value2 = analogRead(ANALOG_INPUT);
  Serial.print(motion_sensor);
  if (motion_sensor > 500)
  {
    Serial.print("Motion"); digitalWrite(LED, HIGH); Serial.print(" ||| ");
  }
  else
  {
    Serial.print("No Motion"); digitalWrite(LED, LOW); Serial.print(" ||| ");
  }

  //////////////////////////////////////////////////////////////////////
  changeMux(0, 1, 1);
  float value3 = analogRead(ANALOG_INPUT);
  gas_sensor = read_gas_sensor();
  Serial.print(gas_sensor);
  if (gas_sensor > 500) {
    Serial.print("Toxic Gas");  Serial.print(" ||| ");
    mqtt_client_publish("Toxic","ON");
    beep_buzzer();
  }
  else {
    Serial.print("Good Gas");  Serial.print(" ||| ");
  }
  ///////////////////////////////////////////////////////////////////////
  changeMux(1, 0, 0);
  float value4 = analogRead(ANALOG_INPUT);
  flame_sensor = read_flame_sensor();
  Serial.print(flame_sensor);
  if (flame_sensor > 500) {
    Serial.print("Fire"); Serial.print(" ||| ");
  }
  else {
    Serial.print("No Fire"); Serial.print(" ||| ");
  }

  delay(1000);
  /////////////////////////////////////////////////////////////////////
  changeMux(1, 0, 1);
  float value5 = analogRead(ANALOG_INPUT);
  float Temp = read_dht11_temp();
  Serial.print(Temp);
  Serial.print("C");
  Serial.print(" ||| ");
  float Humid = read_dht11_hum();
  Serial.print(Humid);
  Serial.print("H");
  Serial.print('\n');
}
//////////////////////////////////////////////////////////////////////
float read_touch_sensor() {
  changeMux(0, 0, 1);
  return (float)analogRead(ANALOG_INPUT);  // Note: Motion sensor output is 1 when motion is detected
}

float read_motion_sensor() {
  changeMux(0, 1, 0);
  return (float)analogRead(ANALOG_INPUT);  // Note: Motion sensor output is 1 when motion is detected
}

float read_gas_sensor() {
  changeMux(0, 1, 1);
  return (float)analogRead(ANALOG_INPUT);  // Note: Motion sensor output is 1 when motion is detected
}

float read_flame_sensor() {
  changeMux(1, 0, 0);
  return (float)analogRead(ANALOG_INPUT);  // Note: Motion sensor output is 1 when motion is detected
}

float read_dht11_temp() {
  return (float)dht.getTemperature();
  float temperature = read_dht11_temp();
}

float read_dht11_hum() {
  return (float)dht.getHumidity();
  float humidity = read_dht11_hum();
}

void put_on_relay() {
  digitalWrite(D7, HIGH);  // RELAY.  We have inverted D0 in hardware

}

void put_off_relay() {
  digitalWrite(D7, LOW); // RELAY.  We have inverted D0 in hardware
}

void beep_buzzer() {
  digitalWrite(D6, HIGH);  // produces a beep of 200 msesc duration
  delay(500);
  digitalWrite(D6, LOW);
}

void led_on() {
  digitalWrite(D1, HIGH);  // produces a beep of 200 msesc duration
}

void led_off() {
  digitalWrite(D1, LOW);   // produces a beep of 200 msesc duration
}


/////////////////////////////////////////////////////////////////////////////////////
void ReceivedMessage(char* topic, byte* payload, unsigned int payload_length) {
  // Output the first character of the message to serial (debug)
  char msg[256];
  int i;
  float touch_sensor;
  float motion_sensor;
  float gas_sensor;
  float temperature, humidity;
  float temp;
  float humid;
  float flame_sensor;
  char* touch;
  char* motion;
  char* gas;
  char* flame;
  boolean send_sensors;

  if (payload_length > 0) {
    if (strncmp((char*)payload, "RELAY:OFF", payload_length) == 0) {
      put_off_relay();
    } else if (strncmp((char*)payload, "RELAY:ON", payload_length) == 0) {
      put_on_relay();
    } else if (strncmp((char*)payload, "BUZZER:ON", payload_length) == 0) {
      beep_buzzer();
    } else if (strncmp((char*)payload, "LED:ON", payload_length) == 0) {
      led_on();
    } else if (strncmp((char*)payload, "LED:OFF", payload_length) == 0) {
      led_off();
    }
  }

  touch_sensor = read_touch_sensor();
  if (touch_sensor > 500) {
    touch = "ON";
  } else {
    touch = "OFF";
  }

  motion_sensor = read_motion_sensor();
  if (motion_sensor > 500) {
    motion = "ON";
  } else {
    motion = "OFF";
  }

  gas_sensor = read_gas_sensor();
  if (gas_sensor > 500) {
    gas = "Toxic";
  } else {
    gas = "Good";
  }

  flame_sensor = read_flame_sensor();
  if (flame_sensor > 500) {
    flame = "FIRE";
  } else {
    flame = "NO FIRE";
  }
  temp = read_dht11_temp();
  temperature = temp;
  humid = read_dht11_hum();
  humidity = humid;

  sprintf(msg, "Motion: %s,  Temperature: %3.2f, Humidity: %3.2f, MQ-2: %s, Flame: %s, Touch: %s", motion, temperature, humidity, gas, flame, touch);

  mqtt_client_publish("SENSORS", msg);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Connect() {
  // Connect to MQTT Server and subscribe to the topic
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    client.subscribe("DEVICE");
    return true;
  }
  else {
    return false;
  }
}

boolean mqtt_client_publish(char* topic, char* msg) {
  boolean result;

  result = client.publish(topic, msg);
  if (!result) {
    Serial.println("Connected, but could not publish");
  }
  return result;
}
