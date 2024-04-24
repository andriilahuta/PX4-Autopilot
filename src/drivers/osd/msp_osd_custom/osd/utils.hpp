#pragma once

#include "elements/base.hpp"


class OsdBlinker {
public:
    static OsdBlinker& getInstance();
    OsdBlinker(OsdBlinker const&) = delete;
    void operator=(OsdBlinker const&) = delete;

    bool showObject(const OsdObject& object) const;

    bool enabled = false;
    float frequency = 1;
private:
    OsdBlinker() {};
};
