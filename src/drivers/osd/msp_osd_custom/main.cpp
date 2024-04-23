#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <typeinfo>
#include <unistd.h>
#include "msp.hpp"
#include "compat/format.hpp"


void printVal(const std::byte b) {
    std::cout << (char)b;
    // std::cout << std::hex << "0x" << static_cast<unsigned int>(b) << " " << std::dec;
}

void printVal(const msp_osd_buffer& b) {
    for (auto n : b)
        printVal(n);
}


bool printObj(const MspEncoder& encoder, const auto& obj) {
    auto res = encoder.encode(obj);
    for (auto el : res)
        printVal(el);

    std::cout << std::endl;
    return true;
}


bool writeSerial(const MspWriter& writer, const MspEncoder& encoder, const auto& obj) {
    auto res = encoder.encode(obj);
    return writer.write(res);
}


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

    auto write = [&](auto v) {return writeSerial(writer, encoder, v);};
    // auto write = [&](auto v) {return printObj(encoder, v);};

    OsdText disarmed(OsdPosition {.x = 2, .y = 12}, "DISARMED");
    OsdText armed(OsdPosition {.x = 8, .y = 6}, "ARMED");
    OsdBattery batt(OsdPosition {.x = 8, .y = 3}, 11.1, 22.2);
    OsdHorizon ah(OsdPosition {.x = 8, .y = 5}, 69, 60);
    OsdCompass cp(OsdPosition {.x = 8, .y = 4}, 900);


    write(MspCommand::CLEAR_SCREEN);
    // write(MspCommand::DRAW_SCREEN);

    auto res = write(disarmed);
    std::cout << "Res: " << res << std::endl;
    write(batt);
    write(ah);
    write(cp);
    write(MspCommand::DRAW_SCREEN);

    // arm
    for (int i = 0; i < 10; i++)
        write(MspStatus {
            .time = 300,
            .flightModes = {FlightModeFlag::ARM, FlightModeFlag::_3D}
        });
    // writer.write(
    //     std::vector<std::byte> {
            // std::byte{0x24}, std::byte{0x4d}, std::byte{0x3e}, std::byte{0x16}, std::byte{0x65},
            // std::byte{0x7c}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x23}, std::byte{0x00},
    //     std::byte{0x01},
    //     std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x13}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x1a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x24}
    //     });

    // sleep(15);

    res = write(armed);
    std::cout << "Res2: " << res << std::endl;

    write(MspCommand::DRAW_SCREEN);


    auto& inst = OsdBlinker::getInstance();
    inst.enabled = true;
    OsdText test(OsdPosition {.x = 3, .y = 10}, "TEST_0");
    test.setBlink(true);

    for (int i = 0; i < 50; i++) {
        test.setValue(std::format("TEST_{}", i));
        if (i > 30) test.setBlink(false);

        write(MspCommand::CLEAR_SCREEN);
        if (test.enabled && inst.showObject(test)) {
            write(test);
            // test.position.x++;
        }
        write(MspCommand::DRAW_SCREEN);
        sleep(1);
    }

    return 0;
}
