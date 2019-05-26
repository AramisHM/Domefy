//
// FpMED - Framework for Distributed Multiprojection Systems
// Copyright Aramis Hornung Moraes 2014-2019
//

#ifndef FPMED_H
#define FPMED_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <Core/CoreParameters.h>
#include <Core/Environment.h>
#include <Core/FpmedCore.h>
#include <Core/ProjectorMachine.h>
#include <Core/ServerMachine.h>
#include <Forms/GUIWindows.h>
#include <Forms/fpmed_forms.h>
#include <stdio.h>
#include <string.h>

//#include "Splash.h"

//#include "Urho3DAll.h"
#include <Core/AutoDetectServer.h>

// major, year+month, version, minor
#define fpmed_version "0.195.0.1"

// UI includes
#include <FL/Fl.H>
#include <FL/Fl_Box.H>      //fl_alert
#include <FL/Fl_Browser.H>  //fl_alert
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <FL/fl_ask.H>  //fl_alert

#endif
