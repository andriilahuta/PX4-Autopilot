#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include "msp.hpp"


void printObj(const MspEncoder& encoder, const OsdObject& obj) {
    auto res = encoder.encode(obj);
    for (auto el : res)
        for (auto n : el)
            std::cout /* << "----" */ << (char)n/*  << std::endl */;
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

    OsdTextObject disarmed(OsdPosition {.x = 2, .y = 12}, "DISARMED");
    OsdTextObject armed(OsdPosition {.x = 8, .y = 6}, "ARMED");


    writeSerial(writer, encoder, MspCommand::CLEAR_SCREEN);
    writeSerial(writer, encoder, MspCommand::DRAW_SCREEN);

    // printObj(encoder, disarmed);
    auto res = writeSerial(writer, encoder, disarmed);
    std::cout << "Res: " << res << std::endl;
    // printObj(encoder, disarmed);
    writeSerial(writer, encoder, MspCommand::DRAW_SCREEN);

    // arm
    for (int i = 0; i < 10; i++)
    writer.write(std::vector<std::vector<std::byte>> {
        std::vector<std::byte> {std::byte{0x24}, std::byte{0x4d}, std::byte{0x3e}, std::byte{0x16}, std::byte{0x65}, std::byte{0x7c}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x23}, std::byte{0x00}, std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x13}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x1a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x24}},
    });

    // sleep(15);

    res = writeSerial(writer, encoder, armed);
    std::cout << "Res2: " << res << std::endl;

    writeSerial(writer, encoder, MspCommand::DRAW_SCREEN);

    return 0;
}
