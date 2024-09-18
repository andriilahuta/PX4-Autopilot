#include "../../compatlib/container.hpp"
#include "layout.hpp"
#include "utils.hpp"


void OsdLayout::tick(const OsdParams& params) {
    updateObjects(params);
    timer.tick();
    prevParams = params;
}


OsdPrimaryLayout::OsdPrimaryLayout(const OsdPrimaryLayoutConfig& config):
        config(config), objects {
            {OsdLayoutElement::COMPASS, std::make_shared<OsdCompass>()},
            {OsdLayoutElement::HORIZON, std::make_shared<OsdHorizon>()},
            {OsdLayoutElement::CROSSHAIRS, std::make_shared<OsdCrosshairs>()},
            {OsdLayoutElement::BATTERY_INFO, std::make_shared<OsdBattery>()},
            {OsdLayoutElement::ARMING_STATUS, std::make_shared<OsdText>()},
            {OsdLayoutElement::FLIGHT_MODE, std::make_shared<OsdText>()},
        } {
    for (const auto [_, obj] : objects) {
        obj->enabled = false;
    }
    for (const auto& [element, position, config] : config.elements) {
        if (map_contains(objects, element)) {
            auto obj = objects[element];
            obj->configure(config);
            obj->enabled = true;
            obj->position = position;
        };
    }
}

const std::vector<std::shared_ptr<OsdObject>> OsdPrimaryLayout::getObjects() const {
    auto values = std::views::values(objects);
    return std::vector<std::shared_ptr<OsdObject>>(values.begin(), values.end());
}

void OsdPrimaryLayout::updateObjects(const OsdParams& params) {
    auto compass = std::dynamic_pointer_cast<OsdCompass>(objects[OsdLayoutElement::COMPASS]);
    auto horizon = std::dynamic_pointer_cast<OsdHorizon>(objects[OsdLayoutElement::HORIZON]);
    auto batteryInfo = std::dynamic_pointer_cast<OsdBattery>(objects[OsdLayoutElement::BATTERY_INFO]);
    auto armingStatus = std::dynamic_pointer_cast<OsdText>(objects[OsdLayoutElement::ARMING_STATUS]);
    auto flightMode = std::dynamic_pointer_cast<OsdText>(objects[OsdLayoutElement::FLIGHT_MODE]);

    compass->update(params.attitude.yaw);
    horizon->update(params.attitude.pitch, params.attitude.roll);
    batteryInfo->update(params.battery.voltage, params.battery.current);
    flightMode->setValue(params.flightMode);

    armingStatus->setValue(params.armed ? "ARMED" : "DISARMED");
    if (params.armed && !prevParams.armed) {
        armingStatus->setFontLevel(OsdFontLevel::CRITICAL);
        armingStatus->setBlink(true);
        timer.schedule([&]() {
            armingStatus->setFontLevel(OsdFontLevel::NORMAL);
            armingStatus->setBlink(false);
            armingStatus->enabled = false;
        }, 5000);
    } else if (!params.armed && prevParams.armed) {
        armingStatus->enabled = true;
    }
}


OsdLayoutPainter::OsdLayoutPainter(const MspEncoder& encoder, const MspWriter& writer):
        encoder(encoder), writer(writer) {
}

void OsdLayoutPainter::draw(const OsdLayout& layout) {
    write(MspCommand::HEARTBEAT);
    write(MspCommand::CLEAR_SCREEN);
    for (const auto object : layout.getObjects()) {
        drawObject(*object);
    }
    write(MspCommand::DRAW_SCREEN);
}

bool OsdLayoutPainter::drawObject(const OsdObject& object) {
    if (object.enabled && OsdBlinker::getInstance().showObject(object)) {
        return write(object);
    }
    return false;
}

bool OsdLayoutPainter::write(const auto& object) {
    return writeMsp(encoder, writer, object);
}
