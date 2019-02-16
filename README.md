# cso2-launcher

[![Build status](https://ci.appveyor.com/api/projects/status/6ehjwfyekyfd9ipx/branch/master?svg=true)](https://ci.appveyor.com/project/Ochii/cso2-launcher/branch/master)

A custom client launcher for Nexon's Counter-Strike: Online 2, written in C++.

Ready for [this CSO2 master server](https://github.com/Ochii/cso2-master-server/).

**Any help** towards the project is welcomed and appreciated.

## Installation

### Before starting

You ***must*** **allow `launcher.exe` in your firewall** and, either **port forward ports 27015 to 27020** or **open your router's NAT**, in order to play with others.

If you host a room match, **your IP will be known to other clients**.

If you join a host's room match, **your IP will be known to the host**.

This might be avoidable in the future once a dedicated server launcher is developed.

### Obtaining CSO2

You can grab a copy of **Nexon's Counter-Strike: Online 2** at [MEGA](https://mega.nz/#!nhgnBJgD!iR57D5Mf3_1GCcAR36tqFQ7H7KN_F0e3XicD2JBoSN4) or [Google Drive](https://drive.google.com/open?id=1y0diL2nTERlOaJZQTA3xPb8owx82GjtB). 

```SHA-256: 02F76DBDD083EF78ECC92EB963C7C7A7576FF2E2C695671A4F358C2580584965```

### Obtaining the launcher

- Download a copy of the [launcher](https://github.com/Ochii/cso2-launcher/releases);
- Then extract the files inside to ```[your CSO2 game folder]/Bin```.

## Starting the game

*Note: **someone must be running a master server**.*

Run ```start-cso2.bat``` and enter the master server's IP address to start the game.

### Lauching

*The translations are courtesy of **[Frostie](#thank-yous)**.*

- When the game loads enter some login information - only your username will be used, so you can use any password ([image](https://puu.sh/Bw7hu/ffba6ca218.png));
- Select *커스텀모드* (*custom mode*) to go to the channel menu ([image](https://puu.sh/C1Nno/7122e0fefd.png));
- Pick a channel ([image](https://puu.sh/C1NpM/adead9596e.png))
- Create your own room by pressing *방만들기* (*create a room*) ([image](https://puu.sh/C1NrH/5cbaa524f8.png)) then choosing game mode ([image](https://puu.sh/C1NSv/8023bd39e8.png));
- You can now start the match by pressing *게임 시작* (*game start*) ([image](https://puu.sh/Bw7M3/9209eb7637.png)).

You can access your inventory by pressing *인벤토리* (*inventory*) in the bottom menu bar.

You **may open a CoD styled console by pressing the *~*** (*tilde*) **key**.

### Available command line arguments

- ```-lang [some language]``` - Sets the game's language to *some language*'s (considering you have the language files)
- ```-masterip [some ip]``` - Sets the master server's IP address to *some ip*
- ```-masterport [some port]``` - Sets the master server's port number to *some port*
- ```-decryptedfiles``` - Tell the game to consider every game file as decrypted
- ```-enablecustom``` - Enables the use of a custom directory `custom` in the root directory of your game's files

## Building

### Build requirements

- [CMake](https://cmake.org/download)
- [Visual Studio 2017](https://www.visualstudio.com/downloads)
- [Windows PowerShell](https://docs.microsoft.com/en-us/powershell/scripting/setup/installing-windows-powershell)

Tested with Visual Studio 2017. Support for other compilers is unknown.

### Starting the build

cso2-launcher uses CMake to generate project files and to build itself.

You can **build it through *CMake*** or **generate its project files**.

For example: to generate Visual Studio 2017 project files, **open a terminal instance** in the project's directory and run

```powershell
# in powershell or in standard Windows cmd
cmake -G "Visual Studio 15 2017" ./
```

Or use the ```cmake-gui``` tool bundled with CMake for a more user friendly experience.

Once successfully generated, you can open ```CSO2Launcher.sln``` and build the project in your preferred configuration.

If built successfully, you will find the launcher inside ```out/bin/Win32/[your configuration]```.

## Bug reporting

Have a look at the [issues](https://github.com/Ochii/cso2-launcher/issues) for a list of current bugs or to report them.

## Pull requests

Improvements and suggestions are welcomed, create a [pull request](https://github.com/Ochii/cso2-launcher/pulls).

## Libraries used

- **[cotire](https://github.com/sakra/cotire)** by [Sascha Kratky](https://github.com/sakra)
- **[Dear ImGui](https://github.com/ocornut/imgui)** by [Omar Cornut](https://github.com/sakra)
- **[PolyHook 2.0](https://github.com/stevemk14ebr/PolyHook_2_0)** by [stevemk14ebr](https://github.com/stevemk14ebr)

## Contributors

Thank you for your contributions.

- [GEEKiDoS](https://github.com/GEEKiDoS)
- [cs-student101](https://github.com/cs-student101)

## Credits

- [Valve Software](https://github.com/ValveSoftware/source-sdk-2013)

## Thank you's

- To **[UserU](https://www.youtube.com/user/GoodbyeSpy)**
- To **[SHI_KU](https://www.youtube.com/channel/UC2HZo-HFOuxmS6zWYPMD0hQ)**
- To **ReddUT** from [Counter-Strike Online Wikia](https://cso.wikia.com/)'s Discord
- To **Frostie** from [Counter-Strike Online Wikia](https://cso.wikia.com/)'s Discord

## License

Read ```LICENSE``` for license information.

I'm not affiliated with either Valve and/or Nexon, nor do I own Counter-Strike Online 2.
