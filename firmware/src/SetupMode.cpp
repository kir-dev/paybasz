#include <WiFi.h>
#include <WebServer.h>
#include <cstring>
#include "Firmware.h"

WebServer * server;

IPAddress local_ip(192, 168, 0, 69);
IPAddress gateway(192, 168, 0, 69);
IPAddress subnet(255, 255, 255, 0);

void handleIndexScreen() {
    server->send(200, "text/html",
            "<!DOCTYPE html> <html>\n"
            "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n"
            "<title>paybasz: Setup mode</title>\n"
            "<style>* { font-family: Helvetica } input { width: 98% }</style>\n"
            "</head> <body>\n"
            "<h1>SETUP MODE</h1>"
            "<form action=\"/change\" method=\"post\">"
            "<label for=\"ssid\">SSID:</label><br><input id=\"ssid\" type=\"text\" name=\"ssid\" maxlength=\"32\" value=\"" + String(permanentMemory->ssid) + "\"><br>"
            "<label for=\"password\">PASSWORD:</label><br><input id=\"password\" type=\"text\" name=\"password\" maxlength=\"64\" value=\"" + String(permanentMemory->wifiPassword) + "\"><br>"
            "<label for=\"name\">GATEWAY NAME:</label><br><input id=\"name\" type=\"text\" name=\"name\" maxlength=\"64\" value=\"" + String(permanentMemory->gatewayName) + "\"><br>"
            "<label for=\"token\">GATEWAY TOKEN:</label><br><input id=\"token\" type=\"text\" name=\"token\" maxlength=\"64\" value=\"" + String(permanentMemory->token) + "\"><br>"
            "<label for=\"base\">API BASE URL:</label><br><input id=\"base\" type=\"text\" name=\"base\" maxlength=\"128\" value=\"" + String(permanentMemory->baseUrl) + "\"><br><br>"
            "<input type=\"submit\" value=\"Save\">"
            "</form>\n"
            "</body></html>\n");
}

void handleChange() {
    if (server->method() != HTTP_POST) {
        server->send(405, F("text/plain"), F("Method Not Allowed"));
        return;
    }

    String message = F("Changed to:\n");

    permanentMemory->setSetup(false);
    if (server->arg(F("ssid")).length() < PermanentMemory::SSID_LENGTH) {
        std::strcpy(permanentMemory->ssid, server->arg(F("ssid")).c_str());
        message += " SSID: " + server->arg(F("ssid")) + "\n";
    }
    if (server->arg(F("password")).length() < PermanentMemory::WPASSWD_LENGTH) {
        std::strcpy(permanentMemory->wifiPassword, server->arg(F("password")).c_str());
        message += " PASSWORD: " + server->arg(F("password")) + "\n";
    }
    if (server->arg(F("name")).length() < PermanentMemory::GW_NAME_LENGTH) {
        std::strcpy(permanentMemory->gatewayName, server->arg(F("name")).c_str());
        message += " GATEWAY NAME: " + server->arg(F("name")) + "\n";
    }
    if (server->arg(F("token")).length() < PermanentMemory::TOKEN_LENGTH) {
        std::strcpy(permanentMemory->token, server->arg(F("token")).c_str());
        message += " GATEWAY TOKEN: " + server->arg(F("token")) + "\n";
    }
    if (server->arg(F("base")).length() < PermanentMemory::BASE_URL_LENGTH) {
        std::strcpy(permanentMemory->baseUrl, server->arg(F("base")).c_str());
        message += " API BASE URL: " + server->arg(F("base")) + "\n";
    }
    permanentMemory->save();
    Serial.println("[SETUP] Saved!");
    permanentMemory->load();
    Serial.println("[SETUP] Reloaded!");

    Serial.println("[SETUP] - SSID: " + String(permanentMemory->ssid));
    Serial.println("[SETUP] - PW: " + String(permanentMemory->wifiPassword));
    Serial.println("[SETUP] - NAME: " + String(permanentMemory->gatewayName));
    Serial.println("[SETUP] - TOKEN: " + String(permanentMemory->token));
    Serial.println("[SETUP] - BASE URL: " + String(permanentMemory->baseUrl));
    message += "and then saved permanently\n\nNow reboot the device!\n";

    server->send(200, F("text/plain"), message);
}

void handleNotFound(){
    server->send(404, "text/plain", "Not found");
}

void setupModeSetup() {
    delay(100);
    server = new WebServer(80);

    WiFi.softAP(setupSsid, setupPassword);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);

    server->on(F("/"), handleIndexScreen);
    server->on(F("/change"), handleChange);
    server->onNotFound(handleNotFound);

    server->begin();
    Serial.println("[SETUP] HTTP config server started on: 192.168.0.1");
}

void setupModeLoop() {
    server->handleClient();
}
