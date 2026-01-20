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


namespace calculations {
    int convertHeadingToDiscreteDirection(int heading, int directions) {
        const int circle = 360;
        heading += circle;  // Ensure positive value

        // Split input heading 0..359 into sectors 0..(directions - 1), but offset
        // by half a sector so that sector 0 gets centered around heading 0.
        // We multiply heading by directions to not loose precision in divisions
        // In this way each segment will be a `circle` length
        int direction = (heading * directions + circle / 2) / circle;  // scale with rounding
        direction %= directions;  // normalize

        return direction;  // return segment number
    }
}
