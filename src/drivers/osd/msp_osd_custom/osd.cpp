#include "osd.hpp"


OsdObject::OsdObject(const OsdPosition& position):
    position(position) {
}


OsdTextObject::OsdTextObject(const OsdPosition& position, std::string value):
        position(position) {
    this->value = value;
}

std::vector<OsdElement> OsdTextObject::elements() const {
    return std::vector<OsdElement> {
        OsdElement {
            .position = position,
            .value = value,
            .fontLevel = fontLevel,
            .blink = blink,
        }
    };
}


OsdTextPairObject::OsdTextPairObject(const OsdPosition& position, std::string valueLeft, std::string valueRight):
        valueLeft(valueLeft),
        valueRight(valueRight) {
    this->position = position;
}

std::vector<OsdElement> OsdTextPairObject::elements() const {
    return std::vector<OsdElement> {
    };
}
