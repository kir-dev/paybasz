#ifndef PAYBASZ_NETWORKHELPER_H
#define PAYBASZ_NETWORKHELPER_H

#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <stdint.h>

static WiFiMulti wifiMulti;

struct AccountBalance {
    int32_t balance;
    bool loan;
    bool allowed;
};

class NetworkHelper {
private:
    static char * READING_URL;
    static char * ACCOUNT_URL;
    static char * PAYMENT_URL;
    static char * VALIDATE_URL;
    static char * STATUS_URL;
public:
    NetworkHelper() {}
    void setupWifi();

    void sendReading(const char * cardHash);
    AccountBalance getAccountBalance(const char * cardHash);
    void proceedPayment(const char * cardHash, uint32_t amount);

    static void setupUrls(char * baseUrl, char * gatewayName);
};

#endif //PAYBASZ_NETWORKHELPER_H
