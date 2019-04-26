/*	This file is part of the FpMED (Anydome) Project Copyright (c) 2014-2017 Aramis Hornung Moraes
	See FpmedCore.h for license and conditions for using this code
*/

#define defaultComPort 9991							// the default communication socket port to make the
													// hand-shake communication in the system.

#define defaultSceneRepPort 9876					// the default communication socket port used by Urho
													// to replicate the 3D scene elements.

#define defaultCalibrationServerPort 7766			// default port for server to listen for
													// calibration requests

#define defaultScanableServerPort 6688			    // default port for server to listen for
													// server detection/scan

#define defaultScriptBackDoorServerPort 9111		// used to send packets to a fuction in scripts
													// (the function is called when a packet is
													// sended to this port)

#define ipSizeChar 16								// defines the size of a char array to store a ip
													// information
#define MaxFilePathNameSize 2048					// auto explanatory, the maximum lenth to storea
													// file path or directory path

#define serverPropertiesFileExtension ".ssr"		// TODO
#define projectorPropertiesFileExtension ".spr"		// TODO

#define defaultIniFileName "conf.ini"				// This is the name and extension of the file the
													// software lookup for when starting

#define defaultServerFileName "servers.ssr"			// This is the name and extension of the file the
													// software lookup for default configuration for
													// the servers

#define MediumDataLen 512							// Medium informations length. We use this to
													// define the default data lenth for general
													// purpose info, like the properties in the
													// files that describe the environment

#define PORT_ASK_FOR_PROJECTORS_LOCAL_NETWORK 7491  // universao port to discover projector
                                                    // and send global commands for them

#define CHECK_SERVER_ALIVE_TIMEOUT_MILISEC 50      // how long in milisec wait for the response of server

