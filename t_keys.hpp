#pragma once

#include <Map>
#include <vector>
#include <thread>
#include <string>
#include <Windows.h>

typedef std::map<ULONG, bool> KeyMap;
typedef std::vector<int> KeyCodes;

class TKeys {
public:
	TKeys(void);

	void scan_keys();

	void add_hot_key(ULONG vkCode);
	void add_hot_key(KeyCodes vkCodes);
	
	bool get_hot_key_toggle(ULONG vkCode);
	bool get_hot_key_toggle(KeyCodes vkCodes);

	bool get_hot_key_pressed(ULONG vkCode);
	bool get_hot_key_pressed(KeyCodes vkCodes);

	bool get_hot_key_changed(ULONG vkCode);
	bool get_hot_key_changed(KeyCodes vkCodes);

	void toggle_hot_key_state(ULONG vkCode);
	void toggle_hot_key_state(KeyCodes vkCodes);

	void key_press(UINT kSC);
	void key_up(UINT kSC);
	void key_sequence(std::wstring keys);
	void key_automate(std::wstring accName, std::wstring passwd);
	
protected:
	KeyMap keysPressed;
	KeyMap keysToggle;
	KeyMap keysChanged;
};
