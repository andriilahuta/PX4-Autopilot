#include <cstddef>
#include <unistd.h>
#include "msp.hpp"
#include "compat/container.hpp"
#include "compat/span.hpp"


static std::byte mspSerialChecksumBuf(const auto& data, std::byte checksum = std::byte{0}) {
    for (const auto& el : data) {
        checksum ^= el;
    }
    return checksum;
}

static std::byte crc8Calc(std::byte val, std::byte poly, std::byte checksum = std::byte{0}) {
    checksum ^= val;
    for (int i = 0; i < 8; i++) {
        checksum <<= 1;
        if (static_cast<bool>(checksum & std::byte{0x80})) {
            checksum ^= poly;
        }
    }
    return checksum;
}

static std::byte crc8Update(const auto& data, std::byte poly, std::byte checksum = std::byte{0}) {
    for (const auto& el : data) {
        checksum = crc8Calc(el, poly, checksum);
    }
    return checksum;
}

static std::byte crc8DvbS2Update(const auto& data, std::byte checksum = std::byte{0}) {
    return crc8Update(data, std::byte{0xD5}, checksum);
}


MspOsdElement::MspOsdElement(const OsdElement& element):
        value(element.value, 0, MSP_OSD_MAX_STRING_LENGTH),
        blink(element.blink) {
    row = element.position.y;
    column = element.position.x + value.length();
    switch (element.fontLevel) {
        case OsdFontLevel::NORMAL:
            fontSeverity = MspOsdFontSeverity::NORMAL;
            break;
        case OsdFontLevel::INFO:
            fontSeverity = MspOsdFontSeverity::INFO;
            break;
        case OsdFontLevel::WARNING:
            fontSeverity = MspOsdFontSeverity::WARNING;
            break;
        case OsdFontLevel::CRITICAL:
            fontSeverity = MspOsdFontSeverity::CRITICAL;
            break;
    }
}


MspEncoder::MspEncoder(MspVersion version):
	_version(version) {
}

msp_osd_buffer MspEncoder::encode(const OsdElement& element) const {
    const MspOsdElement mspElement(element);
    return encode(mspElement);
}

std::vector<msp_osd_buffer> MspEncoder::encode(const OsdObject& object) const {
    const auto elements = object.elements();
    std::vector<msp_osd_buffer> res;
    res.reserve(elements.size());
    for (const auto& el : elements) {
        res.push_back(encode(el));
    }
    return res;
}

msp_osd_buffer MspEncoder::createHeaderBuffer(const MspResultStatus resultStatus) const {
    return msp_osd_buffer {
        std::byte{'$'},
        std::byte{mspVersionMagicInitializer.at(_version)},
        std::byte{resultStatus == MspResultStatus::ERROR ? '!' : '>'},
    };
}

msp_osd_buffer MspEncoder::createDataBuffer(const MspCommand command) const {
    return msp_osd_buffer {static_cast<std::byte>(command)};
}

