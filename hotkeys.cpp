#include "hotkeys.hpp"

void HotKeys::start_recording()
{
    isRecording_ = true;
    keyStates_.clear();
    recordingThread_ = std::thread(&recording_thread, this);
}

void HotKeys::recording_thread()
{
    auto startTime = std::chrono::steady_clock::now();
    while (isRecording_)
    {
        for (int key = 0; key < 256; ++key)
        {
            bool isKeyPressed = GetAsyncKeyState(key) & 0x8000;
            if (isKeyPressed && std::find(keyStates_.begin(), keyStates_.end(), key) == keyStates_.end())
            {
                keyStates_.push_back(key);
            }
        }

        auto currentTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();

        if (keyStates_.size() >= 3 || duration >= 6)
        {
            isRecording_ = false;
            callback_(keyStates_);
        }
    }
}
