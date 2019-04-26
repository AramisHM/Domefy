//
// FpMED - Framework for Distributed Multiprojection Systems
// Copyright Aramis Hornung Moraes 2014-2019
//

// Built-in script editor

#ifndef SCRIPTEDITOR_H
#define SCRIPTEDITOR_H

#include "FPMED.H"

class ScriptEditor 
{
private:
Fl_Window *editorWindow; // FLTK window for editing scripts

public:
void editScriptLoop();
};
#endif