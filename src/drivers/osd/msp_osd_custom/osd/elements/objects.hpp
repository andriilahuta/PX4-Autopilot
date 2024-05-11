#pragma once

#include "../symbols.hpp"
#include "base.hpp"
#include "configs.hpp"


class OsdText : virtual public OsdObject {
public:
    OsdText(std::string value = "");
    virtual const std::vector<OsdElement> elements() const override;
    void setValue(std::string value);
    void setFontLevel(OsdFontLevel fontLevel);
protected:
    std::string value = "";
    OsdFontLevel fontLevel = OsdFontLevel::NORMAL;
};

class OsdBattery : public OsdText, public OsdObjectConfigMixin<OsdBatteryConfig> {
public:
    OsdBattery();
    void update(float voltage, float current);
};

class OsdHorizon : public OsdObjectConfigMixin<OsdHorizonConfig> {
public:
    OsdHorizon();

    virtual const std::vector<OsdElement> elements() const override;
    void update(int pitch, int roll);
private:
    const int symbolCount = 9;
    const int sidebarWidth = 7;
    const int sidebarHeight = 3;
    
    bool inverted = false;
    int pitch = 0;
    int roll = 0;
    int maxPitch = 20;  // degrees
    int maxRoll = 40;  // degrees
};

class OsdCrosshairs : public OsdText {
public:
    OsdCrosshairs();
};

class OsdCompass : public OsdText {
public:
    OsdCompass();
    void update(int yaw);
private:
    #define COMPASS_SYM(sym) static_cast<unsigned char>(OsdSymbol::sym)
    const std::vector<unsigned char> compassBar {
        COMPASS_SYM(HEADING_W),
        COMPASS_SYM(HEADING_LINE), COMPASS_SYM(HEADING_DIVIDED_LINE), COMPASS_SYM(HEADING_LINE),
        COMPASS_SYM(HEADING_N),
        COMPASS_SYM(HEADING_LINE), COMPASS_SYM(HEADING_DIVIDED_LINE), COMPASS_SYM(HEADING_LINE),
        COMPASS_SYM(HEADING_E),
        COMPASS_SYM(HEADING_LINE), COMPASS_SYM(HEADING_DIVIDED_LINE), COMPASS_SYM(HEADING_LINE),
        COMPASS_SYM(HEADING_S),
        COMPASS_SYM(HEADING_LINE), COMPASS_SYM(HEADING_DIVIDED_LINE), COMPASS_SYM(HEADING_LINE),
        COMPASS_SYM(HEADING_W),
        COMPASS_SYM(HEADING_LINE), COMPASS_SYM(HEADING_DIVIDED_LINE), COMPASS_SYM(HEADING_LINE),
        COMPASS_SYM(HEADING_N),
        COMPASS_SYM(HEADING_LINE), COMPASS_SYM(HEADING_DIVIDED_LINE), COMPASS_SYM(HEADING_LINE),
    };

    int getDiscreteDirection(int heading, int directions);
};
