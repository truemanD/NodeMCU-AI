#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN            2         // Pin which is connected to the DHT sensor.

#define DHTTYPE           DHT11     // DHT 11 

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

int status = WL_IDLE_STATUS;
int statusAP = WL_IDLE_STATUS;
String ownSsid = "SmartHome";
String tmpOwnSsid;
String ownPassword = "SmartHome";
String networkSsid;
String networkUser;
String networkPassword ;
String moduleType = "ap";
String dashAddress;
String serverPoint;
String dashToken;
boolean apUpFlag = true;

ESP8266WebServer server(80);
const int pin = 13;
String webPage = "";

void initDefNetwork() {
  networkSsid = "HOME";
  networkPassword = "ARjOwI23";
  moduleType = "tempHumid";
  dashAddress = "demo.thingsboard.io";
  dashToken = "DGAHvEw5X9ZI6rYx8KMr";
}

void setup() {
  pinMode(pin, OUTPUT);
  Serial.begin(9600);
  Serial.println();

  initDefNetwork();

  tmpOwnSsid = ownSsid;
  Serial.setDebugOutput(true);

  //configureWebPage
  configureWebPage();

  //  create AP
  if (apUpFlag == true) {
    createAccessPoint();
  }

  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;

}

void loop()
{
  status = WiFi.status();
  if (status != WL_CONNECTED)  {
    if (networkSsid.length() == 0) {
      scanAndConnectToNetwork();
    } else {
      connectToWifi(networkSsid, networkPassword);
    }
  }
  if (status == WL_CONNECTED)  {
    initSensors();
  }


  if (statusAP == 0 && apUpFlag == true) {
    createAccessPoint();
  }
  if (statusAP == 1 && apUpFlag == false) {
    disableAccessPoint();
  }
  server.handleClient();
}

