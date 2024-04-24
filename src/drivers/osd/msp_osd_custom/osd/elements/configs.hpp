#pragma once

#include "base.hpp"


struct OsdBatteryConfig : OsdObjectConfig {
    OsdBatteryConfig(bool showAmps = true): showAmps(showAmps) {};
	bool showAmps;
};
