#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <FL/Fl.H>
// #include <FL/x.H> // for fl_open_callback
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/filename.H>
#include <FL/fl_ask.H>

Fl_Window *CodeEditor(int argc, char **argv);

#endif
