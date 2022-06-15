#include <HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

#include <TimeLib.h>
#include <ArduinoJson.h>
#include <Vector.h>

namespace Constants {
  const char CERT[] = \
  "-----BEGIN CERTIFICATE-----\n" \
  "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\n" \
  "-----END CERTIFICATE-----\n";

  const char TIME_FORMAT[] = "%d-%d-%d %d:%d:%d";
  const char TIME_FORMAT_TIME_ONLY[] = "%d:%d:%d";

  const char API_ENDPOINT[] = "https://cukii.me/sumc/api/timing/";
}

struct Line {
  String name;
  time_t nextArrival;
};

struct StopInfo {
  Vector<Line> lines;
  time_t calculatedTime;
  bool isValid;
};

Line LINES[100];

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
    Serial.println("httpsGET|[HTTPS] begin...");
    if (https.begin(*client, address)) {  // HTTPS
      Serial.println("httpsGET|[HTTPS] GET...");
      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.println(String("httpsGET|[HTTPS] GET... code: ") + httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          result = https.getString();
          return result;
        }
      } else {
        Serial.println("httpsGET|[HTTPS] GET... failed, error: " + https.errorToString(httpCode));
      }

      https.end();
    } else {
      Serial.println("httpsGET|[HTTPS] Unable to connect");
    }
    // End extra scoping block
  }
  
  delete client;
  
  return result;
}

tmElements_t buildTime(int year, int month, int day, int h, int m, int s) {
  return {s, m, h, 0, day, month, CalendarYrToTm(year)};
}

tmElements_t parseTime(const char* timeStr) {
  int year, month, day, hour, minute, second;
  sscanf(timeStr, Constants::TIME_FORMAT, &year, &month, &day, &hour, &minute, &second);
  return buildTime(year, month, day, hour, minute, second);
}

tmElements_t parseTime(const char* timeStr, time_t relative) {
  int hour, minute, second;
  sscanf(timeStr, Constants::TIME_FORMAT_TIME_ONLY, &hour, &minute, &second);
  return buildTime(year(relative), month(relative), day(relative), hour, minute, second);
}

StopInfo deserializeApiOutput(const String& data) {
  Serial.println("deserializeApiOutput|Deserializing data...");
  DynamicJsonDocument doc(16384);
  DeserializationError error = deserializeJson(doc, data);

  if (error) {
    Serial.print("deserializeApiOutput|deserializeJson() failed: ");
    Serial.println(error.c_str());
    return {};
  }

  const char* code = doc["code"]; // "1194"
  const char* name = doc["name"]; // "УМБАЛ СВ. АННА"

  const char* timestamp_calculated = doc["timestamp_calculated"]; // "2022-05-30 23:45:22"
  Serial.println(String("deserializeApiOutput|timestamp_calculated: ") + timestamp_calculated);
  time_t calculatedTime = timestamp_calculated
    ? makeTime(parseTime(timestamp_calculated))
    : 0;

  const JsonArray jlines = doc["lines"];
  Serial.println(String("deserializeApiOutput|JLines count: ") + jlines.size());

  Vector<Line> lines(LINES);
  Serial.println(String("deserializeApiOutput|Vector: ") + lines.size() + " " + lines.max_size());

  for (const auto& line : jlines) {
    String name = line["name"];
    const char* time = line["arrivals"].as<JsonArray>()[0]["time"];
    Serial.println(String("deserializeApiOutput|time: ") + time);

    if (!time) {
      continue;
    }
    time_t nextArrival = makeTime(parseTime(time, calculatedTime));

    Serial.println(String("deserializeApiOutput|Line name: ") + name);
    Serial.println(String("deserializeApiOutput|Line next arrival: ") + nextArrival);

    lines.push_back({name, nextArrival});
  }

  Serial.println(String("deserializeApiOutput|Calculated time: ") + calculatedTime);
  Serial.println(String("deserializeApiOutput|Lines count: ") + lines.size());

  return {lines, calculatedTime, true};
}

StopInfo getStopInfo(const String& stopId) {
  String address(Constants::API_ENDPOINT + stopId);
  String output = httpsGET(address.c_str(), Constants::CERT);

  return deserializeApiOutput(output);
}
