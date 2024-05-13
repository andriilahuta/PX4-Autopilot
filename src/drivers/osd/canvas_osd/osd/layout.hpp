#pragma once

#include <map>
#include <memory>
#include "../msp.hpp"
#include "../utils.hpp"
#include "elements/objects.hpp"


enum struct OsdLayoutElement {
    COMPASS,
    HORIZON,
    CROSSHAIRS,
    BATTERY_INFO,
    ARMING_STATUS,
    FLIGHT_MODE,
};

struct OsdPrimaryLayoutConfig {
    std::vector<std::tuple<OsdLayoutElement, OsdPosition, std::shared_ptr<OsdObjectConfig>>> elements;
};

struct OsdBatteryParams {
    float voltage = 0;
    float current = 0;
};

struct OsdAttitudeParams {
    int pitch = 0;
    int roll = 0;
    int yaw = 0;
};

struct OsdParams {
    bool armed = false;
    std::string flightMode = "";
    OsdBatteryParams battery;
    OsdAttitudeParams attitude;
};


class OsdLayout {
public:
    virtual ~OsdLayout() = default;
    void tick(const OsdParams& params);
    virtual const std::vector<std::shared_ptr<OsdObject>> getObjects() const = 0;
protected:
    virtual void updateObjects(const OsdParams& params) = 0;

    msp_osd_utils::SyncTimer timer;
    OsdParams prevParams;
};

class OsdPrimaryLayout : public OsdLayout {
public:
    OsdPrimaryLayout(const OsdPrimaryLayoutConfig& config);
    virtual const std::vector<std::shared_ptr<OsdObject>> getObjects() const override;
protected:
    virtual void updateObjects(const OsdParams& params) override;
private:
    OsdPrimaryLayoutConfig config;
    std::map<OsdLayoutElement, std::shared_ptr<OsdObject>> objects;
};


class OsdLayoutPainter {
public:
    OsdLayoutPainter(const MspEncoder& encoder, const MspWriter& writer);
    OsdLayoutPainter(const MspEncoder&&, const MspWriter&&) = delete;
    void draw(const OsdLayout& layout);
private:
    const MspEncoder& encoder;
    const MspWriter& writer;

    bool drawObject(const OsdObject& object);
    bool write(const auto& object);
};
