#!/bin/bash

# Compile the C code
gcc -o cew cew.c -lcurl -ljansson

# Set the path to your C executable
EXECUTABLE=./cew

while true; do
    # Run the C code to fetch weather data and parse it
    $EXECUTABLE

    # Sleep for 10 seconds before running again
    sleep 10
done