msp_osd_buffer MspEncoder::createDataBuffer(const MspStatus status) const {
    /*
    MSP:
        uint16_t cycleTime;
        uint16_t i2cErrorsCount;
        uint16_t sensor;
        uint32_t flag;
        uint8_t currentSetting;
    */



            // boxBitmask_t flightModeFlags;
            // const int flagBits = packFlightModeFlags(&flightModeFlags);

            // sbufWriteU16(dst, getTaskDeltaTimeUs(TASK_PID));
            // sbufWriteU16(dst, i2cGetErrorCounter());
            // sbufWriteU16(dst, sensors(SENSOR_ACC) | sensors(SENSOR_BARO) << 1 | sensors(SENSOR_MAG) << 2 | sensors(SENSOR_GPS) << 3 | sensors(SENSOR_RANGEFINDER) << 4 | sensors(SENSOR_GYRO) << 5);
            // sbufWriteData(dst, &flightModeFlags, 4);        // unconditional part of flags, first 32 bits
            // sbufWriteU8(dst, getCurrentPidProfileIndex());
            // sbufWriteU16(dst, constrain(getAverageSystemLoadPercent(), 0, LOAD_PERCENTAGE_ONE));
            // if (cmdMSP == MSP_STATUS_EX) {
            //     sbufWriteU8(dst, PID_PROFILE_COUNT);
            //     sbufWriteU8(dst, getCurrentControlRateProfileIndex());
            // } else {  // MSP_STATUS
            //     sbufWriteU16(dst, 0); // gyro cycle time
            // }

            // // write flightModeFlags header. Lowest 4 bits contain number of bytes that follow
            // // header is emited even when all bits fit into 32 bits to allow future extension
            // int byteCount = (flagBits - 32 + 7) / 8;        // 32 already stored, round up
            // byteCount = constrain(byteCount, 0, 15);        // limit to 16 bytes (128 bits)
            // sbufWriteU8(dst, byteCount);
            // sbufWriteData(dst, ((uint8_t*)&flightModeFlags) + 4, byteCount);

            // // Write arming disable flags
            // // 1 byte, flag count
            // sbufWriteU8(dst, ARMING_DISABLE_FLAGS_COUNT);
            // // 4 bytes, flags
            // const uint32_t armingDisableFlags = getArmingDisableFlags();
            // sbufWriteU32(dst, armingDisableFlags);

            // // config state flags - bits to indicate the state of the configuration, reboot required, etc.
            // // other flags can be added as needed
            // sbufWriteU8(dst, (getRebootRequired() << 0));

            // // Added in API version 1.46
            // // Write CPU temp
            // sbufWriteU16(dst, getCoreTemperatureCelsius());




    return msp_osd_buffer{};
}

msp_osd_buffer MspEncoder::createDataBuffer(const MspOsdSysElement& element) const {
    msp_osd_buffer buff {
        static_cast<std::byte>(MspCommand::SYS),
        std::byte{element.row},
        std::byte{element.column},
        std::byte{static_cast<uint8_t>(element.type)},
    };
    return buff;
}

msp_osd_buffer MspEncoder::createDataBuffer(const MspOsdElement& element) const {
    msp_osd_buffer buff {
        static_cast<std::byte>(MspCommand::WRITE_STRING),
        std::byte{element.row},
        std::byte{element.column},
        std::byte{static_cast<uint8_t>(element.fontSeverity) & MSP_ATTR_FONT},
    };
    if (element.blink) {
        buff[3] |= std::byte{MSP_ATTR_BLINK};
    }
    buff.reserve(buff.size() + element.value.size());
    std::transform(element.value.begin(), element.value.end(), std::back_inserter(buff), [](unsigned char ch) {
        return std::byte{ch};
    });
    return buff;
}

msp_osd_buffer MspEncoder::encodeDispatch(msp_osd_buffer& headerBuff, const msp_osd_buffer& dataBuff,
                                          const MspProtocolCommand command) const {
    switch (_version) {
        case MspVersion::V1:
            return encodeV1(headerBuff, dataBuff, command);
            break;
        case MspVersion::V2_OVER_V1:
            return encodeV2overV1(headerBuff, dataBuff, command);
            break;
        case MspVersion::V2:
            return encodeV2(headerBuff, dataBuff, command);
            break;
        default:
            return encodeV1(headerBuff, dataBuff, command);
            break;
    }
}

msp_osd_buffer MspEncoder::encodeV1(msp_osd_buffer& headerBuff, const msp_osd_buffer& dataBuff,
                                    const MspProtocolCommand command) const {
    /*
    Header:
        uint8_t size;
        uint8_t cmd;

    Jumbo header:
        uint16_t size;
    */
    const auto dataSize = dataBuff.size();

    headerBuff.push_back(std::byte{std::min(dataSize, static_cast<std::size_t>(MSP_JUMBO_FRAME_SIZE_LIMIT))});
    headerBuff.push_back(static_cast<std::byte>(command));

    if (dataSize >= MSP_JUMBO_FRAME_SIZE_LIMIT) {
        append_range(headerBuff, msp_osd_utils::to_bytes(static_cast<uint16_t>(dataSize), true));
    }

    std::byte checksum = mspSerialChecksumBuf(std::span<std::byte>(headerBuff.begin() + 3, headerBuff.end()));
    checksum = mspSerialChecksumBuf(dataBuff, checksum);

    msp_osd_buffer res;
    std::move(headerBuff.begin(), headerBuff.end(), std::back_inserter(res));
    std::move(dataBuff.begin(), dataBuff.end(), std::back_inserter(res));
    res.push_back(checksum);

    return res;
}

