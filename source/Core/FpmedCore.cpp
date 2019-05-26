
#include "FpmedCore.h"
#include <Core/Environment.h>
#include <stdlib.h>
#include <string.h>
#include "FpmedCore.h"
#include "irrXML.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <Forms/fpmed_forms.h>
#include <Urho3D/Math/Random.h>
#include <time.h>

using namespace irr;
using namespace io;

// should we editi the script?
int flag_edit_script = 1;

// should we run the script?
int flag_run_script = 0;

char softwareDirectory[MaxFilePathNameSize] = {0};
char executableFileExtensionDirectory[MaxFilePathNameSize] = {0};
char serverPropertiesFileDirectory[MaxFilePathNameSize] = {0};
char projectorPropertiesFileDirectory[MaxFilePathNameSize] = {0};
char applicationScriptFileDirectory[MaxFilePathNameSize] = {0};

int isProjectorFileReachable = -1;
int isApplicationScriptFileReachable = -1;

fpmed::Environment globalEnv;
int selected_proj = -1;
int selected_serv = -1;

int debug_enabled = 0;          // show console?
int dome_grid = 0;              // show grid in dome?
int find_any_local_server = 0;  // Should projectors ask for and connect to any
                                // server running in local network?

int run_scripts = 1;  // should server run scripts?

int calibrationServerPort =
    defaultCalibrationServerPort;  // port for server to calibrate projector
                                   // view

void loadIniFile(char *iniFile) {
    IrrXMLReader *xml = createIrrXMLReader(iniFile);

    printf("Trying to read: %s\n", iniFile);

    bool endReached = false;

    while (endReached == false && xml && xml->read()) {
        switch (xml->getNodeType()) {
            case EXN_ELEMENT: {
                if (!strcmp("FindAnyLocalServer", xml->getNodeName())) {
                    find_any_local_server =
                        atoi(xml->getAttributeValue("value"));
                }
                if (!strcmp("RunScripts", xml->getNodeName())) {
                    run_scripts = atoi(xml->getAttributeValue("value"));
                }

                if (!strcmp("DefaultProjectorFile", xml->getNodeName())) {
                    if (!strlen(projectorPropertiesFileDirectory)) {
                        char *projFileData = 0;
                        projFileData = (char *)xml->getAttributeValue("value");

                        int isProjectorFile = 0;
                        isProjectorFile = strlen(projFileData);
                        if (isProjectorFile) {
                            FILE *fp;
                            fp = fopen(projFileData, "rb");
                            if (fp) {
                                isProjectorFileReachable = 1;
                                fclose(fp);
                            } else
                                isProjectorFileReachable = 0;
                        }

                        if (isProjectorFileReachable > 0) {
                            int indexVal = -1;

                            if (projFileData) {
                                strncpy(projectorPropertiesFileDirectory,
                                        projFileData, strlen(projFileData));
                                char *fileData2 = 0;
                                fileData2 =
                                    (char *)xml->getAttributeValue("index");
                                indexVal = atoi(fileData2);
                                if (fileData2) {
                                    selected_proj = indexVal;
                                }
                            }
                        }
                    }
                }
                if (!strcmp("DebugEnabled", xml->getNodeName())) {
                    debug_enabled = atoi(xml->getAttributeValue("value"));
                }
                if (!strcmp("FulldomeGrid", xml->getNodeName())) {
                    dome_grid = atoi(xml->getAttributeValue("value"));
                }

                if (!strcmp("ApplicationScript", xml->getNodeName())) {
                    char *scriptFileName = 0;
                    scriptFileName = (char *)xml->getAttributeValue("value");

                    // checar se conseguimos ler o arquivo informado
                    int isScriptFile = 0;
                    isScriptFile = strlen(scriptFileName);
                    if (isScriptFile) {
                        FILE *fp;
                        fp = fopen(scriptFileName, "rb");
                        if (fp) {
                            isApplicationScriptFileReachable = 1;
                            fclose(fp);
                        } else
                            isApplicationScriptFileReachable = 0;
                    }

                    if (isApplicationScriptFileReachable > 0) {
                        int indexVal = -1;
                        if (isScriptFile &&
                            applicationScriptFileDirectory[0] ==
                                0)  // if scripts are allowd and if we have not
                                    // loaded a script until now, then load this
                                    // one in the .ini file
                        {
                            strncpy(applicationScriptFileDirectory,
                                    scriptFileName, strlen(scriptFileName));
                        }
                    }
                }
            } break;
            case EXN_ELEMENT_END:
                if (!strcmp("FpMED", xml->getNodeName())) {
                    printf("\nLoaded conf.ini file.\n");
                    endReached = true;
                    break;
                }
                break;
        }
    }

    if (strlen(serverPropertiesFileDirectory))
        strncpy(serverPropertiesFileDirectory, defaultServerFileName,
                strlen(defaultServerFileName));
    delete xml;
}

