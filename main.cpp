// TKeysDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "t_keys.hpp"
#include <chrono>

int hotkey_test(){
	TKeys tK;

	std::vector<ULONG> hotKeys = {
		VK_UP, 
		VK_DOWN, 
		VK_LEFT, 
		VK_RIGHT,
		VK_LMENU | 'K' << 8,
		VK_LMENU | 'I' << 8,
		VK_LMENU | VK_LSHIFT | 'F' << 8, 
	};

	std::vector<std::wstring> hotKeyNames = {
		L"Up",
		L"Down",
		L"Left",
		L"Right",
		L"Alt+K",
		L"Alt+I",
		L"Alt+Shift+F"
	};

	for (size_t i = 0; i < hotKeys.size(); ++i) {
		tK.add_hot_key(hotKeys.at(i));
	}

	//// Alternatively, manually add each one of them
	//tK.addHotKey(VK_UP);
	//tK.addHotKey(VK_DOWN);
	//tK.addHotKey(VK_LEFT);
	//tK.addHotKey(VK_RIGHT);
	//tK.addHotKey({ VK_LMENU,'Z' });
	//tK.addHotKey({ VK_LMENU,'I' });

    auto start_time = std::chrono::high_resolution_clock::now();

	while (true) {
		auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

		if (duration.count() > 5000)
		{
            tK.scan_keys(); // Read hotkey
            std::cout << "";
		}

		for (size_t i = 0; i < hotKeys.size(); ++i) { // iterate through the hotKeys
			if (tK.get_hot_key_changed(hotKeys.at(i))) { // if key state changed
				std::wcout << hotKeyNames.at(i).c_str() << L": [" 
					<< L"State -> "
					<< (tK.get_hot_key_pressed(hotKeys.at(i)) ? L"Pressed" : L"Released")
					<< L", Toggle -> "
					<< (tK.get_hot_key_toggle(hotKeys.at(i)) ? L"On" : L"Off")
					<< L"]" << std::endl;
			}
		}
		////Alternatively, manually process each of them
		//if (tK.getHotKeyChanged(VK_UP)) {
		//	std::wcout << L"UP:" << (tK.getHotKeyPressed(VK_UP) ? L"Pressed" : L"Released") << std::endl;
		//}
		//if (tK.getHotKeyChanged(VK_DOWN)) {
		//	std::wcout << L"DOWN:" << (tK.getHotKeyPressed(VK_DOWN) ? L"Pressed" : L"Released") << std::endl;
		//}
		//if (tK.getHotKeyChanged(VK_LEFT)) {
		//	std::wcout << L"LEFT:" << (tK.getHotKeyPressed(VK_LEFT) ? L"Pressed" : L"Released") << std::endl;
		//}
		//if (tK.getHotKeyChanged(VK_RIGHT)) {
		//	std::wcout << L"RIGHT:" << (tK.getHotKeyPressed(VK_RIGHT) ? L"Pressed" : L"Released") << std::endl;
		//}

		if (tK.get_hot_key_pressed({ VK_LMENU,'K' }) && tK.get_hot_key_changed({ VK_LMENU,'K' })) {
			// Enter key sequences automatically in a separate thread
			std::thread t1(&TKeys::key_sequence, tK,
				L"Some string that will be entered automatically");
			t1.detach();
		}

		if (tK.get_hot_key_pressed({ VK_LMENU,'I' }) && tK.get_hot_key_changed({ VK_LMENU,'I' })) {
			// Enter account & password automatically in a separate thread
			std::thread t1(&TKeys::key_automate, tK,
				L"testACC123@Lol.com.jk",
				L"SomePassword?-+=\\|<>,./;:'[]{}\"!@#$%^&*()");
			t1.detach();
		}

		if (tK.get_hot_key_toggle({ VK_LMENU,'X' })) { // Toggle initially false, toggles after press
			std::wcout << L"Alt+Z Pressed, Exiting" << std::endl;
			break;
		}

		Sleep(33);
	}
    return 0;
}

const int HOTKEY_ID = 1;

#include <functional>
#include <iostream>
#include <thread>
#include <vector>
#include <windows.h>

class KeyRecorder
{
  public:
    KeyRecorder(std::function<void(const std::vector<int> &)> callback) : callback_(callback)
    {
    }

    void StartRecording()
    {
        isRecording_ = true;
        keyStates_.clear();
        recordingThread_ = std::thread(&KeyRecorder::RecordingThread, this);
    }

  private:
    std::function<void(const std::vector<int> &)> callback_;
    std::vector<int> keyStates_;
    std::thread recordingThread_;
    bool isRecording_ = false;

    void RecordingThread()
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
};

// Example usage:
int main()
{
    KeyRecorder keyRecorder([](const std::vector<int> &keys) {
        std::cout << "Key combination recorded: ";
        for (int key : keys)
        {
            std::cout << key << " ";
        }
        std::cout << std::endl;
    });

    keyRecorder.StartRecording();

    // Sleep to allow time for recording.
    std::this_thread::sleep_for(std::chrono::seconds(100));

    return 0;
}