msp_osd_buffer MspEncoder::encodeV2overV1(msp_osd_buffer& headerBuff, const msp_osd_buffer& dataBuff,
                                          const MspProtocolCommand command) const {
    const auto payloadSize = 5 + dataBuff.size() + 1;  // MSPv2 header + data payload + MSPv2 checksum

    headerBuff.push_back(std::byte{std::min(payloadSize, static_cast<std::size_t>(MSP_JUMBO_FRAME_SIZE_LIMIT))});
    headerBuff.push_back(static_cast<std::byte>(MspProtocolCommand::V2_FRAME_ID));

    msp_osd_buffer headerV2Buff {
        std::byte{},  // flags byte
    };
    append_range(headerV2Buff, msp_osd_utils::to_bytes(static_cast<uint16_t>(command), true));
    append_range(headerV2Buff, msp_osd_utils::to_bytes(static_cast<uint16_t>(dataBuff.size()), true));
    append_range(headerBuff, headerV2Buff);

    if (payloadSize >= MSP_JUMBO_FRAME_SIZE_LIMIT) {
        append_range(headerBuff, msp_osd_utils::to_bytes(static_cast<uint16_t>(payloadSize), true));
    }

    std::byte v2_checksum = crc8DvbS2Update(headerV2Buff);
    v2_checksum = crc8DvbS2Update(dataBuff, v2_checksum);

    std::byte v1_checksum = mspSerialChecksumBuf(std::span<std::byte>(headerBuff.begin() + 3, headerBuff.end()));
    v1_checksum = mspSerialChecksumBuf(dataBuff, v1_checksum);
    v1_checksum = mspSerialChecksumBuf(msp_osd_buffer {v2_checksum}, v1_checksum);

    msp_osd_buffer res;
    std::move(headerBuff.begin(), headerBuff.end(), std::back_inserter(res));
    std::move(dataBuff.begin(), dataBuff.end(), std::back_inserter(res));
    res.push_back(v2_checksum);
    res.push_back(v1_checksum);
    return res;
}

msp_osd_buffer MspEncoder::encodeV2(msp_osd_buffer& headerBuff, const msp_osd_buffer& dataBuff,
                                    const MspProtocolCommand command) const {
    /*
    Header:
        uint8_t  flags;
        uint16_t cmd;
        uint16_t size;
    */
    headerBuff.push_back(std::byte{});  // flags byte
    append_range(headerBuff, msp_osd_utils::to_bytes(static_cast<uint16_t>(command), true));
    append_range(headerBuff, msp_osd_utils::to_bytes(static_cast<uint16_t>(dataBuff.size()), true));

    std::byte checksum = crc8DvbS2Update(std::span<std::byte>(headerBuff.begin() + 3, headerBuff.end()));
    checksum = crc8DvbS2Update(dataBuff, checksum);

    msp_osd_buffer res;
    std::move(headerBuff.begin(), headerBuff.end(), std::back_inserter(res));
    std::move(dataBuff.begin(), dataBuff.end(), std::back_inserter(res));
    res.push_back(checksum);
    return res;
}


MspWriter::MspWriter(int fd):
    _fd(fd) {
}

bool MspWriter::write(const msp_osd_buffer& buffer) const {
	int packetSize = buffer.size();
	return ::write(_fd, reinterpret_cast<const char*>(buffer.data()), packetSize) == packetSize;
}

bool MspWriter::write(const std::vector<msp_osd_buffer>& buffers) const {
    bool res = true;
    for (const auto& buffer : buffers) {
        res &= write(buffer);
    }
    return res;
}
