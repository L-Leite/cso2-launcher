REM ask the user which master server the client should connect to
REM this script should be temporary until a better solution is created

@echo off
echo cso2-launcher for Nexon's Counter-Strike Online 2
echo .
echo Please enter the desired master server IP address
set /p ip=Master server's IP: 
echo Starting the game with "launcher.exe -masterip %ip%"
launcher.exe -masterip %ip%
