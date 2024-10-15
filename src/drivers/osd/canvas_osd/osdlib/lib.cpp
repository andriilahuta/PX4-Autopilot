// #include <iostream>

#include "lib.hpp"

#include "msp.hpp"
#include "osd/layout.hpp"
#include "osd/utils.hpp"
#include "../compatlib/format.hpp"


Osd::Osd(int fd) {
    blinker = &OsdBlinker::getInstance();

    // writer = new MspWriter(fd);
    encoder = new MspEncoder();

    // painter = new OsdLayoutPainter(*encoder, *writer);
    layouts = new OsdLayout*[1];
    layouts[0] = new OsdPrimaryLayout(OsdPrimaryLayoutConfig {
		.elements = {
		    {OsdLayoutElement::COMPASS, {12, 0}, nullptr},
		    {OsdLayoutElement::HORIZON, {23, 8}, std::make_shared<OsdHorizonConfig>(true)},
		    {OsdLayoutElement::CROSSHAIRS, {20, 8}, nullptr},
		    {OsdLayoutElement::BATTERY_INFO, {0, 0}, std::make_shared<OsdBatteryConfig>(false)},
		    {OsdLayoutElement::ARMING_STATUS, {14, 15}, nullptr},
		}
	});
    layoutsSize = 1;
    currentLayout = 0;

    params = new OsdParams();
}

Osd::~Osd() {
    delete params;
    // if (flightModes != nullptr) delete[] flightModes;

    for (size_t i = 0; i < layoutsSize; i++) delete layouts[i];
    delete[] layouts;
    // delete painter;

    delete encoder;
    // delete writer;
}

// void Osd::draw() {
//     auto status = MspStatus {
// 		.time = time,
// 	};
//     if (flightModes != nullptr) {
//         std::set<FlightModeFlag> flightModesSet(flightModes, flightModes + flightModesSize);
//         status.flightModes = flightModesSet;
//     }

// 	writeMsp(*encoder, *writer, status);

//     auto layout = layouts[currentLayout];
//     layout->tick(*params);
//     painter->draw(*layout);
// }

void Osd::setBlinkerEnabled(bool enabled) {
    blinker->enabled = enabled;

    OsdText t;
    blinker->showObject(t);
}

// bool Osd::setCurrentLayout(size_t num) {
//     if (num >= layoutsSize) return false;
//     currentLayout = num;
//     return true;
// }

// void Osd::setArmed(bool armed) {
//     params->armed = armed;
// }

// void Osd::setFlightMode(const char* name, const FlightModeFlag modes[], size_t modesSize) {
//     params->flightMode = name;

//     if (flightModes != nullptr) delete[] flightModes;
//     flightModes = new FlightModeFlag[modesSize];
//     for (size_t i = 0; i < modesSize; i++) {
//         flightModes[i] = modes[i];
//     }
//     flightModesSize = modesSize;
// }

void Osd::setBattery(float voltage, float current) {
    params->battery = OsdBatteryParams{voltage, current};
}

// void Osd::setAttitude(int pitch, int roll, int yaw) {
//     params->attitude = OsdAttitudeParams{pitch, roll, yaw};
// }

// void Osd::setTime(uint16_t time) {
//     this->time = time;
// }

// void Osd::print() {
//     std::cout << "foo" << std::endl;
// }
