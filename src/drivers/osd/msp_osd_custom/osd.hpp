#pragma once

#include <stdint.h>
#include <string>
#include <vector>


struct OsdPosition {
    uint8_t x = 0;
    uint8_t y = 0;
};

enum struct OsdFontLevel : uint8_t {
    NORMAL,
    INFO,
    WARNING,
    CRITICAL,
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
    virtual std::vector<OsdElement> elements() const = 0;
protected:
    OsdObject() = default;
    OsdObject(const OsdPosition& position);
};

class OsdTextObject : public OsdObject {
public:
    OsdTextObject(const OsdPosition& position, std::string value);

    std::string value = "";
    OsdFontLevel fontLevel = OsdFontLevel::NORMAL;
    bool blink = false;

    virtual std::vector<OsdElement> elements() const override;
};

class OsdTextPairObject : public OsdObject {
public:
    OsdTextPairObject(const OsdPosition& position, std::string valueLeft, std::string valueRight);

    std::string valueLeft = "";
    std::string valueRight = "";

    virtual std::vector<OsdElement> elements() const override;
};
