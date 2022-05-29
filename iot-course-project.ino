#include <WiFi.h>          //https://github.com/esp8266/Arduino

#include <WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <ESPmDNS.h>

#include <Preferences.h>

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

void loop() {
  server.handleClient();
}
