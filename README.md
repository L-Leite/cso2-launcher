# cso2-launcher

[![Build status](https://ci.appveyor.com/api/projects/status/6ehjwfyekyfd9ipx/branch/master?svg=true)](https://ci.appveyor.com/project/Ochii/cso2-launcher/branch/master)

A custom client launcher for Nexon's Counter-Strike: Online 2, written in C++.

Ready for [this master server](https://github.com/Ochii/cso2-master-server/)

**Any help** towards the project is welcome and appreciated.

## Installation

### Obtaining CSO2

You can grab a copy of **Nexon's Counter-Strike: Online 2** at [MEGA](https://mega.nz/#!nhgnBJgD!iR57D5Mf3_1GCcAR36tqFQ7H7KN_F0e3XicD2JBoSN4) or [Google Drive](https://drive.google.com/open?id=1y0diL2nTERlOaJZQTA3xPb8owx82GjtB). 

```SHA-256: 02F76DBDD083EF78ECC92EB963C7C7A7576FF2E2C695671A4F358C2580584965```

### Obtaining the launcher

- Download a copy of the [launcher](https://github.com/Ochii/cso2-launcher/releases);
- Then, extract the files inside to ```[your CSO2 game folder]/Bin```.

## Starting the game

*Note: **a master server has to be running somewhere**.*

Run ```start-cso2.bat``` and enter the master server's IP address to start the game.

### Lauching

*The translations are a courtesy of **[Frostie](#thank-yous)**.*

- When the game loads enter some login information - only your username will be used, so you can use any password ([image](https://puu.sh/Bw7hu/ffba6ca218.png));
- Select *커스텀모드* (*custom mode*) to go to the channel menu ([image](https://puu.sh/C1Nno/7122e0fefd.png));
- Pick a channel ([image](https://puu.sh/C1NpM/adead9596e.png))
- Create your own room by pressing *방만들기* (*create a room*) ([image](https://puu.sh/C1NrH/5cbaa524f8.png)) then choosing game mode ([image](https://puu.sh/C1NSv/8023bd39e8.png));
- You can now start the match by pressing *게임 시작* (*game start*) ([image](https://puu.sh/Bw7M3/9209eb7637.png)).

You can input Source Engine commands (such as ```cl_showfps 1```) in the console window.

### Available command line arguments

- ```-lang [some language]``` - Sets the game's language to *some language*'s (considering you have the language files)
- ```-masterip [some ip]``` - Sets the master server's IP address to *some ip*
- ```-masterport [some port]``` - Sets the master server's port number to *some port*

## Building

### Build requirements

- [CMake](https://cmake.org/download/)
- [Visual Studio 2017](https://www.visualstudio.com/downloads/)
- [Windows PowerShell](https://docs.microsoft.com/en-us/powershell/scripting/setup/installing-windows-powershell)

Currently only Visual Studio 2017 is supported.

### Starting the build

In a Powershell instance enter:

```powershell
./setuplibs.ps1 # setups dependencies
```

Open the solution ```CSO2Launcher.sln``` and build it in your preferred configuration.

If built successfully, you will find the launcher inside ```out/bin/Win32/[your configuration]```.

## Bug reporting

Have a look at the [issues](https://github.com/Ochii/cso2-launcher/issues) for a list of bugs found or to report them yourself.

## Pull requests

Improvements and suggestions are welcome, make one at ['pull requests'](https://github.com/Ochii/cso2-launcher/pulls).

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
