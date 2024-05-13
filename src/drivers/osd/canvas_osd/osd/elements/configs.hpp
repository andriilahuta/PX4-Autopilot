#pragma once

#include "base.hpp"


struct OsdBatteryConfig : OsdObjectConfig {
    OsdBatteryConfig(bool showAmps = true): showAmps(showAmps) {};
	bool showAmps;
};

struct OsdHorizonConfig : OsdObjectConfig {
    OsdHorizonConfig(bool showSidebars = true): showSidebars(showSidebars) {};
	bool showSidebars;
};