void createAccessPoint() {
  Serial.print("Configuring access point for wifi network: ");
  if (moduleType.length() != 0 && moduleType != "ap") {
    tmpOwnSsid = ownSsid + "(" + moduleType + ")";
    //    tmpOwnSsid = ownSsid + "(" + moduleType + ")" + random(1000);
  } else {
    tmpOwnSsid = ownSsid;
  }
  Serial.println(tmpOwnSsid);
  char ownSsid_char[ownSsid.length() + 1];
  tmpOwnSsid.toCharArray(ownSsid_char, tmpOwnSsid.length() + 1);
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
  Serial.println(tmpOwnSsid);
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

void configureWebPage() {
  webPage = "<html>\n"
            "    <body>\n"
            "        <FORM name=\"module\" action=\"/\" method=\"post\">\n"
            "            Network params:<br>\n"
            "            <p> <INPUT type=\"text\" name=\"ssid\" value=\"";
  webPage = webPage + networkSsid;
  webPage += "\"> ssid<BR></p>\n"
             "            <p> <INPUT type=\"text\" name=\"user\" value=\"";
  webPage = webPage + networkUser;
  webPage += "\"> user<BR></p>\n"
             "            <p><INPUT type=\"password\" name=\"password\" value=\"";
  webPage = webPage + networkPassword;
  webPage += "\"> password<BR></p>\n"
             "            Module params:<br>\n<p>Module type: ";
  webPage = webPage + moduleType;
  webPage += "</p>\n"
             "            <p><input type=\"radio\" name=\"module\" value=\"tempHumid\">tempHumid<Br>\n"
             "                <input type=\"radio\" name=\"module\" value=\"motion\">motion<Br>\n"
             "                <input type=\"radio\" name=\"module\" value=\"ap\">access point</p>\n"
             "            <p> <INPUT type=\"text\" name=\"server\" value=\"";
  webPage = webPage + dashAddress;
  webPage += "\"> server address; format: www.google.com:80 or 192.168.1.1:8080<BR></p>\n"
             "            <p><INPUT type=\"password\" name=\"token\" value=\"";
  webPage = webPage + dashToken;
  webPage += "\"> dashboard token<BR></p>\n"
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
    String tmpNetworkSsid = server.arg("ssid").length() > 0 ? server.arg("ssid") : networkSsid;
    String tmpNetworkUser = server.arg("user").length() > 0 ? server.arg("user") : networkUser;
    String tmpNetworkPassword = server.arg("password").length() > 0 ? server.arg("password") : networkPassword;
    String tmpDashToken = server.arg("token").length() > 0 ? server.arg("token") : dashToken;
    String tmpModuleType = server.arg("module").length() > 0 ? server.arg("module") : moduleType;
    String tmpDashAddress = server.arg("server").length() > 0 ? server.arg("server") : dashAddress;
    apUpFlag = server.arg("apUpFlag") == "0" ? false : true;

    if (tmpNetworkSsid.length() != 0) {
      networkSsid = tmpNetworkSsid;
    }
    if (tmpNetworkUser.length() != 0) {
      networkUser = tmpNetworkUser;
    }
    if (tmpNetworkPassword.length() != 0) {
      networkPassword = tmpNetworkPassword;
    }
    if (tmpModuleType.length() != 0) {
      if (tmpModuleType != moduleType) {
        moduleType = tmpModuleType;
        disableAccessPoint();
        createAccessPoint();
      }
      if (moduleType != "ap") {
        if (tmpDashAddress.length() != 0) {
          dashAddress = tmpDashAddress;
          //          serverPoint = "http://" + dashAddress + "/api/v1/";
        }
      }
    }
    if (tmpDashToken.length() != 0) {
      dashToken = tmpDashToken;
      //      serverPoint = serverPoint + "/telemetry";
    }
    if (tmpDashAddress.length() != 0) {
      dashAddress = tmpDashAddress;
    }
    configureWebPage();
    String result = webPage;
    result = result + "<p>ESP AP status: " ;
    result = result + (apUpFlag == true ? "up" : "down") + "</p>";
    result = result + "<p>Attempting to connect to SSID: " + networkSsid + "</p>";
    result = result + "<p>User name: " + networkUser + "</p>";
    result = result + "<p>User password: " + networkPassword + "</p>";
    result = result + "<p>Dashboard token: " + dashToken + "</p>";
    result = result + "<p>Current module set to: " + moduleType + "</p>";
    result = result + "<p>Reconfigure WiFi AP to: " + tmpOwnSsid + "</p>";
    result = result + "<p>Current serverPoint set to: " + serverPoint + "</p>";
    result = result + "<p>Dashboard address set to: " + dashAddress + "</p>";
    server.send(200, "text/html", result);
    delay(1000);
  });
  server.begin();
  Serial.println("HTTP server started");
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
    server.send(200, "text/html", webPage + "NodeMCU connected to the network");
    Serial.println("NodeMCU connected to the network");
  } else {
    server.send(200, "text/html", webPage + "NodeMCU NOT connected to the network");
    Serial.println("NodeMCU NOT connected to the network");
  }
}

void initSensors() {
  delay(delayMS);
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
    http_client(event.temperature, 0);
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
    http_client(event.relative_humidity, 1);
  }
}

void http_client(float value, int type) {
  if (moduleType != "ap") {
    if (dashAddress.length() != 0) {
      serverPoint = "http://" + dashAddress + "/api/v1/";
    }
  }
  if (dashToken.length() != 0) {
    serverPoint = serverPoint + dashToken + "/telemetry";
  }
  if (serverPoint.length() != 0) {
    String message = "{\n \"";
    if (type == 0) {
      message = message + "temperature";
    }
    if (type == 1) {
      message = message + "humidity";
    }

    message = message +  "\":";
    message = message + value;
    message = message +  "\n}";
    Serial.println("Server:\n" + serverPoint + "; \nMessage:\n" + message);

    HTTPClient http;
    http.begin(serverPoint);
    //    http.addHeader("Accept-Encoding", "gzip, deflate");
    http.addHeader("Content-Type", "application/json");
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
    delay(1000);
    http.end();
  } else {
    Serial.println("Need to set server and module type via SSID: " + tmpOwnSsid);
  }
}
