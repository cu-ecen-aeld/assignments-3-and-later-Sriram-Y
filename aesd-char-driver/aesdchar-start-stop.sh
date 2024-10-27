#!/bin/sh

if [ "$1" = "start" ]; then
    echo "loading aesd-char-driver!"
    start-stop-daemon -S -n aesdchar_load -a /usr/bin/aesdchar_load -- -d
elif [ "$1" = "stop" ]; then
    echo "unload aesd-char-driver!"
    start-stop-daemon -K -n aesdchar_unload -a /usr/bin/aesdchar_unload -- -d
else
    echo "Usage: $0 command <start|stop>"
    exit 1
fi
