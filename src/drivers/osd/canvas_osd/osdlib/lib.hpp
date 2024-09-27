#pragma once

#include <stddef.h>
#include <stdint.h>

#include "fc.hpp"


class OsdBlinker;
class MspWriter;
class MspEncoder;
class OsdLayoutPainter;
class OsdLayout;
class OsdParams;

class Osd {
public:
    Osd(int fd);
    ~Osd();

    // void draw();
    void setBlinkerEnabled(bool enabled);
    // void addLayout(OsdLayout *layout);
    // bool setCurrentLayout(size_t num);

    // void setArmed(bool armed);
    // void setFlightMode(const char* name, const FlightModeFlag modes[], size_t modesSize);
    void setBattery(float voltage, float current);
    // void setAttitude(int pitch, int roll, int yaw);
    // void setTime(uint16_t time);
    // void print();
private:
    OsdBlinker *blinker;

    // MspWriter *writer;
    MspEncoder *encoder;

    // OsdLayoutPainter *painter;
    OsdLayout **layouts;
    size_t layoutsSize = 0;
    int currentLayout = -1;

    // FlightModeFlag *flightModes = nullptr;
    size_t flightModesSize = 0;
    OsdParams *params;
    uint16_t time = 0;
};
