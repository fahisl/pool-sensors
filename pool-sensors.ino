/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#include <SoftwareSerial.h>
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <SimpleTimer.h>
#include <DHT.h>
#include <OneWire.h>

// Sensor Readings
float waterTemp;
float weatherTemp;
float weatherHum;

// Blynk
int heartBeat = 0;
bool blynkConnected = false;
char auth[] = "BLYNK_TOKEN";
char ssid[] = "SSID";
char pass[] = "WIFI_KEY";

// Wifi ESP8266
#define ESP8266_BAUD 9600
const int WIFI_RX = 2;
const int WIFI_TX = 3;
SoftwareSerial EspSerial(WIFI_RX, WIFI_TX); // RX, TX
ESP8266 wifi(&EspSerial);

// LED PINS
int ledPin_green = 5; // GREEN WIFI Connected
int ledPin_red = 6;   // RED WIFI not Connected
int ledPin_blue = 7;  // Blue  Booting and WIFI off

// Sensor Communications
OneWire ds(10);          // on pin 10 (a 4.7K resistor is necessary)
#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
DHT dht(8, DHTTYPE);

SimpleTimer timer;
WidgetLED led1(V20); //register to virtual pin 1

void setup() {
  Serial.begin(9600);
  Serial.println("CYTRON_TOUCH_PA1_UNO");

  led1.off();
  dht.begin();
  pinMode(4, INPUT);

  // LEDs
  pinMode(ledPin_green, OUTPUT);  // 5 GREEN WIFI Connected
  pinMode(ledPin_red, OUTPUT);  // 6 RED WIFI not Connected
  pinMode(ledPin_blue, OUTPUT); // 7 Blue Booting & WIFI not Connected
  digitalWrite(ledPin_green, LOW);
  digitalWrite(ledPin_blue, LOW);
  digitalWrite(ledPin_red, HIGH);
  
  // Recurring Jobs
  timer.setInterval(10000L, timerLoop);

  // Wifi
  EspSerial.begin(ESP8266_BAUD);
  delay(10);
  Blynk.begin(auth, wifi, ssid, pass);
  Serial.println("Setup done");
}

void loop() {
  Blynk.run();
  timer.run();  
}

void timerLoop() {
  checkConnection();
  if (!blynkConnected) return;
  
  readSensors();
  printResults();
  updateBlynk();
}

void readSensors() {
  weatherSensor();
  waterSensor();
  heartBeat = random(100);
}

void printResults() { 
  // Heartbeat
  Serial.print("Heartbeat: ");
  Serial.println(heartBeat);

  // Weather
  Serial.print("Weather: ");
  Serial.print(weatherHum);
  Serial.print(" %h / ");
  Serial.print(weatherTemp);
  Serial.println(" deg Celsius");

  // Water
  Serial.print("Water: ");
  Serial.print(waterTemp);
  Serial.println(" deg Celsius");

  Serial.println();
}

void updateBlynk() {
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  
  Blynk.virtualWrite(V1, heartBeat);
  Blynk.virtualWrite(V2, weatherHum);
  Blynk.virtualWrite(V3, weatherTemp);
  // TODO: write waterTemp
}

void checkConnection() { 
  blynkConnected = Blynk.connected();
  Serial.print("Connected to Blynk: ");
  Serial.println(blynkConnected);
  
  if (!blynkConnected) {
    digitalWrite(ledPin_green, LOW);
    digitalWrite(ledPin_blue, LOW);
    digitalWrite(ledPin_red, HIGH);
  }
  
  else {
    digitalWrite(ledPin_blue, LOW);
    digitalWrite(ledPin_red, LOW);
    digitalWrite(ledPin_green, HIGH);
  }
}

void weatherSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Store the results
  weatherTemp = t;
  weatherHum = h;
}

void waterSensor() {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;
  
  if (!ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      type_s = 1;
      break;
    case 0x28:
      type_s = 0;
      break;
    case 0x22:
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // start conversion, with parasite power on at the end
  
  delay(1000);     
  // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } 
  
  else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;

  // Store the result
  waterTemp = celsius;
}

