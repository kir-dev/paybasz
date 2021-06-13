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
    static char * QUERY_URL;
    static char * STATUS_URL;
    static char * TOKEN;
public:
    NetworkHelper() {}
    void setupWifi(char * ssid, char * password);

    void sendReading(const char * cardHash);
    AccountBalance getAccountBalance(const char * cardHash);
    void proceedPayment(const char * cardHash, uint32_t amount, char * comment);
    void queryItem(const char * item, uint32_t * price);
    void checkStatus();
    void validateConnection();

    static void setupUrls(char * baseUrl, char * gatewayName, char * token);
};

#endif //PAYBASZ_NETWORKHELPER_H
