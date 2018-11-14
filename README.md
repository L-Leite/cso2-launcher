# cso2-launcher

[![Build status](https://ci.appveyor.com/api/projects/status/6ehjwfyekyfd9ipx/branch/master?svg=true)](https://ci.appveyor.com/project/Ochii/cso2-launcher/branch/master)

A custom client launcher for Nexon's (South Korea) Counter-Strike: Online 2.

## How to use

### Requirements

- A copy of **Nexon's Counter-Strike: Online 2** (hosted in [MEGA](https://mega.nz/#!nhgnBJgD!iR57D5Mf3_1GCcAR36tqFQ7H7KN_F0e3XicD2JBoSN4) and [Google Drive](https://drive.google.com/open?id=1y0diL2nTERlOaJZQTA3xPb8owx82GjtB), SHA-256: 02F76DBDD083EF78ECC92EB963C7C7A7576FF2E2C695671A4F358C2580584965);
- This incomplete [emulated master server](https://github.com/Ochii/cso2-master-server/) running in the background;
- [Visual Studio 2017's C++ Redistributable](https://aka.ms/vs/15/release/vc_redist.x86.exe);
- And the [launcher's files](https://github.com/Ochii/cso2-launcher/releases) in ```[your CSO2 game path]/Bin```.

Keep in mind that the launcher released here will connect 127.0.0.1 (for now).

### Getting ingame

*The translations are a courtesy of **[Frostie](#thank-yous)**.*

- Start the launcher with ```launcher_main.exe``` with the [optional arguments](#available-command-line-arguments) if necessary;
- Once the game loads enter some login information - only your username will be used, and you can use any password ([example](https://puu.sh/Bw7hu/ffba6ca218.png));
- Select *커스텀모드* (*custom mode*) to go to the channel menu ([example](https://puu.sh/C1Nno/7122e0fefd.png));
- Pick a channel ([example](https://puu.sh/C1NpM/adead9596e.png))
- Create your own room by pressing *방만들기* (*room*) ([example](https://puu.sh/C1NrH/5cbaa524f8.png)) then choosing game mode ([example](https://puu.sh/C1NSv/8023bd39e8.png));
- If you wish, you can change **some** room options ([example](https://puu.sh/C1Nti/94a63556ee.png));
- You can now start the match by pressing *게임 시작* (*game start*) ([example](https://puu.sh/Bw7M3/9209eb7637.png)). 

Again, some room options **cannot be changed** at the moment. 

You **can't distinguish between rooms in menu or in the middle of a match**.

There will be a console window where you can input commands just like a regular Source Engine console (you can enable ```cl_showfps```, for example).

### Available command line arguments
- ```-lang [some language]``` - Sets the game's language to *some language*'s (considering you have right language files)
- ```-masterip [some ip]``` - Sets the master server's IP address to *some ip*
- ```-masterport [some port]``` - Sets the master server's port number to *some port*

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

- [GEEKiDoS](https://github.com/GEEKiDoS)
- [Valve Software](https://github.com/ValveSoftware/source-sdk-2013)

## Thank you's

- To **[UserU](https://www.youtube.com/user/GoodbyeSpy)**
- To **[SHI_KU](https://www.youtube.com/channel/UC2HZo-HFOuxmS6zWYPMD0hQ)**
- To **ReddUT** from [Counter-Strike Online Wikia](https://cso.wikia.com/)'s Discord
- To **Frostie** from [Counter-Strike Online Wikia](https://cso.wikia.com/)'s Discord 

## License

Read ```LICENSE``` for license information.

I'm not affiliated with either Valve and/or Nexon, neither I own Counter-Strike Online 2.
