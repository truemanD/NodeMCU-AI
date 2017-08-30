
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


int status = WL_IDLE_STATUS;
int statusAP = WL_IDLE_STATUS;
const char ownSsid[] = "SmartHome";
const char ownPassword[] = "SmartHomeModule";
String privateSsid;
String privatePassword;
String moduleType;

ESP8266WebServer server(80);
const int pin = 13;
String webPage = "";

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
    if (privateSsid.length() == 0) {
            scanAndConnectToNetwork();
    } else {
      connectToWifi(privateSsid, privatePassword);
      privateSsid = "";
    }
  }
  if (status == WL_CONNECTED)  {
    if (statusAP == 1) {
      disableAccessPoint();
    }
  }

  blink_status();
  server.handleClient();
}

void createAccessPoint() {
  Serial.print("Configuring access point for wifi network: ");
  Serial.println(ownSsid);
  WiFi.softAP(ownSsid, ownPassword);
  statusAP = 1;
  Serial.print("ESP AccessPoint IP address: 192.168.4.1; Status: ");
  Serial.println(statusAP);
  startServer();
}

void disableAccessPoint() {
  Serial.print("Disabling access point: ");
  Serial.println(ownSsid);
  WiFi.softAPdisconnect(true);
  statusAP = 0;
  Serial.print("ESP AccessPoint disabled");
}

void configureWebPage() {
  webPage += "<html>\n"
             "    <body>\n"
             "        <FORM name=\"network\" action = \"/network\" method=\"post\">\n"
             "            Network params:<br>\n"
             "            <p> <INPUT type=\"text\" name=\"ssid\"> SSID<BR>\n"
             "            </p>\n"
             "            <p><INPUT type=\"password\" name=\"password\"> Password<BR></p>\n"
             "            <p>Module type</p>\n"
             "            <p><input type=\"radio\" name=\"module\" value=\"temperature\">temperature<Br>\n"
             "                <input type=\"radio\" name=\"module\" value=\"humidity\">humidity<Br>\n"
             "                <input type=\"radio\" name=\"module\" value=\"motion\">motion</p>\n"
             "            <p><INPUT type =\"submit\" value=\"update\"></p>\n"
             "        </FORM>\n"
             "        <FORM name=\"reset\" action = \"/reset\" method=\"post\">\n"
             "            <p><INPUT type =\"reset\" value=\"reset\"></p>\n"
             "        </FORM>\n"
             "    </body>\n"
             "</html>";
}

void startServer() {
  configureWebPage();
  server.on("/", []() {
    server.send(200, "text/html", webPage);
    delay(1000);
  });
  server.on("/network", []() {
    Serial.println("network -> get request params");
    for (int i = 0; i <   server.args(); i++ ) {
      Serial.println(server.argName(i) + ":" + server.arg(i));
    }
    privateSsid = server.arg("ssid");
    privatePassword = server.arg("password");
    moduleType = server.arg("module");
    String result = webPage + "\nAttempting to connect to SSID: " + privateSsid;
    server.send(200, "text/html", result);
    delay(1000);
  });
  server.on("/reset", []() {
    Serial.println("reset -> get request params");
    for (int i = 0; i <   server.args(); i++ ) {
      Serial.println(server.argName(i) + ":" + server.arg(i));
    }
    privateSsid = "";
    privatePassword = "";
    server.send(200, "text/html", "Current SSID set to: " + privateSsid);
    delay(1000);
  });
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
    //    Serial.printf("%d: %s, Ch:%d (%ddBm) %s %s\n", i + 1, ssid.c_str(), channel, RSSI, encryptionType == ENC_TYPE_NONE ? "open" : "", isHidden ? "hidden" : "");

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

