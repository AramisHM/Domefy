# Domefy

**What is it:** Domefy is an application framework designed for multi-projector systems, particularly enhancing the immersive environments of fulldome theaters. It is written in C++ and uses Urho3D as core engine

![alt tag](https://github.com/AramisHM/Domefy/blob/master/snapshots/1.jpg)

**Why:** A fulldome is a unique, dome-shaped projection space that surrounds viewers, offering a captivating and all-encompassing visual experience. Domefy serves as a bridge between these fulldome theaters and the vast world of online immersive content, providing tools for developing interactive experiences controlled through mobile devices like smartphones and tablets. In essence, Domefy brings a blend of technology, interactivity, and online content to elevate the immersive experience in fulldome environments.

Architecture:
![alt tag](https://github.com/AramisHM/Domefy/blob/master/snapshots/2.png)

A slideshow demonstration using the Domefy framework:
![alt tag](https://github.com/AramisHM/Domefy/blob/master/snapshots/5.jpeg)

Example 39 "Crowd Navigation" (in agenscript) of Urho3D ported and runnning in a real Fulldome Theater:
![alt tag](https://github.com/AramisHM/Domefy/blob/master/snapshots/4.jpg)

A port of angelscript example 11 "Physics" of Urho and a prototype panel that controls the backend of the framework:
![alt tag](https://github.com/AramisHM/Domefy/blob/master/snapshots/6.png)

A port of angelscript example 46 "Raycast Vehicle" of Urho and a prototype of the controls that user may open in any smartphone with a browser:
![alt tag](https://github.com/AramisHM/Domefy/blob/master/snapshots/7.png)

Prototype of regular Smartphone as controller for application:
![alt tag](https://github.com/AramisHM/Domefy/blob/master/snapshots/3.jpeg)

## Video demonstrations
Video demonstration of controlling application with smartphone:


[![Controls](https://github.com/AramisHM/Domefy/blob/master/snapshots/tn1.jpeg)](https://youtu.be/ge5aO6qlKP4)

Video demonstration of calibration:


[![Controls](https://github.com/AramisHM/Domefy/blob/master/snapshots/tn3.jpeg)](https://youtu.be//KAlLalKcC7o)

Video demonstration of VHP in fulldome **(sensitive content!!, contain medical images)**:


[![Controls](https://github.com/AramisHM/Domefy/blob/master/snapshots/tn2.jpeg)](https://youtu.be/JmlqDRLmoUM)

Video demonstration of VHP planecut **(sensitive content!!, contain medical images)**:


[![Controls](https://github.com/AramisHM/Domefy/blob/master/snapshots/tn4.jpeg)](https://youtu.be/MgEEJWsn2o8)

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
Now, we can build Urho3D from source code. Go to Urho3D's main directory and do the following:
```sh
sudo ./cmake_generic.sh ./
sudo make
```

# Windows 10
// TODO: FIXME: bote um break point aqui se vc usar o MinGW GCC

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

