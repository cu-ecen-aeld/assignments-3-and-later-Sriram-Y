#!/bin/sh

if [ "$1" = "start" ]; then
    echo "loading aesdchardriver server!!!"
    aesdchar_load
elif [ "$1" = "stop" ]; then
    echo "unloading aesdchardriver server!!!"
    aesdchar_unload
else
    echo "Usage: $0 <start|stop>"
    exit 1
fi

exit 0
