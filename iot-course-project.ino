#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#include <WiFi.h>                //https://github.com/esp8266/Arduino
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <WebServer.h>
#include <ESPmDNS.h>

#include <Preferences.h>

const char AP_SSID[]      =   "esp32AP";
const char AP_PASS[]      =   "testpass";
const char MDNS_DOMAIN[]  =   "esp32";

const int LCD_COLS        =   16;
const int LCD_ROWS        =   2;

LiquidCrystal_PCF8574 lcd(0x27);
WebServer server(80);
Preferences preferences;

String stopCode = "";
bool stopCodeChanged = true;

void setup() {
  Serial.begin(115200);

  Wire.begin(16, 17);
  lcd.begin(LCD_COLS, LCD_ROWS);
  lcd.setBacklight(255);

  WiFiManager wifiManager;
  //wifiManager.resetSettings();

  wifiManager.setAPCallback([](WiFiManager* wifiManager){
    lcd.clear();
    lcd.print(String("SSID: ") + AP_SSID);
    lcd.setCursor(0, 1);
    lcd.print(String("Pass: ") + AP_PASS);
  });

  lcd.clear();
  lcd.print("Connecting...");

  wifiManager.autoConnect(AP_SSID, AP_PASS);
  //wifiManager.autoConnect();

  Serial.println("connected...yeey :)");
  Serial.println(String("SSID: ") + WiFi.SSID());
  Serial.println(String("IP: ") + WiFi.localIP());

  lcd.clear();
  lcd.print("Connected to:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.SSID().substring(0, LCD_COLS - 1));
  delay(1000);

  if (MDNS.begin(MDNS_DOMAIN)) {
    Serial.println("MDNS responder started");
  }

  preferences.begin("app", false);
  //preferences.clear();

  server.on("/", HTTP_GET, [](){

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
    html += "</label>";
    html += "<input type='submit' value='Save'>";
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

  String savedStopCode = preferences.getString("stopCode", "");
  if (stopCode != savedStopCode) {
    stopCode = savedStopCode;
    stopCodeChanged = true;
  }

  if (stopCodeChanged) {
    stopCodeChanged = false;
    // TODO: reset last_fetch to -inf
    if (stopCode.length() == 0) {
      lcd.clear();
      lcd.print("Browse URL:");
      lcd.setCursor(0, 1);
      lcd.print(String(MDNS_DOMAIN) + ".local");
    } else {
      lcd.clear();
      lcd.print("Watching stop:");
      lcd.setCursor(0, 1);
      lcd.print(stopCode.substring(0, LCD_COLS - 1));
    }
  } else {
    // TODO: fetch timetables if now - last_fetch > FETCH_INTERVAL
    // TODO: show line arrival times
    // TODO: cycle through lines when more than two
  }

}
