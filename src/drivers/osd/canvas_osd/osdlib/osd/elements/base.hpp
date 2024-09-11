#pragma once

#include <memory>
#include <stdint.h>
#include <string>
#include <vector>
#include "../symbols.hpp"


struct OsdPosition {
    uint8_t x = 0;
    uint8_t y = 0;
};


struct OsdElement {
    OsdPosition position = {};
    std::string value = "";
    OsdFontLevel fontLevel = OsdFontLevel::NORMAL;
    bool blink = false;
};


struct OsdObjectConfig {
    OsdObjectConfig() = default;
    virtual ~OsdObjectConfig() = default;
};


class OsdObject {
public:
    virtual ~OsdObject() = default;
    OsdPosition position = {};
    bool enabled = true;

    virtual bool configure(const std::shared_ptr<OsdObjectConfig>& config);
    virtual const std::vector<OsdElement> elements() const = 0;
    void setBlink(bool value);
    bool shouldBlink() const;
protected:
    OsdObject() = default;
    bool shouldNativeBlink() const;
    std::shared_ptr<OsdObjectConfig> config = std::make_shared<OsdObjectConfig>();
private:
    bool blink = false;
};

template<class _Config = OsdObjectConfig>
class OsdObjectConfigMixin : virtual public OsdObject {
static_assert(std::is_base_of_v<OsdObjectConfig, _Config>, "Config must inherit from OsdObjectConfig");
public:
    virtual bool configure(const std::shared_ptr<OsdObjectConfig>& config) {
        if (!config) return false;

        auto localConfig = std::dynamic_pointer_cast<_Config>(config);
        if (!localConfig) return false;

        this->config = localConfig;
        return OsdObject::configure(config);
    }
protected:
    OsdObjectConfigMixin() = default;
    std::shared_ptr<_Config> config = std::make_shared<_Config>();
};
