#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "dv_asset.h"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H> //fl_alert
#include <FL/Fl_Box.H> //fl_alert
#include <FL/Fl_Browser.H> //fl_alert
#include <FL/Fl_Native_File_Chooser.H>

extern Fl_Double_Window* window_profile_setup;
extern Fl_Double_Window* window_projector_definition;
Fl_Double_Window *desenha_janela_page_1();
Fl_Double_Window *desenha_janela_page_2();
void callback_projetor_selecionado(Fl_Widget*, void*);
Fl_Double_Window *desenha_janela_page_1();
Fl_Double_Window *desenha_janela_page_2();
void ChooseEnvFile(Fl_Widget*, void*);
void ChooseScriptFile(Fl_Widget*, void*);
