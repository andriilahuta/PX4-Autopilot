#include <algorithm>
#include "objects.hpp"
#include "../../utils.hpp"
#include "../../compat/format.hpp"


OsdText::OsdText(std::string value):
        value(value) {
}

void OsdText::setValue(std::string value) {
    this->value = value;
}

void OsdText::setFontLevel(OsdFontLevel fontLevel) {
    this->fontLevel = fontLevel;
}

const std::vector<OsdElement> OsdText::elements() const {
    return std::vector<OsdElement> {
        OsdElement {
            .position = position,
            .value = value,
            .fontLevel = fontLevel,
            .blink = shouldNativeBlink(),
        }
    };
}


OsdBattery::OsdBattery() {
    update(0, 0);
}

void OsdBattery::update(float voltage, float current) {
    const bool isCritical = true;
    value = std::format(
        "{:c} {}{:c}",
        static_cast<unsigned char>(OsdSymbol::BATT_3),
        voltage, static_cast<unsigned char>(OsdSymbol::VOLT)
    );
    if (config->showAmps) {
        value += std::format(
            " {}{:c}",
            current, static_cast<unsigned char>(OsdSymbol::AMP)
        );
    }

    if (isCritical) {
        setBlink(true);
        fontLevel = OsdFontLevel::CRITICAL;
    }
}


OsdHorizon::OsdHorizon() {
    update(0, 0);
}

void OsdHorizon::update(int roll, int pitch) {
    const int sign = inverted ? -1 : 1;
    const int maxPitch = this->maxPitch * 10;
    const int maxRoll = this->maxRoll * 10;

    roll = std::clamp(roll * sign, -maxRoll, maxRoll);
    pitch = std::clamp(pitch * sign, -maxPitch, maxPitch);

    // Convert pitch to y compensation value
    // uses fixed divisor of 8 and fixed max AHI pitch angle of 20.0 degrees
    if (maxPitch > 0) {
        pitch = pitch * 25 / maxPitch;
    }
    pitch -= 4 * symbolCount + 5;

    this->roll = roll;
    this->pitch = pitch;
}

const std::vector<OsdElement> OsdHorizon::elements() const {
    std::vector<OsdElement> res;

    for (int x = -4; x <= 4; x++) {
        int y = -roll * x / 64 - pitch;
        if (y >= 0 && y <= 81) {
            res.push_back(
                OsdElement {
                    .position = OsdPosition{
                        position.x + x,
                        position.y + y / symbolCount - symbolCount / 2,
                    },
                    .value = std::string{static_cast<int>(OsdSymbol::AH_BAR9_0) + y % symbolCount},
                    .blink = shouldNativeBlink(),
                }
            );
        }
    }

    if (config->showSidebars) {
        for (int y = -sidebarHeight; y <= sidebarHeight; y++) {
            for (int xSign : {-1, 1}) {
                res.push_back(
                    OsdElement {
                        .position = OsdPosition{
                            position.x + xSign * sidebarWidth,
                            position.y + y,
                        },
                        .value = std::string{static_cast<unsigned char>(OsdSymbol::AH_DECORATION)},
                        .blink = shouldNativeBlink(),
                    }
                );
            }
        }
        for (const auto& [xSign, symbol] : {std::pair(-1, OsdSymbol::AH_LEFT), std::pair(1, OsdSymbol::AH_RIGHT)}) {
            res.push_back(
                OsdElement {
                    .position = OsdPosition{
                        position.x + xSign * sidebarWidth - xSign,
                        position.y,
                    },
                    .value = std::string{static_cast<unsigned char>(symbol)},
                    .blink = shouldNativeBlink(),
                }
            );
        }
    }

    return res;
}


OsdCrosshairs::OsdCrosshairs() {
    value = std::format(
        "{:c}{:c}{:c}",
        static_cast<unsigned char>(OsdSymbol::AH_CENTER_LINE),
        static_cast<unsigned char>(OsdSymbol::AH_CENTER),
        static_cast<unsigned char>(OsdSymbol::AH_CENTER_LINE_RIGHT)
    );
}


OsdCompass::OsdCompass() {
    update(0);
}

void OsdCompass::update(int yaw) {
    const int degYaw = msp_osd_utils::decidegrees_to_degrees(yaw);
    const int direction = getDiscreteDirection(degYaw, 16);
    value = std::string(compassBar.begin() + direction, compassBar.begin() + direction + 9);
}

int OsdCompass::getDiscreteDirection(int heading, int directions) {
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
