#ifndef PAYBASZ_PERMANENTMEMORY_H
#define PAYBASZ_PERMANENTMEMORY_H

#include <stdint.h>

class PermanentMemory {
private:
    static constexpr uint8_t STATE_LENGTH = 2;
    static constexpr uint8_t PIN_LENGTH = 8 + 1;
    static constexpr uint8_t SSID_LENGTH = 32 + 1;
    static constexpr uint8_t WPASSWD_LENGTH = 63 + 1;
    static constexpr uint8_t GW_NAME_LENGTH = 63 + 1;
    static constexpr uint8_t TOKEN_LENGTH = 63 + 1;

    inline void fillEmpty(uint8_t * var, uint8_t length);
public:
    uint8_t state[STATE_LENGTH]; // 1<<1 LOCKED| 1 << 0 SETUP
    uint8_t pinCode[PIN_LENGTH]; // chars allowed
    uint8_t ssid[SSID_LENGTH];
    uint8_t wifiPassword[WPASSWD_LENGTH];
    uint8_t gatewayName[GW_NAME_LENGTH];
    uint8_t token[TOKEN_LENGTH];
    uint8_t baseUrlLength;
    uint8_t * baseUrl;

    PermanentMemory() : baseUrl(nullptr), baseUrlLength(0) {
        fillEmpty(state, STATE_LENGTH);
        fillEmpty(pinCode, PIN_LENGTH);
        fillEmpty(ssid, SSID_LENGTH);
        fillEmpty(wifiPassword, WPASSWD_LENGTH);
        fillEmpty(gatewayName, GW_NAME_LENGTH);
        fillEmpty(token, TOKEN_LENGTH);
    }

    bool isLocked();
    void save();
    void load();
    void getHashedToken(char hash[41]);
};

#endif //PAYBASZ_PERMANENTMEMORY_H
