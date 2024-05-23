#!/bin/bash

cleanup() {
    echo "Cleaning up..."
    pkill -f "$OUTPUT_FILE"
    exit 0
}


trap cleanup EXIT


echo "Sleeping for 30 seconds..."
sleep 30

BASE_DIR="/home/g10/Desktop/server/ipshow"
SOURCE_FILE="$BASE_DIR/ipshower.cc"
OUTPUT_FILE="$BASE_DIR/ipshower"
LAN_IP_FILE="$BASE_DIR/laniptest.txt"

echo "BASE_DIR: $BASE_DIR"
echo "SOURCE_FILE: $SOURCE_FILE"
echo "OUTPUT_FILE: $OUTPUT_FILE"
echo "LAN_IP_FILE: $LAN_IP_FILE"

if [ ! -f "$SOURCE_FILE" ]; then
    echo "Source file not found: $SOURCE_FILE" >&2
    exit 1
fi

LAN_IP=$(ip addr show wlan0 | grep "inet " | awk '{print $2}' | cut -d'/' -f1)
echo "LAN_IP: $LAN_IP"
echo "$LAN_IP" > "$LAN_IP_FILE"

echo "Compiling $SOURCE_FILE..."
g++ -o "$OUTPUT_FILE" "$SOURCE_FILE" -I/home/g10/rpi-rgb-led-matrix/include -L/home/g10/rpi-rgb-led-matrix/lib -lrgbmatrix -lpthread

if [ $? -eq 0 ]; then
    echo "Compilation successful. Running $OUTPUT_FILE..."

    sudo "$OUTPUT_FILE"
else
    echo "Compilation failed." >&2
    exit 1
fi