void defineSoftwareDirecotry(int argc, char *argv[]) {
    printf("%s\n", argv[0]);
    char *lastOccur = 0;
    int originalSize = 0;
    int ignoreSize = 0;

#ifdef _WIN32
    lastOccur = strrchr(
        argv[0], '\\');  // THIS WILL DESTROY YOUR DAY - UNDER ANY *UNIX SYSTEM
#else
    lastOccur = strrchr(argv[0], '/');
#endif
    originalSize = strlen(argv[0]);
    ignoreSize = strlen(lastOccur);
    strncpy(softwareDirectory, argv[0], (originalSize - ignoreSize));
    softwareDirectory[originalSize - ignoreSize] = 0;
    printf("EXE DIR: %s\n", softwareDirectory);
#ifdef _WIN32
    _chdir(softwareDirectory);
#else
    chdir(softwareDirectory);
#endif
    if (argc > 1)  // get the parameter file data and process it
    {
        printf("Warning, %d parameter(s) found", argc);
// also get the executable file extension directory
#ifdef _WIN32
        lastOccur = strrchr(argv[1], '\\');
        if (!lastOccur) lastOccur = argv[1];
#else
        lastOccur = strrchr(argv[1], '/');
        if (!lastOccur) lastOccur = argv[1];
#endif
        originalSize = strlen(argv[1]), ignoreSize = strlen(lastOccur);
        strncpy(executableFileExtensionDirectory, argv[1],
                (originalSize - ignoreSize));
        executableFileExtensionDirectory[originalSize - ignoreSize] = 0;
        printf("%s DIR: %s\n", strrchr(argv[1], '.'),
               executableFileExtensionDirectory);

        // determin what this file will do in the system
        char *fileExtension = strrchr(argv[1], '.');

        if (!strcmp(
                fileExtension,
                serverPropertiesFileExtension))  // its a server properties file
        {
            // especial extension executable file is a server properties file
            strncpy(serverPropertiesFileDirectory, argv[1], strlen(argv[1]));
            selected_serv = 1;
        } else if (!strcmp(fileExtension, projectorPropertiesFileExtension)) {
            // especial extension executable file is a projector properties file
            strncpy(projectorPropertiesFileDirectory, argv[1], strlen(argv[1]));
            selected_serv = 0;
        } else if (!strcmp(fileExtension,
                           ".as"))  // angelscript, probably the game script
        {
            strncpy(applicationScriptFileDirectory, argv[1], strlen(argv[1]));
            selected_serv = 1;  // if we wan to run a script, we should be a
                                // server, tight? ;)
        }
    }
}

void fpmedInit(int argc, char *argv[]) {
    defineSoftwareDirecotry(argc, argv);
    loadIniFile();

    if (debug_enabled) {  // show black console?
#ifdef _WIN32
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
#else
#endif
        printf("DEBUG CONSOLE ENABLED\n\nPARAMETERS ARE:\n");
        int ai;
        for (ai = 0; ai < argc; ++ai) printf("argument %d: %s\n", ai, argv[ai]);
        printf(" ");
        printf("\n\n");
    }

    Urho3D::SetRandomSeed(time(NULL));  // TODO : check randomness

    // FLTK Windows
    flag_edit_script = 0;

    if (argc <= 1)  // show profile window choice
        window_profile_setup = desenha_janela_page_1();
    else if (selected_serv == 0 && selected_proj == -1)  // its projector
        selecionaProjectorIndexForm();  // define qual configuração de projetor
                                        // usar

    // Loop FLTK until user defines the primary application behavior/profile
    // (server or projector).
    while (Fl::check() &&         // atualiza a UI
           (selected_serv < 0 ||  // While it's not a server
            (selected_serv == 0 &&
             selected_proj == -1)))  // While is not exclusively a projector
    {
    }
}

// Tells if isntance is projector
int isProjector() {
    if (selected_serv == 0) {
        return 1;
    }
    return 0;
}

// Tells if isntance is server
int isServer() {
    if (selected_serv > 0) {
        return 1;
    }
    return 0;
}