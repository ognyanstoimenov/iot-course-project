#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#include <WiFi.h>                //https://github.com/esp8266/Arduino
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <WebServer.h>
#include <ESPmDNS.h>

#include <Preferences.h>

#include <HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

#include <ArduinoJson.h>
#include <TimeLib.h>

namespace Constants {
  const char AP_SSID[]      =   "esp32AP";
  const char AP_PASS[]      =   "testpass";
  const char MDNS_DOMAIN[]  =   "esp32";

  const int LCD_COLS        =   16;
  const int LCD_ROWS        =   2;

  const char CERT[] = \
  "-----BEGIN CERTIFICATE-----\n" \
  "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\n" \
  "-----END CERTIFICATE-----\n";
  
  const char API_ENDPOINT[] = "https://cukii.me/sumc/api/timing/";
  
  const char TIME_FORMAT[] = "%d-%d-%d %d:%d:%d";
  const char TIME_FORMAT_TIME_ONLY[] = "%d:%d:%d";

  const int FETCH_INTERVAL_SECONDS = 15;

  const int ARRIVAL_INFO_DURATION_SECONDS = 5;
}

int CurrentYear = 1970;
int CurrentMonth = 1;
int CurrentDay = 1;

struct BusStopInfo {
  JsonArray lines;
  const char* calculatedTime;
};

String httpsGET(const char* address, const char* certificate) {
  WiFiClient* client;
  String result = "";
  if (certificate) {
    client = new WiFiClientSecure;
    ((WiFiClientSecure*)client)->setCACert(certificate);
  } else {
    client = new WiFiClient;
  }
  {
    // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
    HTTPClient https;
    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, address)) {  // HTTPS
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
  
  return result;
}

time_t deserializeTime(const String& data) {
  StaticJsonDocument<768> doc;

  DeserializationError error = deserializeJson(doc, data);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return 0;
  }

  return doc["unixtime"];
}

//time_t getCurrentTime() {
// return deserializeTime(httpsGET("http://worldtimeapi.org/api/timezone/Europe/Sofia", NULL));
//}

LiquidCrystal_PCF8574 lcd(0x27);
WebServer server(80);
Preferences preferences;

String stopCode = "";
bool stopCodeChanged = true;

