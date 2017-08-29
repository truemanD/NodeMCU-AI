
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


int status = WL_IDLE_STATUS;
const char *ssid = "AIWifi";    //  your network SSID (name)
const char *password = "AIWifi";  // your network password

ESP8266WebServer server(80);

void setup()
{
  Serial.begin(9600);
  Serial.println();

  //scan networks
  int n = WiFi.scanNetworks();

  String ssid;
  uint8_t encryptionType;
  int32_t RSSI;
  uint8_t* BSSID;
  int32_t channel;
  bool isHidden;

  for (int i = 0; i < n; i++)
  {
    WiFi.getNetworkInfo(i, ssid, encryptionType, RSSI, BSSID, channel, isHidden);
    Serial.printf("%d: %s, Ch:%d (%ddBm) %s %s\n", i + 1, ssid.c_str(), channel, RSSI, encryptionType == ENC_TYPE_NONE ? "open" : "", isHidden ? "hidden" : "");
  }

  //try to connect to open network
  //Serial.setDebugOutput(true);

  for (int i = 0; i < n; i++)
  {
    WiFi.getNetworkInfo(i, ssid, encryptionType, RSSI, BSSID, channel, isHidden);
    if (status != WL_CONNECTED)
    {
      if (encryptionType == ENC_TYPE_NONE)
      {
        Serial.print("Attempting to connect to open SSID: ");
        Serial.println(ssid);
        char ssid_char[ssid.length() + 1];
        ssid.toCharArray(ssid_char, ssid.length());
        status = WiFi.begin(ssid_char);
        delay(1000);
      }
    }
    if (status == WL_CONNECTED) {
      Serial.print("You're connected to the network");
    }
  }

  delay(1000);

  if (status != WL_CONNECTED) {
    Serial.print("Configuring access point...");
    WiFi.softAP(ssid, password);

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    server.on("/", handleRoot);
    server.begin();
    Serial.println("HTTP server started");
  }

  //  printCurrentNet();
  //  printWifiData();
}

void loop()
{
  server.handleClient();

  //  printCurrentNet();
}

void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

//void printWifiData() {
//  // print your WiFi shield's IP address:
//  IPAddress ip = WiFi.localIP();
//  Serial.print("IP Address: ");
//  Serial.println(ip);
//  Serial.println(ip);
//
//  // print your MAC address:
//  byte mac[6];
//  WiFi.macAddress(mac);
//  Serial.print("MAC address: ");
//  Serial.print(mac[5], HEX);
//  Serial.print(":");
//  Serial.print(mac[4], HEX);
//  Serial.print(":");
//  Serial.print(mac[3], HEX);
//  Serial.print(":");
//  Serial.print(mac[2], HEX);
//  Serial.print(":");
//  Serial.print(mac[1], HEX);
//  Serial.print(":");
//  Serial.println(mac[0], HEX);
//
//  // print your subnet mask:
//  IPAddress subnet = WiFi.subnetMask();
//  Serial.print("NetMask: ");
//  Serial.println(subnet);
//
//  // print your gateway address:
//  IPAddress gateway = WiFi.gatewayIP();
//  Serial.print("Gateway: ");
//  Serial.println(gateway);
//}
//
//void printCurrentNet() {
//  // print the SSID of the network you're attached to:
//  Serial.print("SSID: ");
//  Serial.println(WiFi.SSID());
//
//  // print the MAC address of the router you're attached to:
//  byte bssid[6];
//  WiFi.BSSID(bssid);
//  Serial.print("BSSID: ");
//  Serial.print(bssid[5], HEX);
//  Serial.print(":");
//  Serial.print(bssid[4], HEX);
//  Serial.print(":");
//  Serial.print(bssid[3], HEX);
//  Serial.print(":");
//  Serial.print(bssid[2], HEX);
//  Serial.print(":");
//  Serial.print(bssid[1], HEX);
//  Serial.print(":");
//  Serial.println(bssid[0], HEX);
//
//  // print the received signal strength:
//  long rssi = WiFi.RSSI();
//  Serial.print("signal strength (RSSI):");
//  Serial.println(rssi);
//
//  // print the encryption type:
//  byte encryption = WiFi.encryptionType();
//  Serial.print("Encryption Type:");
//  Serial.println(encryption, HEX);
//}

