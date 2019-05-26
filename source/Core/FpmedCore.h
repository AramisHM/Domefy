#ifndef FPMEDCORE_H
#define FPMEDCORE_H

#include <Core/CoreParameters.h>
#include <Core/Environment.h>

//#include <Urho3D/Network/NetworkEvents.h>


#ifdef _WIN32
#include <sys/stat.h>// _chdir, defineSoftwareDirecotry
#include <direct.h> // _chdir, defineSoftwareDirecotry
#else
#include <unistd.h>
#endif
#define fpmed_version_major 0
#define fpmed_version_minor 4
#define fpmed_version_patch 0

//#define fpmed_allow_scripted_application // comment this out to remove script support
#define fpmed_allow_cpp_application // comment this out to remove the custom cpp code

extern int flag_run_script; // should we run the script?
extern int flag_edit_script; // should we edit the script?

extern char softwareDirectory[MaxFilePathNameSize];
extern char executableFileExtensionDirectory[MaxFilePathNameSize];
extern char serverPropertiesFileDirectory[MaxFilePathNameSize];
extern char projectorPropertiesFileDirectory[MaxFilePathNameSize];
extern char applicationScriptFileDirectory[MaxFilePathNameSize];

extern fpmed::Environment globalEnv;
extern int selected_proj;
extern int selected_serv;
extern int isProjectorFileReachable;
extern int isApplicationScriptFileReachable;

void loadIniFile(char *iniFile = defaultIniFileName);

extern int debug_enabled; // indicates if we should display the outputconsole.
extern int dome_grid; // tells us if we display a grid projection on fulldome mode
extern int find_any_local_server; // Should projectors ask for and connect to any server running in local network?
extern int run_scripts; // tells us if we can load scripts as a server
extern int calibrationServerPort;  // port used to allow communication to calirbate the projector view

// fpmedInit - Needed initializations
void fpmedInit(int argc, char**argv);
void defineSoftwareDirecotry(int argc, char**argv);

// Tells if isntance is projector
int isProjector();

// Tells if isntance is server
int isServer();
#endif
