/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <SimpleTimer.h>
#include <DHT.h>
#include <OneWire.h>
#define SENSORPIN A0
#define SENSORPINPOOLTEMP A1
const int BT_TX = 8;
const int BT_RX = 9;
int button1_status = 0;
int waterlevel = 0;
int watertemp = 0;
long  randNumber1 ;
long  randNumber2 ;
long  randNumber3 ;
long  randNumber4 ;
float reading;
float readingwatertemp;
String readString;
String strV1; //cold Room Tmp
String strV2; //waterlevel
String strV3; //waterlevel
String strV4; //Random 2
bool Connected2Blynk = false;
// LED PINS
int ledPin_green = 5; // GREEN WIFI Connected
int ledPin_red = 6; //RED WIFI not Connected
int ledPin_blue = 7; //Blue  Booting and WIFI off
char auth[] = "9cd8bec294ab48d69dd0ae07dfbcf82b";

char ssid[] = "BELL135";
char pass[] = "Blake123";
//OneWire  ds(10);  // on pin 10 (a 4.7K resistor is necessary) <--------------------------
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 3); // RX, TX
SoftwareSerial btSerial(8, 9);// RX, TX


// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600
#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
DHT dht(8, DHTTYPE);

ESP8266 wifi(&EspSerial);
SimpleTimer timer;
WidgetLED led1(V20); //register to virtual pin 1

void setup()
{
  Serial.println("CYTRON_TOUCH_PA1_UNO");
  Serial.begin(9600);
  led1.off();
  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);
  pinMode(ledPin_green, OUTPUT);  // 5 GREEN WIFI Connected
  pinMode(ledPin_red, OUTPUT);  // 6 RED WIFI not Connected
  pinMode(ledPin_blue = 7, OUTPUT); // 7 Blue Booting & WIFI not Connected

  digitalWrite(ledPin_green, LOW);
  delay(1000);
  digitalWrite(ledPin_blue, LOW);
  digitalWrite(ledPin_red, HIGH);



  pinMode(4, INPUT);      // sets the digital pin 7 as input
  Blynk.begin(auth, wifi, ssid, pass);
  dht.begin();
  
  timer.setInterval(5000L, CheckConnection);
  timer.setInterval(5000L, Hearthbeat);
  timer.setInterval(5000L, WeatherSensor);
  timer.setInterval(5000L, WaterTemperaturesensor);
//  timer.setInterval(5000L, TempSensDS1820);  <--------------------------

}

void loop()
{
  Blynk.run();
  timer.run();

}




void CheckConnection() {
  Connected2Blynk = Blynk.connected();
  if (!Connected2Blynk) {
    Serial.println("Not connected to Blynk server");
    digitalWrite(ledPin_green, LOW);
    digitalWrite(ledPin_blue, LOW);
    digitalWrite(ledPin_red, HIGH);
    //  Blynk.connect(3333);  // timeout set to 10 seconds and then continue without Blynk
  }
  else {
    Serial.println("Connected to Blynk server");
    digitalWrite(ledPin_blue, LOW);
    digitalWrite(ledPin_red, LOW);
    digitalWrite(ledPin_green, HIGH);
    
  }
}

void Hearthbeat()
{
  randNumber2 = random(100);
  Blynk.virtualWrite(V1, randNumber2);
}




void WeatherSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, h);
  Blynk.virtualWrite(V3, t);
}




void WaterTemperaturesensor()
{
  readingwatertemp = analogRead(SENSORPINPOOLTEMP);
  watertemp = readingwatertemp - 502;
  Serial.print("Analog reading ");
  Serial.println(watertemp);
  Blynk.virtualWrite(V30, watertemp);
}








void Watersensor()
{
  reading = analogRead(SENSORPIN);

  Serial.print("Analog reading ");
  Serial.println(reading);


  Serial.print("<-- water level: ");
  if (reading < 50 ) {
    waterlevel = 0;
    Serial.println(waterlevel);
    led1.on();

  }

  if ((reading >= 50 ) && (reading < 100 )) {
    waterlevel = 1;
    Serial.println(waterlevel);
    led1.on();

  }

  if ((reading >= 100 ) && (reading < 165 )) {
    waterlevel = 2;
    Serial.println(waterlevel);
    led1.on();

  }
  if ((reading >= 165 ) && (reading < 246 )) {
    waterlevel = 3;
    Serial.println(waterlevel);
    led1.on();

  }
  if ((reading >= 246 ) && (reading < 350 )) {
    waterlevel = 4;
    Serial.println(waterlevel);
    led1.on();

  }

  if ((reading >= 350 ) && (reading < 450 )) {
    waterlevel = 5;
    Serial.println(waterlevel);
    led1.on();

  }

  if ((reading >= 450 ) && (reading < 550 )) {
    waterlevel = 6;
    Serial.println(waterlevel);
    led1.on();

  }

  if ((reading >= 550 ) && (reading < 650 )) {
    waterlevel = 7;
    Serial.println(waterlevel);
    led1.on();

  }

  if ((reading >= 650 ) && (reading < 750 )) {
    waterlevel = 8;
    Serial.println(waterlevel);
    led1.off();
    Blynk.setProperty(V3, "color", "#008000");
  }
  if ((reading >= 750 ) && (reading < 850 )) {
    waterlevel = 9;
    Serial.println(waterlevel);
    led1.off();
    Blynk.setProperty(V3, "color", "#008000");
  }
  if ((reading >= 850 ) && (reading < 950 )) {
    waterlevel = 10;
    Serial.println(waterlevel);
    led1.off();
    Blynk.setProperty(V3, "color", "#008000");
  }
  if ((reading >= 1050 ) && (reading < 1150 )) {
    waterlevel = 11;
    Serial.println(waterlevel);
    led1.off();
    Blynk.setProperty(V3, "color", "#008000");
  }
  if ((reading >= 1150 ) ) {
    waterlevel = 12;
    Serial.println(waterlevel);
    led1.off();
    Blynk.setProperty(V3, "color", "#008000");
  }
  Blynk.virtualWrite(V10, waterlevel);
  Blynk.virtualWrite(V11, waterlevel);

}



/*                                    <--------------------------
void TempSensDS1820() 
{
  
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(250);
    return;
  }
  
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

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
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius, ");
  Serial.print(fahrenheit);
  Serial.println(" Fahrenheit");
}
*/
