#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "SoftwareSerial.h"

#define DHTPIN            2         // Pin which is connected to the DHT sensor.
#define PIN_RX  4           //D2
#define PIN_TX  5           //D1
#define DHTTYPE           DHT11     // DHT 11 


DHT_Unified dht(DHTPIN, DHTTYPE);
ADC_MODE(ADC_VCC); //vcc read
SoftwareSerial mySerial(PIN_RX, PIN_TX); // RX,TX
byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
unsigned char response[9];

void setup() {
  Serial.begin(9600);
  Serial.println();

  initSensors();
}

void loop() {
  postSensorsValues();
}

void initSensors() {
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
}


void postSensorsValues() {
  delay(10000);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    Serial.println(" *C");
  }

  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity);
    Serial.println("%");
  }

  //  Get voltage
  {
    float vdd = ESP.getVcc();
    vdd = vdd / 1000;
    Serial.print("Voltage: ");
    Serial.print(vdd);
    Serial.println(" V");
  }

  //  Get PPM
  getCO2Data();

}

void getCO2Data() {
  mySerial.write(cmd, 9);
  memset(response, 0, 9);
  mySerial.readBytes(response, 9);

  // CRC check
  int i;
  byte crc = 0;
  for (i = 1; i < 8; i++) crc += response[i];
  crc = 255 - crc;
  crc++;
  // End of CRC check
  if ( !(response[0] == 0xFF && response[1] == 0x86 && response[8] == crc) ) {
    //Serial.println("CRC error: " + String(crc) + " / " + String(response[8]));
    char raw[32];
    sprintf(raw, "RAW: %02X %02X %02X %02X %02X %02X %02X %02X %02X", response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7], response[8]);
    Serial.println(raw);
    Serial.println("CRC error: " + String(crc) + " / " + String(response[8]));
  } else {
    unsigned int responseHigh = (unsigned int) response[2];
    unsigned int responseLow = (unsigned int) response[3];
    int ppm = (256 * responseHigh) + responseLow;
    int temp = response[4] - 20;
    Serial.println(String("-=-"));
    char raw[32];
    sprintf(raw, "RAW: %02X %02X %02X %02X %02X %02X %02X %02X %02X", response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7], response[8]);
    Serial.println(raw);
    if (ppm <= 400 || ppm > 4900) {
      Serial.println("CO2: no valid data");
    } else {
      Serial.println("CO2 PPM:" + String(ppm) + "; Temp:" + String(temp));
    }
  }
}
