#pragma once


enum struct FcSensorFlag {
    GYRO = 1 << 0,
    ACC = 1 << 1,
    BARO = 1 << 2,
    MAG = 1 << 3,
    SONAR = 1 << 4,
    RANGEFINDER = 1 << 4,
    GPS = 1 << 5,
    GPSMAG = 1 << 6,
};

enum struct FlightModeFlag {
    ARM = 0,

    // flight modes
    ANGLE,
    HORIZON,
    MAG,
    HEADFREE,
    PASSTHRU,
    FAILSAFE,
    GPSRESCUE,

    // RC modes
    ACRO,
    _3D,
    ANTIGRAVITY,
    OSD,
    TELEMETRY,
    BLACK,
    AIRMODE,
    FLIPOVERAFTERCRASH,
    PREARM,
    ACROTRAINER,
    LAUNCHCONTROL,

    __COUNT__,
};

static_assert((int)FlightModeFlag::__COUNT__ <= 32, "Flight mode numbers larger than 32 (bits) is not implemented");
