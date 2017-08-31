
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

int status = WL_IDLE_STATUS;
int statusAP = WL_IDLE_STATUS;
String ownSsid = "SmartHome";
String ownPassword = "SmartHome";
String privateSsid;
String privateUser;
String privatePassword;
String moduleType;
String serverAddress;
boolean apUpFlag = true;

ESP8266WebServer server(80);
const int pin = 13;
String webPage = "";

void setup() {
  pinMode(pin, OUTPUT);
  Serial.begin(9600);
  Serial.println();

  //  Serial.setDebugOutput(true);
  //  create AP
  if (apUpFlag == true) {
    createAccessPoint();
  }
  //scan networks
  scanAndConnectToNetwork();

  //configureWebPage
  configureWebPage();
}

void loop()
{
  status = WiFi.status();
  if (status != WL_CONNECTED)  {
    if (privateSsid.length() == 0) {
      scanAndConnectToNetwork();
    } else {
      connectToWifi(privateSsid, privatePassword);
    }
  }
  if (status == WL_CONNECTED)  {
    http_client();
  }
  if (statusAP == 0 && apUpFlag == true) {
    createAccessPoint();
  }
  if (statusAP == 1 && apUpFlag == false) {
    disableAccessPoint();
  }

  blink_status();
  server.handleClient();
}

void createAccessPoint() {
  Serial.print("Configuring access point for wifi network: ");
  Serial.println(ownSsid);
  char ownSsid_char[ownSsid.length() + 1];
  ownSsid.toCharArray(ownSsid_char, ownSsid.length() + 1);
  char ownPassword_char[ownPassword.length() + 1];
  ownPassword.toCharArray(ownPassword_char, ownPassword.length() + 1);
  WiFi.softAP(ownSsid_char, ownPassword_char);
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
  Serial.println("ESP AccessPoint disabled");
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
    server.send(200, "text/html", webPage + "NodeMCU NOT connected to the network");
    Serial.println("NodeMCU connected to the network");
  } else {
    server.send(200, "text/html", webPage + "NodeMCU NOT connected to the network");
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

void http_client() {
  HTTPClient http;
  String message = "{\n \"auth_token\": \"token\", \n \"current\": ";
  message = message + random(100);
  message = message +  ", \nlast\": ";
  message = message + random(100);
  message = message +  "\n}";
  http.begin(serverAddress);
  http.addHeader("Accept - Encoding", "gzip, deflate");
  http.addHeader("Content - Type", "application / json");
  Serial.println("Server: " + serverAddress + "; Message:" + message);
  int httpCode = http.POST(message);
  if (httpCode > 0) {
    Serial.printf("[HTTP] POST... code: % d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: % s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void configureWebPage() {
  webPage += "<html>\n"
             "    <body>\n"
             "        <FORM name=\"module\" action=\"/\" method=\"post\">\n"
             "            Module params:<br>\n"
             "            <p> <INPUT type=\"text\" name=\"ssid\"> ssid<BR></p>\n"
             "            <p> <INPUT type=\"text\" name=\"user\"> user<BR></p>\n"
             "            <p><INPUT type=\"password\" name=\"password\"> password<BR></p>\n"
             "            <p> <INPUT type=\"text\" name=\"server\"> server address; format: www.google.com:80 or 192.168.1.1:8080<BR></p>\n"
             "            <p>Module type</p>\n"
             "            <p><input type=\"radio\" name=\"module\" value=\"temperature\">temperature<Br>\n"
             "                <input type=\"radio\" name=\"module\" value=\"humidity\">humidity<Br>\n"
             "                <input type=\"radio\" name=\"module\" value=\"motion\">motion<Br>\n"
             "                <input type=\"radio\" name=\"module\" value=\"ap\">access point</p>\n"
             "            <p>Module AP status: " ;
  webPage = webPage +  (apUpFlag == true ? "up" : "down") ;
  webPage += "</p>\n"
             "                <input type=\"radio\" name=\"apUpFlag\" value=\"0\">down<Br>\n"
             "            <p><INPUT type =\"submit\" value=\"update\"></p>\n"
             "        </FORM>\n"
             "    </body>\n"
             "</html>";
}

void startServer() {
  server.on("/", []() {
    Serial.println("Get request params");
    for (int i = 0; i <   server.args(); i++ ) {
      Serial.println(server.argName(i) + ":" + server.arg(i));
    }
    privateSsid = server.arg("ssid");
    privateUser = server.arg("user");
    privatePassword = server.arg("password");
    moduleType = server.arg("module");
    serverAddress = server.arg("server");
    apUpFlag = server.arg("apUpFlag") == "0" ? false : true;

    String result = webPage;
    if (privateSsid.length() != 0) {
      result = result + "\nAttempting to connect to SSID: " + privateSsid;
    }
    if (privateUser.length() != 0) {
      result = result + "\nuser name: " + privateUser;
    }
    if (privatePassword.length() != 0) {
      result = result + "\nuser password: " + privatePassword;
    }
    if (moduleType.length() != 0) {
      if (moduleType != "ap") {
        serverAddress = "http://" + serverAddress + "/widgets/" + moduleType;
        ownSsid = ownSsid + "(" + moduleType + ")" + random(1000000);
      } else {
        ownSsid = "SmartHome";
      }
      result = result + "\nCurrent module set to: " + moduleType;
      result = result + "\nReconfigure WiFi AP to: " + ownSsid + "\tPlease reconnect!";
      disableAccessPoint();
      createAccessPoint();
    }
    if (serverAddress.length() != 0) {
      result = result + "\nServer address set to: " + serverAddress;
    }
    result = result + "\nESP AP status: " ;
    result =  result + (apUpFlag == true ? "up" : "down");
    server.send(200, "text/html", result);
    delay(1000);
  });
  server.begin();
  Serial.println("HTTP server started");
}

