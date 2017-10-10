#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "FS.h"

#define RELAYPIN            2


int status = WL_IDLE_STATUS;
int statusAP = WL_IDLE_STATUS;
String ownSsid = "SmartHome";
String tmpOwnSsid = "";
String ownPassword = "SmartHome";
String ownIP = "";
String networkSsid = "";
String networkUser = "";
String networkPassword = "";
String networkIP = "";
String moduleType = "ap";
String dashAddress = "";
String dashToken = "SmartHome";
//String suffix;
boolean apUpFlag = true;
int netExceptionCounter = 0;
int netConnectExceptionCounter = 0;
boolean isAttributesSet = false;
boolean isSavedParams = false;

ESP8266WebServer server(80);
String webPage = "";

void setup() {
  Serial.begin(9600);
  Serial.println();
  pinMode(RELAYPIN, OUTPUT);

  tmpOwnSsid = ownSsid;
  Serial.setDebugOutput(true);

  disableAccessPoint();
  disconnectFromWifi();

  //configureWebPage
  configureWebPage();

  //  create AP
  createAccessPoint();
  loadConfig();
}

void loop() {
  status = WiFi.status();
  postModuleAttributes();
  if (status != WL_CONNECTED)  {
    connectToWifi(networkSsid, networkPassword);
  }
  if (statusAP == 0 && apUpFlag == true) {
    createAccessPoint();
    postModuleAttributes();
  }
  if (statusAP == 1 && apUpFlag == false) {
    disableAccessPoint();
    postModuleAttributes();
  }
  server.handleClient();
}

//Network configuration

void initDefNetwork() {
  networkSsid = "HOME"; //default SSID
  networkPassword = "ARjOwI23"; //default password
  moduleType = "tempHumid"; //default Type
  dashAddress = "171.25.167.194:8082"; //default Address
  dashToken = "SmartHome"; //default Token
}

void resetSettings() {
  ownSsid = "SmartHome";
  tmpOwnSsid = "";
  ownPassword = "SmartHome";
  ownIP = "";
  networkSsid = "";
  networkUser = "";
  networkPassword = "";
  networkIP = "";
  moduleType = "ap";
  dashAddress = "";
  dashToken = "SmartHome";
}

void createAccessPoint() {
  if (apUpFlag == true) {
    disableAccessPoint();
    Serial.print("Configuring access point for wifi network: ");
    tmpOwnSsid = getTokenValue();
    char ownSsid_char[ownSsid.length() + 1];
    tmpOwnSsid.toCharArray(ownSsid_char, tmpOwnSsid.length() + 1);
    char ownPassword_char[ownPassword.length() + 1];
    ownPassword.toCharArray(ownPassword_char, ownPassword.length() + 1);
    WiFi.softAP(ownSsid_char, ownPassword_char);
    statusAP = 1;
    Serial.print("ESP AccessPoint IP address: 192.168.4.1; Status: ");
    Serial.println(statusAP);
    ownIP = String(WiFi.softAPIP()[0]) + "." + String(WiFi.softAPIP()[1]) + "." + String(WiFi.softAPIP()[2]) + "." + String(WiFi.softAPIP()[3]);
    startServer();
  }
}

void disableAccessPoint() {
  Serial.print("Disabling access point: ");
  Serial.println(tmpOwnSsid);
  WiFi.softAPdisconnect(true);
  statusAP = 0;
  isAttributesSet = false;
  Serial.println("ESP AccessPoint disabled");
}

