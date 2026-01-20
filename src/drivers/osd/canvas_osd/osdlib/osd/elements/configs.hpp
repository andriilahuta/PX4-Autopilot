#pragma once

#include "base.hpp"


struct OsdBatteryConfig : OsdObjectConfig {
    OsdBatteryConfig(bool showAmps = true):
        showAmps(showAmps) {};

	bool showAmps;
};

struct OsdHorizonConfig : OsdObjectConfig {
    OsdHorizonConfig(bool showSidebars = true, bool invertedPitch = true, bool invertedRoll = false):
        showSidebars(showSidebars),
        invertedPitch(invertedPitch),
        invertedRoll(invertedRoll) {};

    bool showSidebars;
	bool invertedPitch;
	bool invertedRoll;
};
