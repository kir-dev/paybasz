#include "NetworkHelper.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "Firmare.gen.h"
#include "ScreenBase.h"

char * NetworkHelper::READING_URL = nullptr;
char * NetworkHelper::ACCOUNT_URL = nullptr;
char * NetworkHelper::PAYMENT_URL = nullptr;
char * NetworkHelper::VALIDATE_URL = nullptr;
char * NetworkHelper::STATUS_URL = nullptr;
char * NetworkHelper::TOKEN = nullptr;

void NetworkHelper::setupUrls(char * baseUrl, char * gatewayName, char * token) {
    int baseLength = strlen(baseUrl);
    int gatewayLength = strlen(gatewayName);
    NetworkHelper::TOKEN = token;

    READING_URL = new char[baseLength + 9 + gatewayLength + 1];
    strcpy(READING_URL, baseUrl);
    strcpy(READING_URL + baseLength, "/reading/");
    strcpy(READING_URL + baseLength + 9, gatewayName);
    Serial.println(READING_URL);

    ACCOUNT_URL = new char[baseLength + 9 + gatewayLength + 1];
    strcpy(ACCOUNT_URL, baseUrl);
    strcpy(ACCOUNT_URL + baseLength, "/balance/");
    strcpy(ACCOUNT_URL + baseLength + 9, gatewayName);
    Serial.println(ACCOUNT_URL);

    PAYMENT_URL = new char[baseLength + 5 + gatewayLength + 1];
    strcpy(PAYMENT_URL, baseUrl);
    strcpy(PAYMENT_URL + baseLength, "/pay/");
    strcpy(PAYMENT_URL + baseLength + 5, gatewayName);
    Serial.println(PAYMENT_URL);

    VALIDATE_URL = new char[baseLength + 10 + gatewayLength + 1];
    strcpy(VALIDATE_URL, baseUrl);
    strcpy(VALIDATE_URL + baseLength, "/validate/");
    strcpy(VALIDATE_URL + baseLength + 10, gatewayName);
    Serial.println(VALIDATE_URL);

    STATUS_URL = new char[baseLength + 6 + 1];
    strcpy(STATUS_URL, baseUrl);
    strcpy(STATUS_URL + baseLength, "/status");
    Serial.println(STATUS_URL);
}

