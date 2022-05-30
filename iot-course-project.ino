#include <WiFi.h>          //https://github.com/esp8266/Arduino

#include <WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <TimeLib.h>

namespace Constants {
  const char[] CERT = \
  "-----BEGIN CERTIFICATE-----\n" \
  "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\n" \
  "-----END CERTIFICATE-----\n";
  
  const char[] API_ENDPOINT = "https://cukii.me/sumc/api/timing/";
  
  const char[] TIME_FORMAT = "%d-%d-%d %d:%d:%d";
}
WebServer server(80);
Preferences preferences;

void setup() {
  Serial.begin(115200);

  WiFiManager wifiManager;
  //wifiManager.resetSettings();

  wifiManager.autoConnect("AutoConnectAP1", "testpass");
  //wifiManager.autoConnect();

  Serial.println("connected...yeey :)");

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  preferences.begin("app", false);

  server.on("/", HTTP_GET, [](){

    String stopCode = preferences.getString("stopCode", "");

    String html = "";
    html += "<!DOCTYPE html><html lang='en'>";
    html += "<head>";
    html += "<meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width,initial-scale=1,user-scalable=no' />";
    html += "</head>";
    html += "<body>";
    html += "<h1>Stop configuration</h1>";
    html += "<p>";
    html += "Configured stop: ";
    html += stopCode;
    html += "</p>";
    html += "<form method='POST'>";
    html += "<label>";
    html += "Enter stop code: ";
    html += "<input type='text' name='stop-code'></input>";
    html += "<input type='submit' value='Save'>";
    html += "</label>";
    html += "</form>";
    html += "</body>";
    html += "</html>";

    server.send(200, "text/html", html);
  });

  server.on("/", HTTP_POST, [](){
    String code = server.arg("stop-code");

    preferences.putString("stopCode", code);

    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.begin();
  Serial.println("HTTP server started");
}


JsonArray deserializeApiOutput(const String& data) {
  DynamicJsonDocument doc(3072);
  DeserializationError error = deserializeJson(doc, data);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return JsonArray();
  }
  
  const char* timestamp_calculated = doc["timestamp_calculated"]; // "2022-05-30 23:45:22"
  const char* code = doc["code"]; // "1194"
  const char* name = doc["name"]; // "УМБАЛ СВ. АННА"

  const JsonArray lines = doc["lines"];
  // const JsonObject line = lines[i];
  // const JsonArray arrivals = line["arrivals"].as<JsonArray>()
  // arrivals[j]["time"] -> char*
  return lines;
}

tmElements_t parseStringToTime(const char* timeStr) {
  tmElements_t tm;
  int Year, Month, Day, Hour, Minute, Second;
  sscanf(timeStr, "%d-%d-%d %d:%d:%d", &Year, &Month, &Day, &Hour, &Minute, &Second);
  tm.Year = CalendarYrToTm(Year);
  tm.Month = Month;
  tm.Day = Day;
  tm.Hour = Hour;
  tm.Minute = Minute;
  tm.Second = Second;

  return tm; 
}

JsonArray getBusStopInfo(int busStopId) {
  String address(API_ENDPOINT + busStopId);
  String output = httpsGET(address.c_str(), CERT);

  return deserializeApiOutput(output);
}

String httpsGET(const char* address, const char* certificate) {
  WiFiClientSecure *client = new WiFiClientSecure;
  String result = "";
  if(client) {
    client->setCACert(certificate);

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https;
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, certificate)) {  // HTTPS
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
  
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            result = https.getString();
            return result;
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
      // End extra scoping block
    }
  
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
  return result;
}

void loop() {
  server.handleClient();
  
  delay(1000000);
}
