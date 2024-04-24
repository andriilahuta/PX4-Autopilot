#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "../symbols.hpp"


struct OsdPosition {
    uint8_t x = 0;
    uint8_t y = 0;
};


struct OsdElement {
    OsdPosition position = {};
    std::string value = "";
    OsdFontLevel fontLevel = OsdFontLevel::NORMAL;
    bool blink = false;
};


class OsdObject {
public:
    virtual ~OsdObject() = default;
    OsdPosition position = {};
    bool enabled = true;

    virtual const std::vector<OsdElement> elements() const = 0;
    void setBlink(bool value);
    bool shouldBlink() const;
protected:
    OsdObject() = default;
    OsdObject(const OsdPosition& position);
    bool shouldNativeBlink() const;
private:
    bool blink = false;
};