void NetworkHelper::setupWifi() {
    wifiMulti.addAP(TEST_SSID, TEST_PASSWD);
    while (wifiMulti.run() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.print("[SETUP] Connected to WiFi network with IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("[SETUP] Wifi: GO");
}

void NetworkHelper::sendReading(const char * cardHash) {
    Serial.print("[API] Sending card read: ");
    Serial.println(cardHash);

    for (int i = 0; i < 25; ++i) {
        if (i != 0)
            ScreenBase::displayManager->displayLoadingScreen(i % 4);

        if (wifiMulti.run() == WL_CONNECTED) {
            HTTPClient http;
            http.begin(READING_URL);
            http.addHeader("Content-Type", "application/json");
            char message[64 + 64 + 28 + 1];
            sprintf(message, "{\"card\":\"%s\",\"gatewayCode\":\"%s\"}", cardHash, NetworkHelper::TOKEN);
            int httpResponseCode = http.PUT(message);

            Serial.print("[API] HTTP Response code: ");
            Serial.println(httpResponseCode);

            if (httpResponseCode == HTTP_CODE_OK) {
                Serial.print("[API] OK; payload: ");
                String payload = http.getString();
                Serial.println(payload);
            } else if (httpResponseCode < 0) {
                delay(400);
                http.end();
                continue;
            }

            http.end();
            return;
        } else {
            Serial.println("[API] WiFi Disconnected, retrying");
            delay(400);
        }
    }
}

AccountBalance NetworkHelper::getAccountBalance(const char * cardHash) {
    AccountBalance result;
    result.balance = 0;
    result.loan = true;
    result.allowed = false;

    Serial.print("[API] Sending card read: ");
    Serial.println(cardHash);

    for (int i = 0; i < 25; ++i) {
        if (i != 0)
            ScreenBase::displayManager->displayLoadingScreen(i % 4);

        if (wifiMulti.run() == WL_CONNECTED) {
            HTTPClient http;
            http.begin(ACCOUNT_URL);
            http.addHeader("Content-Type", "application/json");
            char message[64 + 64 + 28 + 1];
            sprintf(message, "{\"card\":\"%s\",\"gatewayCode\":\"%s\"}", cardHash, NetworkHelper::TOKEN);
            int httpResponseCode = http.PUT(message);

            Serial.print("[API] HTTP Response code: ");
            Serial.println(httpResponseCode);
            if (httpResponseCode == 200) {
                Serial.print("[API] OK; payload: ");
                String payload = http.getString();
                Serial.println(payload);

                char money[12] = { '\0' };
                char separator[] = { ';' };
                int moneyLength = strcspn(payload.c_str(), separator);
                memcpy(money, payload.c_str(), moneyLength);
                result.balance = atoi(money);
                result.loan = payload.charAt(moneyLength + 1) == '1';
                result.allowed = payload.charAt(moneyLength + 3) == '1';

            } else if (httpResponseCode < 0) {
                delay(400);
                http.end();
                continue;
            }

            http.end();
            return result;
        } else {
            Serial.println("[API] WiFi Disconnected, retrying");
            delay(400);
        }
    }

    return result;
}

void NetworkHelper::proceedPayment(const char * cardHash, uint32_t amount) {
    Serial.print("[API] Sending payment: ");
    Serial.print(cardHash);
    Serial.print(" wit amount: ");
    Serial.println(amount);

    for (int i = 0; i < 25; ++i) {
        if (i != 0)
            ScreenBase::displayManager->displayLoadingScreen(i % 4);

        if (wifiMulti.run() == WL_CONNECTED) {
            HTTPClient http;
            http.begin(PAYMENT_URL);
            http.addHeader("Content-Type", "application/json");
            char message[64 + 64 + 28 + 16 + 1];
            sprintf(message, "{\"card\":\"%s\",\"amount\":%d,\"gatewayCode\":\"%s\"}",
                    cardHash, amount, NetworkHelper::TOKEN);
            int httpResponseCode = http.POST(message);

            Serial.print("[API] HTTP Response code: ");
            Serial.println(httpResponseCode);

            if (httpResponseCode == HTTP_CODE_OK) {
                Serial.print("[API] OK; payload: ");
                String payload = http.getString();
                Serial.println(payload);

                if (payload.equals("ACCEPTED")) {
                    ScreenBase::displayManager->displayDoneStatus();
                    Serial.print("[API] Payment proceed!");
                } else if (payload.equals("INTERNAL_ERROR")) {
                    ScreenBase::displayManager->displayFailedStatus("SZERVER HIBA");
                } else if (payload.equals("NOT_ENOUGH_CASH")) {
                    ScreenBase::displayManager->displayFailedStatus("NINCS FEDEZET");
                } else if (payload.equals("VALIDATION_ERROR")) {
                    ScreenBase::displayManager->displayFailedStatus("NEM REGISZTRALT");
                } else if (payload.equals("CARD_REJECTED")) {
                    ScreenBase::displayManager->displayFailedStatus("LETILTVA");
                } else if (payload.equals("UNAUTHORIZED_TERMINAL")) {
                    ScreenBase::displayManager->displayFailedStatus("TERMINAL AUTH HIBA");
                } else {
                    ScreenBase::displayManager->displayFailedStatus("VARATLAN HIBA");
                }
            } else if (httpResponseCode < 0) {
                delay(400);
                http.end();
                continue;
            } else {
                ScreenBase::displayManager->displayFailedStatus("KOMM. HIBA");
            }

            http.end();
            return;
        } else {
            Serial.println("[API] WiFi Disconnected, retrying");
            delay(400);
        }
    }

}