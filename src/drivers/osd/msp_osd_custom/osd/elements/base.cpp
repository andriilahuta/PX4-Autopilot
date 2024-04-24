#include "../utils.hpp"
#include "base.hpp"


void OsdObject::setBlink(bool value) {
    blink = value;
}

bool OsdObject::shouldBlink() const {
    return blink;
}

bool OsdObject::shouldNativeBlink() const {
    if (OsdBlinker::getInstance().enabled) return false;
    return shouldBlink();
}
