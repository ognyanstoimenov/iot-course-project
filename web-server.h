#include <WebServer.h>
#include <ESPmDNS.h>

namespace Constants {
  const char MDNS_DOMAIN[] = "esp32";
  const int SERVER_PORT = 80;
}

WebServer server(Constants::SERVER_PORT);

void webServer_setup(
  String (*getStopCode)(),
  void (*setStopCode)(const String&)) {

  if (MDNS.begin(Constants::MDNS_DOMAIN)) {
    Serial.println("webServer_setup|MDNS responder started");
  }

  server.on("/", HTTP_GET, [getStopCode](){

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
    html += getStopCode();
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

  server.on("/", HTTP_POST, [setStopCode](){
    String code = server.arg("stop-code");

    setStopCode(code);

    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.begin();
  Serial.println(String("webServer_setup|HTTP server started on ") + Constants::SERVER_PORT);
}

void server_loop() {
  server.handleClient();
}
