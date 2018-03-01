#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DS3231_SCL 1 //---- Переназначаем стандартный пин для Wire.h
#define DS3231_SDA 3 //---- Переназначаем стандартный пин для Wire.h
#define DHTPIN  2           //D4
#define PIN_RX  5           //D1
#define PIN_TX  4           //D2
#define DHTTYPE DHT11       //DHT 11 

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
DHT_Unified dht(DHTPIN, DHTTYPE);
ADC_MODE(ADC_VCC); //vcc read
SoftwareSerial mySerial(PIN_RX, PIN_TX); // RX,TX
byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
unsigned char response[9];
uint32_t delayMS;
//String suffix;
float t_temp = 0;
float t_ppm = 0;
float t_humid = 0;
float t_vdd = 0;
int delayInterval = 3000;

String webPage = "";

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.setDebugOutput(true);
  initSensors();
  disconnectFromWifi();
  disableAccessPoint();

  Wire.begin(DS3231_SDA, DS3231_SCL); //---- Запускаем I2C на нужных пинах
  lcd.init();
  lcd.backlight();
}

void loop() {
  updateSensorsValues();
  displayTelemetry();
}

void initSensors() {
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
}

void updateSensorsValues() {
  delay(delayInterval);
  int temp = 0;
  int ppm = 0;
  int humid = 0;
  float vdd = 0;

  // Temperature
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    temp = event.temperature;
  }

  // Humidity
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
  {
    ppm = getCO2Data();
  }

  t_temp = temp;
  t_humid = humid;
  t_vdd = vdd;
  t_ppm = ppm;

}

void displayTelemetry() {
  Serial.print("Temperature: " + String(t_temp) + "*C; ");
  Serial.print("Humidity: " + String(t_humid) + "%; ");
  Serial.print("CO2: " + String(t_ppm) + "PPM; ");
  Serial.print("Voltage: " + String(t_vdd) + "V;");
  Serial.println();

  lcd.setCursor(0, 0);
  lcd.print(t_ppm);
  lcd.setCursor(5, 0);
  lcd.print("co2");
  lcd.setCursor(10, 0);
  lcd.print(t_vdd);
  lcd.setCursor(14, 0);
  lcd.print("V");
  lcd.setCursor(0, 1);
  lcd.print(t_temp);
  lcd.setCursor(5, 1);
  lcd.print("C");
  lcd.setCursor(10, 1);
  lcd.print(t_humid);
  lcd.setCursor(14, 1);
  lcd.print("%");
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
    char raw[32];
    sprintf(raw, "RAW: %02X %02X %02X %02X %02X %02X %02X %02X %02X", response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7], response[8]);
    Serial.println("CRC error: " + String(crc) + " / " + String(response[8]));
    result = -1;
  } else {
    unsigned int responseHigh = (unsigned int) response[2];
    unsigned int responseLow = (unsigned int) response[3];
    int ppm = (256 * responseHigh) + responseLow;
    char raw[32];
    sprintf(raw, "RAW: %02X %02X %02X %02X %02X %02X %02X %02X %02X", response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7], response[8]);
      result = ppm;
  }
  return result;
}


void disableAccessPoint() {
  Serial.println("Disabling access point!");
  WiFi.softAPdisconnect(true);
  Serial.println("ESP AccessPoint disabled");
}


void disconnectFromWifi() {
  Serial.println("Disconnecting from wifi!");
  WiFi.disconnect();
  Serial.println("Disconnected form wifi");
}
