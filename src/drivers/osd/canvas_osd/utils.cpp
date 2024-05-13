#include "utils.hpp"


namespace msp_osd_utils {
    int decidegrees_to_degrees(int angle) {
        return angle / 10;
    }


    SyncTimer::SyncTimer() {
        tick();
    }

    void SyncTimer::schedule(std::function<void()> callback, int delay) {
        callbacks.push_back({callback, currentTime + std::chrono::milliseconds(delay)});
    }

    void SyncTimer::tick() {
        currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());
        for (auto callback_it = callbacks.begin(); callback_it != callbacks.end();) {
            auto callback = *callback_it;
            if (currentTime > callback.second) {
                callback.first();
                callback_it = callbacks.erase(callback_it);
            } else {
                callback_it++;
            }
        }
    }
}
