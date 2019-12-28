
#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected to
// D2 corresponds to GPIO 4
const int oneWireBus = 4;     
#include <ESP8266WiFi.h>

// ThingSpeak Write API KEY
String apiKey = "---------------";

// Wifi SSID and password
const char *ssid =  "--------";
const char *pass =  "-------------";
const char* server = "api.thingspeak.com";
WiFiClient client;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

void setup() {
  // Start the Serial Monitor
  Serial.begin(115200);
  // Start the DS18B20 sensor
  WiFi.begin(ssid, pass);
  sensors.begin();
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  
}
double total=0;
int reading = 0;
void loop() {
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  total+=temperatureC;
  reading++;
  
  if(reading==60*(60/5)){
    Serial.println(total/reading);
    float temperature = total/reading;
    total=0;
    
    reading = 0;
    if (client.connect(server,80)){
        String postStr = apiKey;
        postStr +="&field1=";
        postStr += String(temperature);
        postStr += "\r\n\r\n";
        
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);
      }
      client.stop();
  }
  delay(5000);
}