void setup() {
  Serial.begin(115200);

  Wire.begin(16, 17);
  lcd.begin(Constants::LCD_COLS, Constants::LCD_ROWS);
  lcd.setBacklight(255);

  WiFiManager wifiManager;
  //wifiManager.resetSettings();

  wifiManager.setAPCallback([](WiFiManager* wifiManager){
    lcd.clear();
    lcd.print(String("SSID: ") + Constants::AP_SSID);
    lcd.setCursor(0, 1);
    lcd.print(String("Pass: ") + Constants::AP_PASS);
  });

  lcd.clear();
  lcd.print("Connecting...");

  wifiManager.autoConnect(Constants::AP_SSID, Constants::AP_PASS);
  //wifiManager.autoConnect();

  Serial.println("connected...yeey :)");
  Serial.println(String("SSID: ") + WiFi.SSID());
  Serial.println(String("IP: ") + WiFi.localIP());

  lcd.clear();
  lcd.print("Connected to:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.SSID().substring(0, Constants::LCD_COLS - 1));
  delay(1000);
//  
//  time_t currentTime = getCurrentTime();
//  setTime(currentTime);

  if (MDNS.begin(Constants::MDNS_DOMAIN)) {
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

BusStopInfo deserializeApiOutput(const String& data) {
  DynamicJsonDocument doc(8192);
  DeserializationError error = deserializeJson(doc, data);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return { JsonArray(), "" };
  }
  
  const char* timestamp_calculated = doc["timestamp_calculated"]; // "2022-05-30 23:45:22"
  const char* code = doc["code"]; // "1194"
  const char* name = doc["name"]; // "УМБАЛ СВ. АННА"

  const JsonArray lines = doc["lines"];
  // const JsonObject line = lines[i];
  // const JsonArray arrivals = line["arrivals"].as<JsonArray>()
  // arrivals[j]["time"] -> char*
  


  return {lines, timestamp_calculated};
}


void setCurrentDate(tmElements_t elements) {
  CurrentYear = elements.Year;
  CurrentMonth = elements.Month;
  CurrentDay = elements.Day;
  
  setTime(makeTime(elements));
}

tmElements_t parseStringToTime(const char* timeStr, bool parseYearMonthDay) {
  Serial.println(String("Parsing time string: ") + timeStr);
  tmElements_t tm;
  int Year, Month, Day, Hour, Minute, Second;
  if (parseYearMonthDay) {
    sscanf(timeStr, Constants::TIME_FORMAT, &Year, &Month, &Day, &Hour, &Minute, &Second);
    tm.Year = CalendarYrToTm(Year);
    tm.Month = Month;
    tm.Day = Day;
  }
  else {
    sscanf(timeStr, Constants::TIME_FORMAT_TIME_ONLY, &Hour, &Minute, &Second);
    tm.Year = CurrentYear;
    tm.Month = CurrentMonth;
    tm.Day = CurrentDay;
  }
  tm.Hour = Hour;
  tm.Minute = Minute;
  tm.Second = Second;

  return tm; 
}

BusStopInfo getBusStopInfo(const String& busStopId) {
  String address(Constants::API_ENDPOINT + busStopId);
  String output = httpsGET(address.c_str(), Constants::CERT);
  
  return deserializeApiOutput(output);
}

time_t lastFetch = 1;
bool shouldFetch = true;
JsonArray lines = JsonArray();
void loop() {
  server.handleClient();
  
  String savedStopCode = preferences.getString("stopCode", "");
  if (stopCode != savedStopCode) {
    stopCode = savedStopCode;
    stopCodeChanged = true;
  }

  shouldFetch = (now() - lastFetch) > Constants::FETCH_INTERVAL_SECONDS;
  Serial.println(String("now: ") + now());
  Serial.println(String("lastFetch: ") + lastFetch);
  Serial.println(String("shouldFetch: ") + (shouldFetch? "true" : "false"));
  if (stopCodeChanged) {
    stopCodeChanged = false;
    lastFetch = 0;
    if (stopCode.length() == 0) {
      lcd.clear();
      lcd.print("Browse URL:");
      lcd.setCursor(0, 1);
      lcd.print(String(Constants::MDNS_DOMAIN) + ".local");
    } else {
      lcd.clear();
      lcd.print("Watching stop:");
      lcd.setCursor(0, 1);
      lcd.print(stopCode.substring(0, Constants::LCD_COLS - 1));
    }
  } else {
    if (shouldFetch) {
      Serial.println(String("Fetching info for stop: ") + savedStopCode);
      BusStopInfo busStopInfo = getBusStopInfo(savedStopCode);
      auto timeToSet = parseStringToTime(busStopInfo.calculatedTime, true);
      lines = busStopInfo.lines;
      setCurrentDate(timeToSet);
      
      lastFetch = now();
    }
    if (lines.size() == 0) {
      lcd.clear();
      lcd.print("No stop info.");
      delay(10000);
    } else {
      for (const auto& line : lines) {
        if (line["arrivals"].size() == 0) {
          continue;
        }
        const char* arrival = line["arrivals"].as<JsonArray>()[0]["time"];
        Serial.println(String("arrivalStr: ") + arrival);
        auto firstArrivingTime = makeTime(parseStringToTime(arrival, false));
        lcd.clear();
        unsigned long minutesToNext = (firstArrivingTime - now())/60;
        const char* lineName = line["name"];
        lcd.print(lineName);
        lcd.setCursor(0,1);
        lcd.print(String("Sled ") + minutesToNext + " minuti");
        delay(Constants::ARRIVAL_INFO_DURATION_SECONDS * 1000);
      }
    }
    // TODO: fetch timetables if now - last_fetch > FETCH_INTERVAL
    // TODO: show line arrival times
    // TODO: cycle through lines when more than two
  }

}