void configureWebPage() {
  webPage = "<html>\n"
            "    <body>\n"
            "        <FORM name=\"module\" action=\"/\" method=\"post\">\n"
            "            Network params:<br>\n"
            "            <p> <INPUT type=\"text\" name=\"ssid\" required value=\"";
  webPage = webPage + networkSsid;
  webPage += "\"> ssid<BR></p>\n"
             //             "            <p> <INPUT type=\"text\" name=\"user\" value=\"";
             //  webPage = webPage + networkUser;
             //  webPage += "\"> user<BR></p>\n"
             "            <p><INPUT type=\"text\" name=\"password\" required value=\"";
  webPage = webPage + networkPassword;
  webPage += "\"> password<BR></p>\n"
             "            <p><INPUT type=\"text\" name=\"ip\" required value=\"";
  webPage = webPage + networkIP;
  webPage += "\"> ip<BR></p>\n"
             "            Module params:<br>\n<p>Module type: ";
  webPage = webPage + moduleType;
  webPage += "</p>\n"
             "            <p><input type=\"radio\" name=\"module\" value=\"relay\" required>relay<Br>\n"
             "                <input type=\"radio\" name=\"module\" value=\"ap\">access point</p>\n"
             "            <p> <INPUT type=\"text\" name=\"server\" value=\"";
  webPage = webPage + dashAddress;
  webPage += "\"> server address; format: www.google.com:80 or 192.168.1.1:8080<BR></p>\n"
             //             "            <p> <INPUT type=\"text\" name=\"suffix\" value=\"";
             //  webPage = webPage + suffix;
             //  webPage += "\"> module suffix<BR></p>\n"
             "            <p><INPUT type=\"text\" name=\"token\" required value=\"";
  webPage = webPage + dashToken;
  webPage += "\"> dashboard token<BR></p>\n"
             "            <p>Module AP status: " ;
  webPage = webPage +  (apUpFlag == true ? "up" : "down") ;
  webPage += "</p>\n"
             "                <input type=\"radio\" name=\"apUpFlag\" value=\"0\">down<Br>\n"
             "                <input type=\"radio\" name=\"apUpFlag\" value=\"1\">up<Br>\n"
             "            <p><INPUT type =\"submit\" value=\"update\"></p>\n"
             "        </FORM>\n"
             "    </body>\n"
             "</html>";
}

void startServer() {
  server.on("/default", []() {
    Serial.println("Set default network values");
    initDefNetwork();
    configureWebPage();
    server.send(200, "text/html", webPage);
    delay(1000);
  });
  server.on("/reset", []() {
    Serial.println("Settings reset");
    resetSettings();
    saveConfig();
    configureWebPage();
    server.send(200, "text/html", webPage);
    delay(1000);
  });
  server.on("/save_config", []() {
    Serial.println("saveConfig");
    saveConfig();
    configureWebPage();
    server.send(200, "text/html", webPage);
    delay(1000);
  });
  server.on("/load_config", []() {
    Serial.println("loadConfig");
    loadConfig();
    configureWebPage();
    server.send(200, "text/html", webPage);
    delay(1000);
  });
  server.on("/action", []() {
    Serial.println("Action POST");
    ActivateAlarm();
    server.send(200, "text/html", "Action POST");
    delay(1000);
  });
  server.on("/activate", []() {
    Serial.println("Relay activated");
    ActivateAlarm();
    server.send(200, "text/html", "Relay activated");
    delay(1000);
  });
  server.on("/deactivate", []() {
    Serial.println("Relay deactivated");
    DeactivateAlarm();
    server.send(200, "text/html", "Relay deactivated");
    delay(1000);
  });
  server.on("/", []() {
    Serial.println("Get request params");
    for (int i = 0; i <   server.args(); i++ ) {
      Serial.println(server.argName(i) + ":" + server.arg(i));
    }
    String tmpNetworkSsid = server.arg("ssid").length() > 0 ? server.arg("ssid") : networkSsid;
    String tmpNetworkUser = server.arg("user").length() > 0 ? server.arg("user") : networkUser;
    String tmpNetworkPassword = server.arg("password").length() > 0 ? server.arg("password") : networkPassword;
    String tmpNetworkIP = server.arg("ip").length() > 0 ? server.arg("ip") : networkIP;
    String tmpDashToken = server.arg("token").length() > 0 ? server.arg("token") : dashToken;
    String tmpModuleType = server.arg("module").length() > 0 ? server.arg("module") : moduleType;
    String tmpDashAddress = server.arg("server").length() > 0 ? server.arg("server") : dashAddress;
    //    String tmpSuffix = server.arg("suffix").length() > 0 ? server.arg("suffix") : suffix;
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
    if (tmpNetworkIP.length() != 0) {
      networkIP = tmpNetworkIP;
    }
    if (tmpDashToken.length() != 0 && tmpDashToken != "SmartHome") {
      dashToken = tmpDashToken;
    } else {
      dashToken = ownSsid;
    }
    if (tmpModuleType.length() != 0) {
      if (tmpModuleType != moduleType) {
        moduleType = tmpModuleType;
        createAccessPoint();
      }
      if (moduleType != "ap") {
        if (tmpDashAddress.length() != 0) {
          dashAddress = tmpDashAddress;
        }
      }
    }
    if (tmpDashAddress.length() != 0) {
      dashAddress = tmpDashAddress;
    }
    configureWebPage();
    String result = webPage;
    result = result + "<p>ESP AP status: " ;
    result = result + (apUpFlag == true ? "up" : "down") + "</p>";
    result = result + "<p>SSID: " + networkSsid + "</p>";
    result = result + "<p>User name: " + networkUser + "</p>";
    result = result + "<p>User password: " + networkPassword + "</p>";
    result = result + "<p>Dashboard token: " + dashToken + "</p>";
    result = result + "<p>Current module set to: " + moduleType + "</p>";
    result = result + "<p>WiFi own SSID : " + getTokenValue() + "</p>";
    result = result + "<p>Dashboard address set to: " + dashAddress + "</p>";
    server.send(200, "text/html", result);
    delay(1000);
  });
  server.begin();
  Serial.println("HTTP server started");
}

