#include <fcntl.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <termios.h>
#include <typeinfo>
#include <unistd.h>
#include "msp.hpp"
#include "osd/layout.hpp"
#include "osd/utils.hpp"
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
    OsdPrimaryLayout layout(OsdPrimaryLayoutConfig {
        .elements = {
            {OsdLayoutElement::COMPASS, {1, 2}, nullptr},
            {OsdLayoutElement::HORIZON, {10, 10}, nullptr},
            {OsdLayoutElement::BATTERY_INFO, {5, 6}, std::make_shared<OsdBatteryConfig>(false)},
            {OsdLayoutElement::ARMING_STATUS, {7, 8}, nullptr},
        }
    });

    std::set<FlightModeFlag> flightModes{FlightModeFlag::_3D};
    OsdParams params {
        .armed = false,
        .flightMode = "FLIGHT",
        .battery = OsdBatteryParams{0, 0},
        .attitude = OsdAttitudeParams{0, 0, 0},
    };

    for (int i = 0; i < 60 * 2; i++) {
        params.battery.voltage = i * 10;
        params.battery.current = i * 100;
        params.attitude.pitch = i;
        params.attitude.roll = i * 2;
        params.attitude.yaw = i * 4 * 10;

        if (i == 30) {
            params.armed = true;
            flightModes.insert(FlightModeFlag::ARM);
        }
        if (i == 60) {
            params.armed = false;
            flightModes = {FlightModeFlag::_3D};
        }

        for (int j = 0; j < 10; j++) {
            writeMsp(encoder, writer, MspStatus {
                .time = i * 1000,
                .flightModes = flightModes
            });
        }

        layout.tick(params);
        painter.draw(layout);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    writeMsp(encoder, writer, MspCommand::CLEAR_SCREEN);
    writeMsp(encoder, writer, MspCommand::DRAW_SCREEN);

    return 0;
}
