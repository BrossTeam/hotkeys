#pragma once
#include "t_keys.hpp"

#include <functional>

class HotKeys: TKeys
{
public:
    std::map<KeyCodes, std::string> label_map;
    void start_recording();
private:
    std::function<void(const std::vector<int> &)> callback_;
    std::vector<int> keyStates_;
    std::thread recordingThread_;
    bool isRecording_ = false;
    void recording_thread();
};
