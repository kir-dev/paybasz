#include "PermanentMemory.h"

void PermanentMemory::fillEmpty(uint8_t * var, uint8_t length) {
    for (uint8_t i = 0; i < length; i++)
        var[i] = 0;
}

bool PermanentMemory::isLocked() {

}

void PermanentMemory::save() {

}

void PermanentMemory::load() {

}

void PermanentMemory::getHashedToken(char hash[41]) {
    return;
}