void connectToWifi(String ssid,  String password) {
  if (status != WL_CONNECTED && ssid.length() != 0 ) {
    Serial.printf("Network connecting exception counter: % d\n", netConnectExceptionCounter);
    if ( netConnectExceptionCounter < 10) {
      disconnectFromWifi();
      char ssid_char[ssid.length() + 1];
      ssid.toCharArray(ssid_char, ssid.length() + 1);
      Serial.print("Attempting to connect to SSID: ");
      Serial.print(ssid_char);
      if (password.length() != 0) {
        char password_char[password.length() + 1];
        password.toCharArray(password_char, password.length() + 1);
        Serial.print(" with password: ");
        Serial.println(password_char);
        status = WiFi.begin(ssid_char, password_char);
      } else {
        status = WiFi.begin(ssid_char);
      }
      int counter = 0;
      while (status != WL_CONNECTED && counter < 10)
      {
        delay(1000);
        status = WiFi.status();
        counter++;
      }
      if (status == WL_CONNECTED) {
        if (networkIP.length() > 0) {
          WiFi.config(string2IP(networkIP), WiFi.gatewayIP(), WiFi.subnetMask());
        }
        networkIP =  String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(WiFi.localIP()[3]);
        Serial.println("NodeMCU connected to the network: " + ssid + " with password: " + ownPassword);
        netConnectExceptionCounter = 0;
      } else {
        Serial.println("NodeMCU NOT connected to the network");
        netConnectExceptionCounter = netConnectExceptionCounter + 1;
        disconnectFromWifi();
      }
    } else {
      Serial.println("Network connecting exception counter exceed limit. Check network SSID and password. Sleep for 1 minute");
      netConnectExceptionCounter = 0;
      delay(60000);
      //            networkSsid = "";
      //            networkPassword = "";
    }
  }
}

void disconnectFromWifi() {
  isAttributesSet = false;
  status = WiFi.disconnect();
  isSavedParams = false;
  Serial.println("Disconnected form wifi");
}

void postModuleAttributes() {
  postAttribute("OwnNetworkSSID", tmpOwnSsid);
  postAttribute("OwnNetworkPassord", ownPassword);
  postAttribute("OwnNetworkStatus", String(apUpFlag));
  postAttribute("OwnIP", "http://" + ownIP + "/");
  postAttribute("InternetSSID", networkSsid);
  postAttribute("InternetUser", networkUser);
  postAttribute("InternetPassword", networkPassword);
  postAttribute("InternetIP", networkIP);
  postAttribute("InternetStatus", String(status));
  postAttribute("ModuleType", moduleType);
  postAttribute("ThingsBoardAddress", "http://" + dashAddress + "/");
  postAttribute("ThingsBoardToken", getTokenValue());
  isAttributesSet = true;
}

