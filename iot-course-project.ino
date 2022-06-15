#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#include <Preferences.h>

#include <TimeLib.h>

#include "sumc-api.h"
#include "web-server.h"
#include "wifi-manager.h"

namespace Constants {
  const char PREF_NS[] = "app";
  const char PREF_CODE[] = "stopCode";

  const int LCD_COLS = 16;
  const int LCD_ROWS = 2;

  const unsigned int FETCH_INTERVAL_SECONDS = 15;

  const unsigned int ARRIVAL_INFO_DURATION_SECONDS = 5;
}

LiquidCrystal_PCF8574 lcd(0x27);
Preferences preferences;
String stopCode = "";

void setup() {
  Serial.begin(115200);

  Wire.begin(16, 17);
  lcd.begin(Constants::LCD_COLS, Constants::LCD_ROWS);
  lcd.setBacklight(255);

  wifiManager_setup(
    [](const String& ssid, const String& pass) {
      lcd.clear();
      lcd.print(String("SSID: ") + ssid);
      lcd.setCursor(0, 1);
      lcd.print(String("Pass: ") + pass);
    },
    []() {
      lcd.clear();
      lcd.print("Connecting...");
    },
    [](const String& ssid) {
      lcd.clear();
      lcd.print("Connected to:");
      lcd.setCursor(0, 1);
      lcd.print(ssid.substring(0, Constants::LCD_COLS - 1));
      delay(1000);
    }
  );

  // Workaround to begin search on first loop()
  setTime(Constants::FETCH_INTERVAL_SECONDS + 1);

  preferences.begin(Constants::PREF_NS, false);
  //preferences.clear();

  webServer_setup(
    []() {
      return stopCode;
    },
    [](String code) {
      preferences.putString(Constants::PREF_CODE, code);
    }
  );
}

time_t lastFetch = 0;
time_t lastDraw = 0;
Vector<Line> lines;
int lineIndex = 0;
bool stopCodeChanged = true;

void loop() {
  server_loop();

  String savedStopCode = preferences.getString(Constants::PREF_CODE, "");
  if (stopCode != savedStopCode) {
    stopCode = savedStopCode;
    stopCodeChanged = true;
  }

  bool shouldFetch = (now() - lastFetch) > Constants::FETCH_INTERVAL_SECONDS;
  bool shouldRedraw = (now() - lastDraw) > Constants::ARRIVAL_INFO_DURATION_SECONDS;

  Serial.println(String("loop|now: ") + now());
  Serial.println(String("loop|lastFetch: ") + lastFetch);
  Serial.println(String("loop|shouldFetch: ") + shouldFetch);

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
      Serial.println(String("loop|Fetching info for stop: ") + savedStopCode);
      BusStopInfo busStopInfo = getBusStopInfo(savedStopCode);
      Serial.println(String("loop|busStopInfo.isValid: ") + busStopInfo.isValid);
      if (busStopInfo.isValid) {
        lines = busStopInfo.lines;
        lineIndex = 0;
        Serial.println(String("loop|Fetched lines: ") + lines.size());

        setTime(busStopInfo.calculatedTime);
        Serial.println(String("loop|Set current time to: ") + busStopInfo.calculatedTime);
      }
      lastFetch = now();
    }

    Serial.println(String("loop|Found ") + lines.size() + " lines");

    if (shouldRedraw && !lines.empty()) {
      Line line = lines[lineIndex];
      int minutesToNext = (line.nexArrival - now()) / 60;

      Serial.println(String("loop|Line name:") + line.name);
      Serial.println(String("loop|Now: ") + now());
      Serial.println(String("loop|Line nex arrival: ") + line.nexArrival);

      lcd.clear();
      lcd.print(line.name);
      lcd.setCursor(0, 1);
      lcd.print(String("Sled ") + minutesToNext + " minuti");

      lineIndex = (lineIndex + 1) % lines.size();
      lastDraw = now();
    }
  }

}
