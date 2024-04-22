#pragma once

#include <map>
#include <set>
#include <vector>
#include "fc.hpp"
#include "osd.hpp"
#include "utils.hpp"


// select bank of 256 characters as per MspOsdFontSeverity
constexpr uint8_t MSP_ATTR_FONT = msp_osd_utils::bit(0) | msp_osd_utils::bit(1);
constexpr uint8_t MSP_ATTR_BLINK = msp_osd_utils::bit(6);

constexpr uint8_t MSP_JUMBO_FRAME_SIZE_LIMIT = 255;
constexpr uint8_t MSP_OSD_MAX_STRING_LENGTH = 30;


using msp_osd_buffer = std::vector<std::byte>;


enum struct MspVersion: uint8_t {
	V1 = 0,
	V2_OVER_V1 = 1,
	V2 = 2,
};
const std::map<MspVersion, char> mspVersionMagicInitializer {
    {MspVersion::V1, 'M'},
    {MspVersion::V2_OVER_V1, 'M'},
    {MspVersion::V2, 'X'},
};

enum struct MspDirection : uint8_t {
    REPLY = 0,
    REQUEST = 1,
};

enum struct MspResultStatus : int8_t {
    ACK = 1,
    ERROR = -1,
    NO_REPLY = 0,
    CMD_UNKNOWN = -2,
};

enum struct MspProtocolCommand : uint8_t {
    STATUS = 101,
    DISPLAYPORT = 182,
    V2_FRAME_ID = 255,
};

enum struct MspCommand : uint8_t {
    HEARTBEAT = 0,
    RELEASE = 1,  // release the display after clearing and updating
    CLEAR_SCREEN = 2,
    WRITE_STRING = 3,
    DRAW_SCREEN = 4,
    OPTIONS = 5,
    SYS = 6,  // display system element at given coordinates
};

enum struct MspOsdFontSeverity : uint8_t {
    NORMAL = 0,
    INFO = 1,
    WARNING = 2,
    CRITICAL = 3,
};

// system elements rendered by VTX or goggles
enum struct MspOsdSystemElementType : uint8_t {
    GOGGLE_VOLTAGE = 0,
    VTX_VOLTAGE = 1,
    BITRATE = 2,
    DELAY = 3,
    DISTANCE = 4,
    LQ = 5,
    GOGGLE_DVR = 6,
    VTX_DVR = 7,
    WARNINGS = 8,
    VTX_TEMP = 9,
    FAN_SPEED = 10,
};

const std::map<FcSensorFlag, int> mspSensorShiftMap {
    {FcSensorFlag::ACC, 0},
    {FcSensorFlag::BARO, 1},
    {FcSensorFlag::MAG, 2},
    {FcSensorFlag::GPS, 3},
    {FcSensorFlag::RANGEFINDER, 4},
    {FcSensorFlag::GYRO, 5},
};

struct MspStatus {
    uint16_t time = 0;  // microseconds
    std::set<FlightModeFlag> flightModes{};
    std::set<FcSensorFlag> sensors{FcSensorFlag::ACC, FcSensorFlag::BARO, FcSensorFlag::GYRO};
    uint16_t averageSystemLoadPercent = 0;
    uint16_t errorsCount = 0;
    uint8_t pidProfileCount = 0;
    uint8_t pidProfile = 0;
    uint8_t controlRateProfile = 0;
};

struct MspOsdElementBase {
    virtual ~MspOsdElementBase() = default;
    uint8_t row = 0;
    uint8_t column = 0;
};

struct MspOsdSysElement : MspOsdElementBase {
    virtual ~MspOsdSysElement() = default;
    MspOsdSystemElementType type;
};

struct MspOsdElement : MspOsdElementBase {
    virtual ~MspOsdElement() = default;
    MspOsdElement(const OsdElement& element);
    std::string value = "";
    MspOsdFontSeverity fontSeverity = MspOsdFontSeverity::NORMAL;
    bool blink = false;
};


class MspEncoder {
public:
    MspEncoder(MspVersion version = MspVersion::V1);
    template<typename T>
    std::enable_if_t<!std::is_base_of_v<OsdObject, T>, msp_osd_buffer>
    encode(const T& object) const;
    msp_osd_buffer encode(const OsdElement& element) const;
    std::vector<msp_osd_buffer> encode(const OsdObject& object) const;
private:
    MspVersion _version = MspVersion::V1;

    msp_osd_buffer encodeDispatch(msp_osd_buffer& headerBuff, const msp_osd_buffer& dataBuff,
                                  const MspProtocolCommand command = MspProtocolCommand::DISPLAYPORT) const;
    msp_osd_buffer encodeV1(msp_osd_buffer& headerBuff, const msp_osd_buffer& dataBuff,
                            const MspProtocolCommand command) const;
    msp_osd_buffer encodeV2overV1(msp_osd_buffer& headerBuff, const msp_osd_buffer& dataBuff,
                                  const MspProtocolCommand command) const;
    msp_osd_buffer encodeV2(msp_osd_buffer& headerBuff, const msp_osd_buffer& dataBuff,
                            const MspProtocolCommand command) const;
    msp_osd_buffer createDataBuffer(const MspCommand command) const;
    msp_osd_buffer createDataBuffer(const MspStatus status) const;
    msp_osd_buffer createDataBuffer(const MspOsdSysElement& element) const;
    msp_osd_buffer createDataBuffer(const MspOsdElement& element) const;
    msp_osd_buffer createHeaderBuffer(const MspResultStatus resultStatus = MspResultStatus::ACK) const;
    template<typename T> MspProtocolCommand getProtocolCommand(const T& object) const;
};


class MspWriter {
public:
    MspWriter(int fd);
    bool write(const msp_osd_buffer& buffer) const;
    bool write(const std::vector<msp_osd_buffer>& buffers) const;
private:
    int _fd = -1;
};


template<typename T>
std::enable_if_t<!std::is_base_of_v<OsdObject, T>, msp_osd_buffer>
MspEncoder::encode(const T& object) const {
    msp_osd_buffer headerBuff = createHeaderBuffer();
    const msp_osd_buffer dataBuff = createDataBuffer(object);
    MspProtocolCommand command = getProtocolCommand(object);
    return encodeDispatch(headerBuff, dataBuff, command);
}

template<typename T>
MspProtocolCommand MspEncoder::getProtocolCommand(const T& object) const {
    if (std::is_same<T, MspStatus>::value) return MspProtocolCommand::STATUS;
    return MspProtocolCommand::DISPLAYPORT;
}
