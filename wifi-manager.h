#include <WiFi.h>                //https://github.com/esp8266/Arduino
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

namespace Constants {
  const char AP_SSID[] = "esp32AP";
  const char AP_PASS[] = "testpass";
}

void wifiManager_setup(
  void (*apCb)(const String&, const String&),
  void (*reconnnectCb)(),
  void (*connectedCb)(const String&)) {

  WiFiManager wifiManager;
  //wifiManager.resetSettings();

  wifiManager.setAPCallback(
    [apCb](WiFiManager* wifiManager) {
      apCb(Constants::AP_SSID, Constants::AP_PASS);
    }
  );

  reconnnectCb();

  Serial.println("wifiManager_setup|attempt to connect...");

  wifiManager.autoConnect(Constants::AP_SSID, Constants::AP_PASS);
  //wifiManager.autoConnect();

  Serial.println("wifiManager_setup|connected...yeey :)");
  Serial.println(String("wifiManager_setup|SSID: ") + WiFi.SSID());
  Serial.println(String("wifiManager_setup|IP: ") + WiFi.localIP());

  connectedCb(WiFi.SSID());
}
