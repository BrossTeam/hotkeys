#pragma once

#include "t_keys.hpp"
#include <functional>
#include <future>

class Hotkeys: public TKeys
{
public:
    Hotkeys(int enum_num);
    ~Hotkeys();
    using Callback = std::function<void(KeyCodes &, int &)>;
    std::map<int, KeyCodes> label_map;
    std::map<int, bool> toggle_map;
    bool* hold_map;
    int hold_map_size;

    unsigned int getKeyName(int key, char *buffer, unsigned int bufferLength);
    void start_recording(Callback callback, int label);
    bool get_hot_key_pressed(int vk_label);
    bool get_hot_key_toggle(int vk_label);
    bool get_hot_key_toggle_change(int vk_label);
    bool get_hot_key_changed(int vk_label);
    bool get_hot_key_full(int vk_label);
    void set_hot_key_hold(int vk_label, bool hold);
    void toggle_hot_key_state(int vk_label);

private:
    int levenshtein_distance(const std::string &str1, const std::string &str2);
    KeyCodes key_states_;
    Callback callback_;
    std::future<void> recordingThread_;
    bool isRecording_ = false;
    void recording_thread(int label);
};
