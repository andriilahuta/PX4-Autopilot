#pragma once

#include <vector>


namespace msp_osd_utils {
    constexpr auto bit(auto x) {
        return 1 << x;
    }

    std::vector<std::byte> to_bytes(auto value, bool pad = false) {
        std::vector<std::byte> bytes;

        if (value == 0) {
            bytes.push_back(std::byte{});
        } else {
            while (value != 0) {
                std::byte byte = static_cast<std::byte>(value & 0xff);
                bytes.push_back(byte);
                value >>= 8;
            }
        }

        if (pad) {
            // we need a separate variable since bytes.size() changes during iteration
            const auto padSize = sizeof(value) - bytes.size();
            for (int i = 0; i < padSize; i++) {
                bytes.push_back(std::byte{});
            }
        }

        return bytes;
    }
}
