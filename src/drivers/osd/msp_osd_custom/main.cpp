#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <typeinfo>
#include <unistd.h>
#include "layout.hpp"
#include "msp.hpp"
#include "compat/format.hpp"


int main(int argc, char *argv[]) {
    auto _device = argv[1];
    std::cout << "Device: " << _device << std::endl;
    auto _msp_fd = open(_device, O_RDWR | O_NONBLOCK);
    if (_msp_fd < 0) {
        std::cout << "Cant open " << _device << std::endl;
        return 1;
    }
    struct termios t;
    tcgetattr(_msp_fd, &t);
    cfsetspeed(&t, B115200);
    t.c_cflag &= ~(CSTOPB | PARENB | CRTSCTS);
    t.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
    t.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
    t.c_oflag = 0;
    tcsetattr(_msp_fd, TCSANOW, &t);

    MspWriter writer(_msp_fd);
    MspEncoder encoder(MspVersion::V1);

    auto& blinker = OsdBlinker::getInstance();
    blinker.enabled = true;

    OsdLayoutPainter painter(encoder, writer);
    OsdPrimaryLayout layout(OsdLayoutConfig {
        .elements = {
            {OsdLayoutElement::COMPASS, {1, 2}},
            {OsdLayoutElement::HORIZON, {10, 10}},
            {OsdLayoutElement::BATTERY_INFO, {5, 6}},
            {OsdLayoutElement::ARMING_STATUS, {7, 8}},
        }
    });

    std::set<FlightModeFlag> flightModes{FlightModeFlag::_3D};
    OsdParams params {
        .armed = false,
        .flightMode = "FLIGHT",
        .battery = OsdBatteryParams{0, 0},
        .attitude = OsdAttitudeParams{0, 0, 0},
    };

    for (int i = 0; i < 60; i++) {
        params.battery.voltage = i * 10;
        params.battery.current = i * 100;
        params.attitude.pitch = i;
        params.attitude.roll = i * 3;
        params.attitude.yaw = i * 5 * 10;
        if (i > 30) {
            params.armed = true;
            flightModes.insert(FlightModeFlag::ARM);
        }

        writeMsp(encoder, writer, MspStatus {
            .time = i * 1000,
            .flightModes = flightModes
        });

        layout.tick(params);
        painter.draw(layout);

        sleep(1);
    }

    return 0;
}
