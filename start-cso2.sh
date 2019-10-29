#!/bin/bash
#ask the user which master server the client should connect to
#this script should be temporary until a better solution is created

echo "cso2-launcher for Nexon's Counter-Strike Online 2"
echo ""
echo "Please enter the desired master server IP address"
echo "Master server's IP: "
read masterip
echo Starting the game with "launcher.exe -masterip" "$masterip"
wine launcher.exe -masterip "$masterip"
