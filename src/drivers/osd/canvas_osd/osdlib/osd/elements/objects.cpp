#include <algorithm>
#include "objects.hpp"
#include "../utils.hpp"
#include "../../../compatlib/format.hpp"


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

void OsdBattery::setPercentage(int value) {
    percentage = std::clamp(value, 0, 100);
}

void OsdBattery::setCritical(bool value) {
    isCritical = value;
}

void OsdBattery::update(float voltage, float current) {
    int battSymbolIndex = (percentage * 7) / 101;  // divide by 101 to avoid overflow at 100
    OsdSymbol battSymbol;
    if (battSymbolIndex >= 0 && battSymbolIndex < 7) {
        battSymbol = batterySymbols[battSymbolIndex];
    } else {
        battSymbol = OsdSymbol::BATT_EMPTY;
    }

    value = std::format(
        "{:c} {:04.1f}{:c}",
        static_cast<unsigned char>(battSymbol),
        voltage, static_cast<unsigned char>(OsdSymbol::VOLT)
    );
    if (config->showAmps) {
        value += std::format(
            " {:04.1f}{:c}",
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

void OsdHorizon::update(int pitch, int roll) {
    const int pitchSign = config->invertedPitch ? -1 : 1;
    const int rollSign = config->invertedRoll ? -1 : 1;

    const int maxPitch = this->maxPitch;
    const int maxRoll = this->maxRoll;

    pitch = std::clamp(pitch * pitchSign, -maxPitch, maxPitch);
    roll = std::clamp(roll * rollSign, -maxRoll, maxRoll);

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
        int y = -roll * x / 8 - pitch;
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
    const int direction = calculations::convertHeadingToDiscreteDirection(yaw, 16);
    value = std::string(compassBar.begin() + direction, compassBar.begin() + direction + 9);
}
