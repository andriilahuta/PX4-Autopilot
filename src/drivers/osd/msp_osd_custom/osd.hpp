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

enum struct OsdSymbol : unsigned char {
    NONE = 0x00,
    END_OF_FONT = 0xFF,
    BLANK = 0x20,
    HYPHEN = 0x2D,
    BBLOG = 0x10,
    HOMEFLAG = 0x11,
    ROLL = 0x14,
    PITCH = 0x15,
    TEMPERATURE = 0x7A,

    // GPS and navigation
    LAT = 0x89,
    LON = 0x98,
    ALTITUDE = 0x7F,
    TOTAL_DISTANCE = 0x71,
    OVER_HOME = 0x05,
    GPS_DEGREE = 0x08,
    GPS_MINUTE = 0x27,
    GPS_SECOND = 0x22,

    // RSSI
    RSSI = 0x01,
    LINK_QUALITY = 0x7B,

    // Throttle Position (%)
    THR = 0x04,

    // Unit Icons (Metric)
    M = 0x0C,
    KM = 0x7D,
    C = 0x0E,

    // Unit Icons (Imperial)
    FT = 0x0F,
    MILES = 0x7E,
    F = 0x0D,

    // Heading Graphics
    HEADING_N = 0x18,
    HEADING_S = 0x19,
    HEADING_E = 0x1A,
    HEADING_W = 0x1B,
    HEADING_DIVIDED_LINE = 0x1C,
    HEADING_LINE = 0x1D,

    // Horizon
    AH_CENTER_LINE = 0x72,
    AH_CENTER = 0x73,
    AH_CENTER_LINE_RIGHT = 0x74,
    AH_RIGHT = 0x02,
    AH_LEFT = 0x03,
    AH_DECORATION = 0x13,
    AH_BAR9_0 = 0x80,
    AH_BAR9_1 = 0x81,
    AH_BAR9_2 = 0x82,
    AH_BAR9_3 = 0x83,
    AH_BAR9_4 = 0x84,
    AH_BAR9_5 = 0x85,
    AH_BAR9_6 = 0x86,
    AH_BAR9_7 = 0x87,
    AH_BAR9_8 = 0x88,

    // Satellite Graphics
    SAT_L = 0x1E,
    SAT_R = 0x1F,

    // Direction arrows
    ARROW_SOUTH = 0x60,
    ARROW_2 = 0x61,
    ARROW_3 = 0x62,
    ARROW_4 = 0x63,
    ARROW_EAST = 0x64,
    ARROW_6 = 0x65,
    ARROW_7 = 0x66,
    ARROW_8 = 0x67,
    ARROW_NORTH = 0x68,
    ARROW_10 = 0x69,
    ARROW_11 = 0x6A,
    ARROW_12 = 0x6B,
    ARROW_WEST = 0x6C,
    ARROW_14 = 0x6D,
    ARROW_15 = 0x6E,
    ARROW_16 = 0x6F,

    ARROW_SMALL_UP = 0x75,
    ARROW_SMALL_DOWN = 0x76,

    // Progress bar
    PB_START = 0x8A,
    PB_FULL = 0x8B,
    PB_HALF = 0x8C,
    PB_EMPTY = 0x8D,
    PB_END = 0x8E,
    PB_CLOSE = 0x8F,

    // Battery
    BATT_FULL = 0x90,
    BATT_5 = 0x91,
    BATT_4 = 0x92,
    BATT_3 = 0x93,
    BATT_2 = 0x94,
    BATT_1 = 0x95,
    BATT_EMPTY = 0x96,
    MAIN_BATT = 0x97,

    // Voltage and amperage
    VOLT = 0x06,
    AMP = 0x9A,
    MAH = 0x07,
    WATT = 0x57,

    // Time
    ON_M = 0x9B,
    FLY_M = 0x9C,

    // Lap Timer
    CHECKERED_FLAG = 0x24,
    PREV_LAP_TIME = 0x79,

    // Speed
    SPEED = 0x70,
    KPH = 0x9E,
    MPH = 0x9D,
    MPS = 0x9F,
    FTPS = 0x99,

    // Menu cursor
    CURSOR = 0x03,

    // Stick overlays
    STICK_OVERLAY_SPRITE_HIGH = 0x08,
    STICK_OVERLAY_SPRITE_MID = 0x09,
    STICK_OVERLAY_SPRITE_LOW = 0x0A,
    STICK_OVERLAY_CENTER = 0x0B,
    STICK_OVERLAY_VERTICAL = 0x16,
    STICK_OVERLAY_HORIZONTAL = 0x17,
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

class OsdText : public OsdObject {
public:
    OsdText(const OsdPosition& position, std::string value = "");
    virtual std::vector<OsdElement> elements() const override;
protected:
    std::string value = "";
    OsdFontLevel fontLevel = OsdFontLevel::NORMAL;
    bool blink = false;
};

class OsdBattery : public OsdText {
public:
    OsdBattery(const OsdPosition& position, float voltage, float current);
    void updateStats(float voltage, float current);
};

class OsdHorizon : public OsdObject {
public:
    OsdHorizon(const OsdPosition& position, int roll, int pitch);

    virtual std::vector<OsdElement> elements() const override;
    void update(int roll, int pitch);
private:
    const int symbolCount = 9;
    bool inverted = false;
    int roll = 0;
    int pitch = 0;
    int maxRoll = 40;  // degrees
    int maxPitch = 20;  // degrees
};

class OsdCompass : public OsdText {
public:
    OsdCompass(const OsdPosition& position, int yaw);
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
