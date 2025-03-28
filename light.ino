#include <WiFiNINA.h>
#include <Wire.h>
#include <BH1750.h>

char ssid[] = "charitha";         
char pass[] = "1234554321";     


char HOST_NAME[] = "maker.ifttt.com";
String EVENT_START = "sunlight_detected";   
String EVENT_STOP  = "sunlight_gone";       
String IFTTT_KEY   = "chYwEUkml5HY5pyeyVU0vq";      

WiFiClient client;
BH1750 lightSensor;

bool sunlightDetected = false;
int threshold = 100; 

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Connect to WiFi
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  
  Wire.begin();
  if (lightSensor.begin()) {
    Serial.println("BH1750 sensor started");
  } else {
    Serial.println("Error starting BH1750 sensor");
    while (1); 
  }
}

void loop() {
  float lux = lightSensor.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lux");


  if (lux > threshold && !sunlightDetected) {
    sendToIFTTT(EVENT_START, lux);
    sunlightDetected = true;
  } else if (lux < threshold && sunlightDetected) {
    sendToIFTTT(EVENT_STOP, lux);
    sunlightDetected = false;
  }

  delay(5000); 
}

void sendToIFTTT(String eventName, float lightLevel) {
  String path = "/trigger/" + eventName + "/with/key/" + IFTTT_KEY;
  String query = "?value1=" + String(lightLevel, 2);

  if (client.connect(HOST_NAME, 80)) {
    Serial.println("Connected to IFTTT");

    client.println("GET " + path + query + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println();

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
      }
    }

    client.stop();
    Serial.println("Disconnected from IFTTT");
  } else {
    Serial.println("Connection to IFTTT failed");
  }
}