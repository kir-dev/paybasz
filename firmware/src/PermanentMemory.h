#ifndef PAYBASZ_PERMANENTMEMORY_H
#define PAYBASZ_PERMANENTMEMORY_H

#include <stdint.h>

class PermanentMemory {
private:
    static constexpr unsigned int STATUS_LENGTH = 1;
    static constexpr unsigned int PIN_LENGTH = 8 + 1;
    static constexpr unsigned int SSID_LENGTH = 32 + 1;
    static constexpr unsigned int WPASSWD_LENGTH = 63 + 1;
    static constexpr unsigned int GW_NAME_LENGTH = 63 + 1;
    static constexpr unsigned int TOKEN_LENGTH = 64 + 1;
    static constexpr unsigned int BASE_URL_LENGTH = 128 + 1;

    inline void fillEmpty(char * var, unsigned int length);
public:
    char state; // 1<<1 LOCKED| 1 << 0 SETUP
    char pinCode[PIN_LENGTH];
    char ssid[SSID_LENGTH];
    char wifiPassword[WPASSWD_LENGTH];
    char gatewayName[GW_NAME_LENGTH];
    char token[TOKEN_LENGTH];
    char baseUrl[BASE_URL_LENGTH];

    PermanentMemory() {
        fillEmpty(state, STATE_LENGTH);
        fillEmpty(pinCode, PIN_LENGTH);
        fillEmpty(ssid, SSID_LENGTH);
        fillEmpty(wifiPassword, WPASSWD_LENGTH);
        fillEmpty(gatewayName, GW_NAME_LENGTH);
        fillEmpty(token, TOKEN_LENGTH);
        fillEmpty(baseUrl, BASE_URL_LENGTH);
    }

    bool isLocked();
    bool isSetup();
    void save();
    void load();
};

#endif //PAYBASZ_PERMANENTMEMORY_H
