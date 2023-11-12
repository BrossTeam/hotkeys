#include "t_keys.hpp"

TKeys::TKeys(void){
}

void TKeys::scan_keys(){
	for (KeyMap::iterator it = keysPressed.begin(); it != keysPressed.end(); ++it) {
		ULONG curKey = it->first;
		bool pressed = (GetAsyncKeyState(curKey % 0x100) & 0x8000) != 0; // Single Key

		while (curKey > 0xFF) { // iterate remaining bytes
			curKey = curKey >> 8;
			pressed = pressed && ((GetAsyncKeyState(curKey % 0x100) & 0x8000) != 0);
		}

		if (pressed) { // Key is Pressed
			if (!(it->second)) { //If not Pressed Previously
				it->second = true;
				keysToggle.find(it->first)->second = !(keysToggle.find(it->first)->second);
				keysChanged.find(it->first)->second = true;
			}
			else {
				keysChanged.find(it->first)->second = false;
			}
		}
		else { //Key is not Pressed
			if (it->second) {
				it->second = false;
				keysChanged.find(it->first)->second = true;
			}
			else {
				keysChanged.find(it->first)->second = false;
			}
		}
	}
}

void TKeys::add_hot_key(ULONG vkCode){
	KeyMap::iterator it = keysToggle.find(vkCode);
	if (it == keysToggle.end()) {
		keysPressed.insert(std::pair<ULONG, bool>(vkCode, false));
		keysToggle.insert(std::pair<ULONG, bool>(vkCode, false));
		keysChanged.insert(std::pair<ULONG, bool>(vkCode, false));
	}
}

void TKeys::add_hot_key(KeyCodes vkCodes){
	ULONG comboVK = 0;
	for (size_t i = 0; i < vkCodes.size(); ++i) {
		comboVK += vkCodes.at(i) << (i * 8);
	}
	add_hot_key(comboVK);
}

bool TKeys::get_hot_key_toggle(ULONG vkCode){
	KeyMap::iterator it = keysToggle.find(vkCode);
	if (it != keysToggle.end()) {
		return it->second;
	}
	return false;
}

bool TKeys::get_hot_key_toggle(KeyCodes vkCodes){
	ULONG comboVK = 0;
	for (size_t i = 0; i < vkCodes.size(); ++i) {
		comboVK += vkCodes.at(i) << (i * 8);
	}
	return get_hot_key_toggle(comboVK);
}

bool TKeys::get_hot_key_pressed(ULONG vkCode){
	KeyMap::iterator it = keysPressed.find(vkCode);
	if (it != keysPressed.end()) {
		return it->second;
	}
	return false;
}

bool TKeys::get_hot_key_pressed(KeyCodes vkCodes) {
	ULONG comboVK = 0;
	for (size_t i = 0; i < vkCodes.size(); ++i) {
		comboVK += vkCodes.at(i) << (i * 8);
	}
	return get_hot_key_pressed(comboVK);
}

bool TKeys::get_hot_key_changed(ULONG vkCode) {
	KeyMap::iterator it = keysChanged.find(vkCode);
	if (it != keysChanged.end()) {
		return it->second;
	}
	return false;
}

bool TKeys::get_hot_key_changed(KeyCodes vkCodes) {
	ULONG comboVK = 0;
	for (size_t i = 0; i < vkCodes.size(); ++i) {
		comboVK += vkCodes.at(i) << (i * 8);
	}
	return get_hot_key_changed(comboVK);
}

void TKeys::toggle_hot_key_state(ULONG vkCode){
	KeyMap::iterator it = keysToggle.find(vkCode);
	if (it != keysToggle.end()) {
		it->second = !it->second;
	}
}

void TKeys::toggle_hot_key_state(KeyCodes vkCodes){
	ULONG comboVK = 0;
	for (size_t i = 0; i < vkCodes.size(); ++i) {
		comboVK += vkCodes.at(i) << (i * 8);
	}
	toggle_hot_key_state(comboVK);
}

#pragma region Keys_related

void TKeys::key_press(UINT kSC) {
	INPUT kinp;
	kinp.type = INPUT_KEYBOARD;
	kinp.ki.time = 0;
	kinp.ki.wVk = 0;
	kinp.ki.dwExtraInfo = 0;

	kinp.ki.dwFlags = KEYEVENTF_SCANCODE;
	kinp.ki.wScan = kSC;

	SendInput(1, &kinp, sizeof(INPUT));
}

void TKeys::key_up(UINT kSC) {
	INPUT kinp;
	kinp.type = INPUT_KEYBOARD;
	kinp.ki.time = 0;
	kinp.ki.wVk = 0;
	kinp.ki.dwExtraInfo = 0;

	kinp.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
	kinp.ki.wScan = kSC;

	SendInput(1, &kinp, sizeof(INPUT));
}

void TKeys::key_sequence(std::wstring keys) {
	std::this_thread::sleep_for(std::chrono::milliseconds(50 + rand() % 100));
	for (size_t j = 0; j < keys.size(); ++j) {

		short kVK = VkKeyScan(keys.at(j));
		bool upperCase = HIBYTE(kVK) == 1;
		UINT kSC = MapVirtualKey(LOBYTE(kVK), MAPVK_VK_TO_VSC);

		if (upperCase) { // Press shift
			key_press(MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC));
			std::this_thread::sleep_for(std::chrono::milliseconds(50 + rand() % 100));
		}
		key_press(kSC);
		key_up(kSC);
		if (upperCase) { // Release shift
			key_up(MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC));
			std::this_thread::sleep_for(std::chrono::milliseconds(50 + rand() % 100));
		}else{
			std::this_thread::sleep_for(std::chrono::milliseconds(50 + rand() % 50));
		}
	}
}

void TKeys::key_automate(std::wstring accName, std::wstring passwd) {

	std::this_thread::sleep_for(std::chrono::milliseconds(600 + rand() % 150));

	key_sequence(accName);

	std::this_thread::sleep_for(std::chrono::milliseconds(100 + rand() % 150));

	UINT kSC = MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);
	key_press(kSC);
	key_up(kSC);

	std::this_thread::sleep_for(std::chrono::milliseconds(100 + rand() % 150));

	key_sequence(passwd);

	std::this_thread::sleep_for(std::chrono::milliseconds(100 + rand() % 150));

	key_press(kSC);
	key_up(kSC);

	return;
}

#pragma endregion Keys_Related 