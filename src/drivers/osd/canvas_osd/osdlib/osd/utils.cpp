#include <chrono>
#include "utils.hpp"


OsdBlinker& OsdBlinker::getInstance() {
    static OsdBlinker instance;
    return instance;
}

bool OsdBlinker::showObject(const OsdObject& object) const {
    if (!enabled) return true;
    if (!object.shouldBlink()) return true;

    const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    const int period = static_cast<int>(1000.0 / frequency);

    return now.count() % (period * 2) < period;
}
