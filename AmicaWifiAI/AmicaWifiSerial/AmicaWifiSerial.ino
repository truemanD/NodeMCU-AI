#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "SoftwareSerial.h"
#include "pitches.h"

#define DHTPIN            2         // Pin which is connected to the DHT sensor.
#define PIN_RX  4           //D2
#define PIN_TX  5           //D1
#define DHTTYPE           DHT11     // DHT 11 
#define SPEAKER_PIN  0          //D3
#define RED_LED 14 //D5
#define GREED_LED 12 //D6

DHT_Unified dht(DHTPIN, DHTTYPE);
ADC_MODE(ADC_VCC); //vcc read
SoftwareSerial mySerial(PIN_RX, PIN_TX); // RX,TX
byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
unsigned char response[9];
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, NOTE_B0, NOTE_B3, NOTE_C4
};
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

float t_temp = 0;
float t_ppm = 0;
float t_humid = 0;
float t_vdd = 0;


void setup() {
  pinMode(RED_LED, OUTPUT);     // Initialize the RED_LED pin as an output
  pinMode(GREED_LED, OUTPUT);     // Initialize the GREED_LED pin as an output
  Serial.begin(9600);
  Serial.println("Hello");
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
  float temp = 0;
  float ppm = 0;
  float humid = 0;
  float vdd = 0;

  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    temp = event.temperature;
  }

  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    humid = event.relative_humidity;
  }

  //  Get voltage
  {
    vdd = ESP.getVcc();
    vdd = vdd / 1000;

  }
  //  Get PPM
  ppm = getCO2Data();

  Serial.print("Temperature: " + String(temp) + "*C; ");
  Serial.print("Humidity: " + String(humid) + "%; ");
  Serial.print("CO2: " + String(ppm) + "PPM; ");
  Serial.print("Voltage: " + String(vdd) + "V;");
  Serial.println();

  if ((t_ppm > 1350 & ppm > 1350) | (t_temp > 26 & temp > 26)) {
    alarm();
  } else {
    alarmOff();
  }
  t_temp = temp;
  t_humid = humid;
  t_vdd = vdd;
  t_ppm = ppm;
}

int getCO2Data() {
  int result;
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
    Serial.println("CRC error: " + String(crc) + " / " + String(response[8]));
    result = -1;
  } else {
    unsigned int responseHigh = (unsigned int) response[2];
    unsigned int responseLow = (unsigned int) response[3];
    int ppm = (256 * responseHigh) + responseLow;
    //    int temp = response[4] - 20;
    char raw[32];
    sprintf(raw, "RAW: %02X %02X %02X %02X %02X %02X %02X %02X %02X", response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7], response[8]);
    if (ppm <= 400 || ppm > 4900) {
      Serial.println("CO2: no valid data");
      result = -2;
    } else {
      result = ppm;
    }
  }
  return result;
}



void alarm() {
  alarmOn();
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(SPEAKER_PIN, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(SPEAKER_PIN);
  }
}


void alarmOn() {
  digitalWrite(GREED_LED, LOW);
  digitalWrite(RED_LED, HIGH);
}

void alarmOff() {
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREED_LED, HIGH);
}
