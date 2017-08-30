
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


int status = WL_IDLE_STATUS;
int statusAP = WL_IDLE_STATUS;
const char ownSsid[] = "SmartHome";
const char ownPassword[] = "SmartHomeModule";
ESP8266WebServer server(80);
const int pin = 13;

void setup() {
  pinMode(pin, OUTPUT);
  Serial.begin(9600);
  Serial.println();

  //  Serial.setDebugOutput(true);

  //  create AP
  createAccessPoint();
  //scan networks
  scanAndConnectToNetwork();
}

void loop()
{
  status = WiFi.status();
  if (status != WL_CONNECTED)  {
    if (statusAP == 0) {
      createAccessPoint();
    }
    scanAndConnectToNetwork();
  }
  if (status == WL_CONNECTED)  {
    if (statusAP == 1) {
      disableAccessPoint();
    }
  }

  blink_status();
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

void createAccessPoint() {
  Serial.print("Configuring access point for wifi network ...");
  Serial.println(ownSsid);
  WiFi.softAP(ownSsid, ownPassword);
  statusAP = 1;
  Serial.print("ESP AccessPoint IP address: 192.168.4.1; Status: ");
  Serial.println(statusAP);
  startServer();
}

void disableAccessPoint() {
  Serial.print("Disabling access point ...");
  Serial.println(ownSsid);
  WiFi.softAPdisconnect(true);
  statusAP = 0;
  Serial.print("ESP AccessPoint disabled");
}

void startServer() {
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void scanAndConnectToNetwork() {
  uint8_t encryptionType;
  int32_t RSSI;
  uint8_t* BSSID;
  int32_t channel;
  bool isHidden;
  int n = WiFi.scanNetworks();
  String ssid;

  for (int i = 0; i < n; i++)  {
    WiFi.getNetworkInfo(i, ssid, encryptionType, RSSI, BSSID, channel, isHidden);
    Serial.printf("%d: %s, Ch:%d (%ddBm) %s %s\n", i + 1, ssid.c_str(), channel, RSSI, encryptionType == ENC_TYPE_NONE ? "open" : "", isHidden ? "hidden" : "");

    //connect to open Wifi;
    if (status != WL_CONNECTED)    {
      if (encryptionType == ENC_TYPE_NONE)      {
        connectToWifi(ssid, "");
      }
    }
  }
  WiFi.scanDelete();
}

void connectToWifi(String ssid,  String password) {
  char ssid_char[ssid.length() + 1];
  ssid.toCharArray(ssid_char, ssid.length() + 1);
  if (password.length() != 0) {
    char password_char[password.length() + 1];
    password.toCharArray(password_char, password.length() + 1);
    Serial.println(password_char);
    Serial.print("Attempting to connect to SSID: ");
    Serial.print(ssid_char);
    Serial.print(" with password: ");
    Serial.println(password_char);
    status = WiFi.begin(ssid_char, password_char);
  } else {
    Serial.print("Attempting to connect to open SSID: ");
    Serial.println(ssid_char);
    status = WiFi.begin(ssid_char);
  }
  int counter = 0;
  while (status != WL_CONNECTED && counter < 20)
  {
    delay(500);
    status = WiFi.status();
    counter++;
  }
  if (status == WL_CONNECTED) {
    Serial.println("NodeMCU connected to the network");
  } else {
    Serial.println("NodeMCU NOT connected to the network");
  }
}

void blink_status() {
  if (status != WL_CONNECTED) {
    digitalWrite(pin, LOW);
    delay(100);
    digitalWrite(pin, HIGH);
    delay(100);
  } else {
    digitalWrite(pin, HIGH);
  }
}



