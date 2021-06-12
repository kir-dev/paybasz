#ifndef PAYBASZ_PERMANENTMEMORY_H
#define PAYBASZ_PERMANENTMEMORY_H

#include <stdint.h>

class PermanentMemory {
private:
    inline void fillEmpty(char * var, unsigned int length);
public:
    static constexpr unsigned int STATE_LENGTH = 1;
    static constexpr unsigned int PIN_LENGTH = 8 + 1;
    static constexpr unsigned int SSID_LENGTH = 32 + 1;
    static constexpr unsigned int WPASSWD_LENGTH = 63 + 1;
    static constexpr unsigned int GW_NAME_LENGTH = 63 + 1;
    static constexpr unsigned int TOKEN_LENGTH = 64 + 1;
    static constexpr unsigned int BASE_URL_LENGTH = 128 + 1;

    char state; // 1<<1 LOCKED| 1 << 0 SETUP
    char pinCode[PIN_LENGTH];
    char ssid[SSID_LENGTH];
    char wifiPassword[WPASSWD_LENGTH];
    char gatewayName[GW_NAME_LENGTH];
    char token[TOKEN_LENGTH];
    char baseUrl[BASE_URL_LENGTH];

    PermanentMemory();

    bool isLocked();
    bool isSetup();
    void setSetup(bool value);
    void save();
    void load();
    void factoryReset();
};

#endif //PAYBASZ_PERMANENTMEMORY_H
