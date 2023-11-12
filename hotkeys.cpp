#include "Hotkeys.hpp"

#include <future>
#include <iostream>

void Hotkeys::start_recording(Callback callback, int label)
{
    if (!isRecording_)
    {
        isRecording_ = true;
        callback_ = callback;
        key_states_.clear();
        recording_thread(label);
        //recordingThread_ = std::async(std::launch::async, &recording_thread, this);
    }
}

bool Hotkeys::get_hot_key_pressed(int vk_label)
{
    return TKeys::get_hot_key_pressed(label_map[vk_label]);
}

bool Hotkeys::get_hot_key_toggle(int vk_label)
{
    return TKeys::get_hot_key_toggle(label_map[vk_label]);
}

bool Hotkeys::get_hot_key_toggle_change(int vk_label)
{
    if (get_hot_key_toggle(vk_label) != toggle_map[vk_label])
    {
        toggle_map[vk_label] = get_hot_key_toggle(vk_label);
        return true;
    }
    return false;
}

bool Hotkeys::get_hot_key_changed(int vk_label)
{
    return TKeys::get_hot_key_changed(label_map[vk_label]);
}

bool Hotkeys::get_hot_key_full(int vk_label)
{
    return get_hot_key_toggle_change(vk_label) && !hold_map[vk_label] ||
           get_hot_key_changed(vk_label) && hold_map[vk_label];
}

void Hotkeys::set_hot_key_hold(int vk_label, bool hold)
{
    hold_map[vk_label] = hold;
}

void Hotkeys::toggle_hot_key_state(int vk_label)
{
    return TKeys::toggle_hot_key_state(label_map[vk_label]);
}

int Hotkeys::levenshtein_distance(const std::string &str1, const std::string &str2)
{
    int m = str1.length();
    int n = str2.length();

    std::vector<int> previous(n + 1);
    std::vector<int> current(n + 1);

    for (int j = 0; j <= n; j++)
    {
        previous[j] = j;
    }

    for (int i = 1; i <= m; i++)
    {
        current[0] = i;

        for (int j = 1; j <= n; j++)
        {
            if (str1[i - 1] == str2[j - 1])
            {
                current[j] = previous[j - 1];
            }
            else
            {
                current[j] = 1 + min(previous[j], min(current[j - 1], previous[j - 1]));
            }
        }

        std::swap(previous, current);
    }

    return previous[n];
}

Hotkeys::Hotkeys(int enum_num)
{
    hold_map_size = enum_num;
    hold_map = new bool[hold_map_size];
    for (int i = 0; i < hold_map_size; i++)
    {
        hold_map[i] = false;
    }
}

Hotkeys::~Hotkeys()
{
    delete[] hold_map;
}

unsigned Hotkeys::getKeyName(int key, char *buffer, unsigned bufferLength)
{
    unsigned int scancode = MapVirtualKey(key, MAPVK_VK_TO_VSC_EX);
    unsigned int result = 0;
    unsigned int extended = scancode & 0xffff00;
    unsigned int lParam = 0;

    if (extended)
    {

        if (extended == 0xE11D00)
        {
            lParam = 0x45 << 16;
        }
        else
        {
            lParam = (0x100 | (scancode & 0xff)) << 16;
        }
    }
    else
    {

        lParam = scancode << 16;

        if (scancode == 0x45)
        {
            lParam |= (0x1 << 24);
        }
    }

    result = GetKeyNameText(lParam, buffer, bufferLength);
    return result;
}

void Hotkeys::recording_thread(int label)
{
    auto startTime = std::chrono::steady_clock::now();
    bool release = false;
    while (isRecording_)
    {
        for (int key = 0; key < 256; ++key)
        {
            bool isKeyPressed = GetAsyncKeyState(key) & 0x8000;
            if (isKeyPressed && std::find(key_states_.begin(), key_states_.end(), key) == key_states_.end())
            {
                char buff_nk[128];
                getKeyName(key, buff_nk, 128);
                if (key_states_.size() != 0)
                {
                    bool found_substr = false;
                    for (int i = 0; i < key_states_.size(); i++)
                    {
                        char buff_k[128];
                        getKeyName(key_states_[i], buff_k, 128);
                        if (std::string(buff_nk) == std::string(buff_k))
                        {
                            found_substr = true;
                        }
                    }
                    if (!found_substr)
                    {
                        char buff_tt[128];
                        getKeyName(key_states_[0], buff_tt, 128);
                        key_states_.push_back(key);
                    }
                }
                else
                {
                    key_states_.push_back(key);
                }
            }
            if (!isKeyPressed && std::find(key_states_.begin(), key_states_.end(), key) != key_states_.end())
            {
                release = true;
            }
        }

        auto currentTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();

        if (key_states_.size() >= 3 || duration >= 10 || release)
        {
            isRecording_ = false;
            label_map[label] = key_states_;
            add_hot_key(key_states_);
            callback_(key_states_, label);
        }
    }
}
