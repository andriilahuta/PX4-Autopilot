#pragma once

#include <vector>


namespace msp_osd_utils {
    constexpr auto bit(auto x) {
        return 1 << x;
    }

    std::vector<std::byte> to_bytes(auto value, bool pad = false) {
        std::vector<std::byte> bytes;

        while (value != 0) {
            std::byte byte = static_cast<std::byte>(value & 0xff);
            bytes.push_back(byte);
            value >>= 8;
        }

        if (pad) {
            for (int i = 0; i < sizeof(value) - bytes.size(); i++) {
                bytes.push_back(std::byte {});
            }
        }

        return bytes;
    }
}
