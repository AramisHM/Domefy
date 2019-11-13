![N|Solid](https://bitbucket.org/Aramis/domefy/raw/d301b980947a867a387198b9fae519b5d6154c12/bin/Data/fpmed/domefy_logo256.png)

# Domefy
Domefy is an application framework used for development of multi-projector
systems based software. It is written in C++ and uses Urho3D as core engine.

## Build/Installation
The following steps shows how to fully compile the Domefy for Windows and Linux

#### Building on GNU/Linux
Before we begin, make sure to have the following packages installed on your system with the following:
```sh
sudo apt-get install cmake
sudo apt-get install build-essential
sudo apt-get install libx11-dev libxrandr-dev libasound2-dev git cmake make libglu1-mesa-dev freeglut3-dev libtheora-dev libogg-dev libvorbis-dev libogg-dev
```
![N|Solid](https://xubuntu.org/wp-content/themes/xubuntu-theme/xubuntu-eighteen/images/xubuntu-logo-45-dark.png)

If you are using Xubuntu 18.04 addition dependencies might be needed.
```sh
sudo apt-get install libxinerama-dev libxft-dev libfontconfig-dev libxcursor-dev
```
If you are going to build CEF with it, remember to have these packages installed aswell:
```sh
sudo apt-get install libgtk2.0-dev libgtkglext1-dev
```

Now, we can build Urho3D from source code. Go to Urho3D's main directory and do the following:
```sh
sudo ./cmake_generic.sh ./
sudo make
```

# Windows 10

These are required to be installed in your system:

1. [MinGW from Code::Blocks 17.12](https://sourceforge.net/projects/codeblocks/files/Binaries/17.12/Windows/)
2. [MSYS 1.0.11](https://sourceforge.net/projects/mingw/files/MSYS/Base/msys-core/msys-1.0.11/)

We suggest you the following tools:

1. [Visual Studio Code](https://code.visualstudio.com/)


Install Code::Blocks. In its installation folder, extract `MinGW`, and place it on `C:/`
`You might wnat to rename gdb32.exe to gdb.exe`
Add the environment variables to Windows
```sh
C:/MinGW/bin to PATH
C:\MinGW to MINGW_SYSROOT 
```
  
Go to Urho3D root directory and do with PowerShell terminal

```sh
.\cmake_mingw.bat ./
```

wait the configuration finish, and then:

```sh
mingw32-make.exe
```
--------------------------
From time to time you may have to recompile Urho3D. When this happen, have in mind that:
- Urho3D is supposed to be installed in the system libs, to create a project people will
offen tell you to read documentation, witch basically makes you use another of theirs
premade cmake. If you decide you want to make a classic .h .lib style of deployment
on your own, you should, compile the engine, get the libraries in the /lib. The include
files are located at /source!! The include folder is just a shortcut to the /source
folder (at least in GNU/linux).

- Another thing is that there might be some MACROS that will give you prblem, such as cursor-shape-thing.
In that case, just comment the stuff out, you can live without it.


--------------------------

## Possible Errors
`src/audio/winmm/SDL_winmm.c:57:33: error: unknown type name 'WAVEOUTCAPS2W'`
replace SDL_winmm from Urho3D source with https://hg.libsdl.org/SDL/file/7e06b0e4dbe0/src/audio/winmm/SDL_winmm.c

`error: 'MEMORYSTATUSEX' was not declared in this scope`
go to `Urho3D-1.7\Source\Urho3D\Core` and add add the following in the beginning of `ProcessUtil.h`
*#define _WIN32_WINNT 0x0500*


`MSYS 1.0.11 didnt start post install`
do run it manually then :-).

` VS Code argues something about MIDebuggerPath`
open `launcher.json` and add `"IDebuggerPath": "C:/MinGW/bin/gdb.exe"`

`Program received signal ?, Unknown signal.`
This seems to be a BUG with gdb for Windows. If program stops during debug, just ask it to continue (press resume buttom). It should be fixed if we use some other version of GCC for windows. 

`When debbuging, some error about "illegal instruction" pops up.`
This only fixes up when recompiling all libraries. Personally, i recompiled FLTK and Urho3D, rebuilt the project, and it debbuged just fine.


`Application compiles, runs, open, but Urho3D doesn't render.`
i) Check if you are debugging with GCC for mingw32 5.1.0

------------------------

Afther that, make sure you have all the packages installed and happy coding. :-)

Proudly made in Brazil [![Brazil Flag](https://www.escavador.com/assets/escavador/user/img/flag/24/Brazil.png)]()

Development Leader: \
Aramis Hornung Moraes [![N|Solid](https://plus.google.com/_/focus/photos/public/AIbEiAIAAABDCJic9sjGqsKHaiILdmNhcmRfcGhvdG8qKDQ5ZTQzZTQ3MTc4MGM0NGZjMTgwMzU0MzhiMzM2ZTc0NWVlNjY4Y2EwATerYkh1psreGZrSbWKM4OxVztVc?sz=128)](https://www.aramishm.com/about-aramis)