void ActivateAlarm() {
  digitalWrite(RELAYPIN, HIGH);
  isAttributesSet = false;
  postAttribute("AlarmStatus", "activated");
  isAttributesSet = true;
}

void DeactivateAlarm() {
  digitalWrite(RELAYPIN, LOW);
  isAttributesSet = false;
  postAttribute("AlarmStatus", "deactivated");
  isAttributesSet = true;
}

void postAttribute(String type, String value) {
  if (status == WL_CONNECTED && isAttributesSet == false && netExceptionCounter < 3) {
    String serverPoint;
    if (dashAddress.length() != 0) {
      serverPoint = "http://" + dashAddress + "/api/v1/";
    }
    if (dashToken.length() != 0) {
      serverPoint = serverPoint + getTokenValue() + "/attributes";
    }
    if (serverPoint.length() != 0) {
      String message = "{\n \"";
      message = message + type;
      message = message +  "\":\"";
      message = message + value;
      message = message +  "\"\n}";
      Serial.println("Server: " + serverPoint + "; \nMessage: " + message);
      sendPost(message, serverPoint);
    } else {
      Serial.println("Need to set server and module type via SSID: " + tmpOwnSsid);
    }
  }
}

void sendPost(String message, String serverPoint) {
  if (netExceptionCounter < 20) {
    HTTPClient http;
    http.begin(serverPoint);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(message);
    if (httpCode > 0) {
      Serial.printf("[HTTP] POST... code: % d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
        netExceptionCounter = 0;
        if (isSavedParams == false) {
          saveConfig();
        }
        status = WiFi.status();
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: % s\n", http.errorToString(httpCode).c_str());
      netExceptionCounter = netExceptionCounter + 1 ;
      status = WiFi.status();
    }
    delay(200);
    http.end();
  } else {
    Serial.println("Address exception counter exceed limit. Check address and token. Sleep for 1 minute.");
    netExceptionCounter = 0;
    delay(60000);
    //    dashAddress = "";
    //    dashToken = ownSsid;
    //    ESP.restart();
  }
}

String getTokenValue() {
  Serial.print("token -> ");
  String value;
  if (moduleType.length() != 0 && moduleType != "ap") {
    value = "SH(" + moduleType + ")";
    if (dashToken.length() != 0 && dashToken != "SmartHome") {
      value = value + dashToken;
    }
  } else {
    value = ownSsid;
  }
  Serial.println(value);
  return value;
}


IPAddress string2IP(String strIP) {
  int Parts[4] = {0, 0, 0, 0};
  int Part = 0;

  for ( int i = 0; i < strIP.length(); i++ )
  {
    char c = strIP[i];
    if ( c == '.' )
    {
      Part++;
      continue;
    }
    Parts[Part] *= 10;
    Parts[Part] += c - '0';
  }
  IPAddress ip(Parts[0], Parts[1], Parts[2], Parts[3]);
  return  ip;
}

//Store values

boolean saveConfig() {
  String value = prepareConfig();
  Serial.println("Save config");
  Serial.println(value);
  isSavedParams = true;
  return fileWrite("/config.json", value);
}

String loadConfig() {
  String content = fileRead("/config.json");
  char value[content.length() + 1];
  content.toCharArray(value, content.length() + 1);
  StaticJsonBuffer<300> JSONBuffer;
  JsonObject&  parsed = JSONBuffer.parseObject(value);
  if (!parsed.success()) {   //Check for errors in parsing
    Serial.println("Parsing failed");
    delay(5000);
    return "";
  }
  const char * CtmpOwnSsid = parsed["OwnNetworkSSID"];
  tmpOwnSsid = CtmpOwnSsid;
  const char * CownPassword = parsed["OwnNetworkPassord"];
  ownPassword = CownPassword;
  const char * CnetworkSsid = parsed["InternetSSID"];
  networkSsid = CnetworkSsid;
  const char * CnetworkUser = parsed["InternetUser"];
  networkUser = CnetworkUser;
  const char * CnetworkPassword = parsed["InternetPassword"];
  networkPassword = CnetworkPassword;
  const char * CnetworkIP = parsed["InternetIP"];
  networkIP = CnetworkIP;
  const char * CmoduleType = parsed["ModuleType"];
  moduleType = CmoduleType;
  const char * CdashAddress = parsed["ThingsBoardAddress"];
  dashAddress = CdashAddress;
  const char * CdashToken = parsed["ThingsBoardToken"];
  dashToken = CdashToken;
  Serial.println("Load config");
  Serial.println(prepareConfig());
  return prepareConfig();
}

String prepareConfig() {
  String result =  "{\"OwnNetworkSSID\":\"" + tmpOwnSsid;
  result = result + "\",\"OwnNetworkPassord\":\"" + ownPassword;
  result = result + "\",\"OwnNetworkStatus\":\"" + String(apUpFlag);
  result = result + "\",\"OwnIP\":\"" + ownIP;
  result = result + "\",\"InternetSSID\":\"" + networkSsid;
  result = result + "\",\"InternetUser\":\"" + networkUser;
  result = result + "\",\"InternetPassword\":\"" + networkPassword;
  result = result + "\",\"InternetIP\":\"" + networkIP;
  result = result + "\",\"ModuleType\":\"" + moduleType;
  result = result + "\",\"ThingsBoardAddress\":\"" + dashAddress ;
  result = result + "\",\"ThingsBoardToken\":\"" + dashToken;
  result = result + "\"}";
  return result;
}

boolean fileWrite(String name, String content) {
  SPIFFS.begin();
  File file = SPIFFS.open(name.c_str(), "w");
  if (!file) {
    String errorMessage = "Can't open '" + name + "' !\r\n";
    Serial.println(errorMessage);
    return false;
  } else {
    file.write((uint8_t *)content.c_str(), content.length());
    file.close();
    return true;
  }
}

String fileRead(String name) {
  SPIFFS.begin();
  String contents;
  File file = SPIFFS.open(name.c_str(), "r");
  if (!file) {
    String errorMessage = "Can't open '" + name + "' !\r\n";
    Serial.println(errorMessage);
    return "FILE ERROR";
  }
  else {
    int fileSize = file.size();
    int chunkSize = 1024;
    char buf[chunkSize];
    int numberOfChunks = (fileSize / chunkSize) + 1;
    int remainingChunks = fileSize;
    for (int i = 1; i <= numberOfChunks; i++) {
      if (remainingChunks - chunkSize < 0) {
        chunkSize = remainingChunks;
      }
      file.read((uint8_t *)buf, chunkSize);
      remainingChunks = remainingChunks - chunkSize;
      contents += String(buf);
    }
    file.close();
    return contents;
  }
}


//void scanAndConnectToNetwork() {
//  uint8_t encryptionType;
//  int32_t RSSI;
//  uint8_t* BSSID;
//  int32_t channel;
//  bool isHidden;
//  int n = WiFi.scanNetworks();
//  String ssid;
//
//  for (int i = 0; i < n; i++)  {
//    WiFi.getNetworkInfo(i, ssid, encryptionType, RSSI, BSSID, channel, isHidden);
//    //    Serial.printf("%d: %s, Ch:%d (%ddBm) %s %s\n", i + 1, ssid.c_str(), channel, RSSI, encryptionType == ENC_TYPE_NONE ? "open" : "", isHidden ? "hidden" : "");
//
//    //connect to open Wifi;
//    if (status != WL_CONNECTED)    {
//      if (encryptionType == ENC_TYPE_NONE)      {
//        connectToWifi(ssid, "");
//      }
//    }
//  }
//  WiFi.scanDelete();
//}
