#include "../utils.hpp"
#include "base.hpp"


bool OsdObject::configure(const std::shared_ptr<OsdObjectConfig> config) {
    if (config) {
        this->config = config;
        return true;
    }
    return false;
}

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
