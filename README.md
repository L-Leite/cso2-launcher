# cso2-launcher

[![Build status](https://ci.appveyor.com/api/projects/status/6ehjwfyekyfd9ipx/branch/master?svg=true)](https://ci.appveyor.com/project/Ochii/cso2-launcher/branch/master)

A modified client launcher for Nexon's (South Korea) Counter-Strike: Online 2.

## How to use

### Requirements

- A copy of **Nexon's Counter-Strike: Online 2** (hosted in [MEGA](https://mega.nz/#!nhgnBJgD!iR57D5Mf3_1GCcAR36tqFQ7H7KN_F0e3XicD2JBoSN4) and [Google Drive](https://drive.google.com/open?id=1y0diL2nTERlOaJZQTA3xPb8owx82GjtB), SHA-256: 02F76DBDD083EF78ECC92EB963C7C7A7576FF2E2C695671A4F358C2580584965);
- This incomplete [emulated master server](https://github.com/Ochii/cso2-master-server/) running in the background;
- [Visual Studio 2017's C++ Redistributable](https://aka.ms/vs/15/release/vc_redist.x86.exe);
- And the [launcher's files](https://github.com/Ochii/cso2-launcher/releases) in ```[your CSO2 game path]/Bin```.

Keep in mind that the launcher released here will connect 127.0.0.1 (for now).

### Getting ingame

*The translations are a courtesy of **[Frostie](#thank-yous)**.*

- To start the launcher, run ```launcher_main.exe```;
- Once the game loads enter some login information - only your username will be used, and you can use any password ([example](https://puu.sh/Bw7hu/ffba6ca218.png));
- In order to host a room, choose the second option in the menu that reads *공식전* (translation: *competitive game*) ([example](https://puu.sh/Bw7tM/7c8298a7eb.png)), then pick the first option ([example](https://puu.sh/Bw7CS/17addcee32.png));
- If you wish to play with more players repeat the previous steps, otherwise you can start the match by pressing *게임 시작* (translation: *game start*) ([example](https://puu.sh/Bw7M3/9209eb7637.png)). 

The match options are unchangeable, and you can't do almost anything since the master server is incomplete.

There will be a console window where you can input commands just like a regular Source Engine console (you can enable ```cl_showfps```, for example).

## How to build

### Build requirements

- [CMake](https://cmake.org/download/)
- [Visual Studio 2017](https://www.visualstudio.com/downloads/)
- [Windows PowerShell](https://docs.microsoft.com/en-us/powershell/scripting/setup/installing-windows-powershell)

Only **Windows** is supported.

### Downloading the source code

Clone the project with ```git clone --recursive https://github.com/Ochii/cso2-launcher.git```.

### Before building

Start a PowerShell instance and change directory to the project's directory, then, run ```./setuplibs.ps1```.

The script will setup PolyHook, its dependency Capstone and the Source SDK.

### Building

Open the solution ```CSO2Launcher.sln``` and build it in your preferred configuration.

If built successfully, you will find your binaries inside ```out/bin/Win32/[your configuration]```.

## Bug reporting

Have a look at the [issues](https://github.com/Ochii/cso2-launcher/issues) for a list of bugs found or to report them yourself.

## Pull requests

If you have any improvements that you would like to share or comment about, have a look at the [pull requests](https://github.com/Ochii/cso2-launcher/pulls).

## Libraries used

- **[PolyHook v2.0](https://github.com/stevemk14ebr/PolyHook_2_0/)** by [stevemk14ebr](https://github.com/stevemk14ebr)

## Credits

- [Valve Software](https://github.com/ValveSoftware/source-sdk-2013) for their SDK

## Thank you's

- To **[UserU](https://www.youtube.com/user/GoodbyeSpy)**
- To **[SHI_KU](https://www.youtube.com/channel/UC2HZo-HFOuxmS6zWYPMD0hQ)**
- To **ReddUT** from [Counter-Strike Online Wikia](https://cso.wikia.com/)'s Discord
- To **Frostie** from [Counter-Strike Online Wikia](https://cso.wikia.com/)'s Discord 

## License

Read ```LICENSE``` for license information.

I'm not affiliated with either Valve and/or Nexon, neither I own Counter-Strike Online 2.
