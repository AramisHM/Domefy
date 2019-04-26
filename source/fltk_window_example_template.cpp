#include "GUIWindows.h"
#include "FpmedCore.h"
#include "FPMED.H"

// TODO: locar esses externs na classe depois com metodos get e se

Fl_Double_Window* window_profile_setup = 0;
Fl_Double_Window* window_projector_definition = 0;

Fl_Double_Window *desenha_janela_page_1();
Fl_Double_Window *desenha_janela_page_2();


// separar depois?

#include "Environment.h"

extern fpmed::Environment globalEnv;

char envFileName[1024];

// GLOBAL VARIABLES
Fl_Browser* browser_projetores;

Fl_Double_Window* janela_Selecionar_projetor = 0;

//-------------------------- Funcoes da janela  ------------------------------------------------
int index_linha_selecionada_tabela_projetores() // return -1 se nao achou, senao retorna um numero inteiro positivo
{
	char *token;
	char linha_selecionada[512];
	char *linha_browser = (char*)browser_projetores->text(browser_projetores->value());

	if(linha_browser)
	{
		strcpy(linha_selecionada, linha_browser);
		token = strtok(linha_selecionada," \t-");
		return atoi(token);
	}
	else return -1;
}
// ---------------------- CALLBACKS DOS BOTOES

void selecionaProjectorIndexForm()
{
	globalEnv.loadEnvironmentFromFile(projectorPropertiesFileDirectory);
	window_projector_definition = desenha_janela_page_2();
}

void callback_projetor_selecionado(Fl_Widget*, void*)
{
	//window_projector_definition = 0;
	window_profile_setup->hide();
	//fl_alert("desenha a janela de perfil");

	//definir um arquivo de projecao antes
	extern int selected_serv;
	extern int selected_proj;

	selected_serv = 0;

	//dialogo que informa quando tinhamos um arquivo predefinido mas nao consguimos abrir ele
	if(isProjectorFileReachable == 0) // arquivo projecao definido mas nao foi possivel abrir
	{
		fl_alert("Nao foi possivel abrir o arquivo .spr informado, favor selcionar um arquivo de propriedades de projetores");
	}
	if(isProjectorFileReachable <= 0) // arquivo indefinido ou n�o foi possivel abrir
	{
		ChooseEnvFile(0,0); // chose the projector properties file
		selecionaProjectorIndexForm();
	}
	if(selected_proj == -1) // arquivo definido mas indice do projetor indefinido
	{
	    selecionaProjectorIndexForm();
	}
	else // arquivo de proejcao definido e foi possivel abrir
		globalEnv.loadEnvironmentFromFile(projectorPropertiesFileDirectory);
	//Sleep(100);
	//window_profile_setup->show();
}



void callback_servidor_selecionado(Fl_Widget*, void*)
{
	extern int selected_serv;
	selected_serv = 1;
	window_profile_setup->hide();
	delete window_profile_setup;
	window_profile_setup = 0;


	#ifdef fpmed_allow_scripted_application
	if(run_scripts == 1) // we only attempt to load scripts if enabled.
	{
		//dialogo que informa quando tinhamos um arquivo predefinido mas nao consguimos abrir ele
		if(isApplicationScriptFileReachable == 0) // arquivo projecao definido mas nao foi possivel abrir
		{
			fl_alert("N�o foi poss�vel encontrar o Script da aplica��o informado, favor seelcionar um script para rodar a aplica��o.");
		}
		if(isApplicationScriptFileReachable <= 0) // arquivo indefinido ou n�o foi possivel abrir
		{
			ChooseScriptFile(0,0); // chose the projector properties file
			//selecionaProjectorIndexForm(); // this seems to be a bug... Why would you choose a projector conf if we just choosen a script (a server-only type file.)?
		}
	}
	#endif
}

void ChooseScriptFile(Fl_Widget*, void*)
{
	// Create native chooser
	Fl_Native_File_Chooser native;
	native.title("Select Application Script");
	native.type(Fl_Native_File_Chooser::BROWSE_FILE);
	native.filter("AngelScript\t*.{as}\n"
				  "Text\t*.txt\n"
				  "AngelScript bytecode\t*.asc\n");
	native.directory(softwareDirectory);
	//native.preset_file(G_filename->value());
	// Show native chooser
	switch (native.show())
	{
	case -1: fprintf(stderr, "ERROR: %s\n", native.errmsg()); break;	// ERROR
	case  1: fprintf(stderr, "*** CANCEL\n"); fl_beep(FL_BEEP_DEFAULT); break;		// CANCEL

	default: 								// PICKED FILE
		if (native.filename())
		{
			strncpy(applicationScriptFileDirectory,(char*)native.filename(),MaxFilePathNameSize);
		}
		break;
	}
}

void ChooseEnvFile(Fl_Widget*, void*)
{
	// Create native chooser
	Fl_Native_File_Chooser native;
	native.title("Select the Projector Properties File");
	native.type(Fl_Native_File_Chooser::BROWSE_FILE);
	native.filter("System Projectors File\t*.{spr}\n"
				  "Text\t*.txt\n");
	native.directory(softwareDirectory);
	//native.preset_file(G_filename->value());
	// Show native chooser
	switch (native.show())
	{
	case -1: fprintf(stderr, "ERROR: %s\n", native.errmsg()); break;	// ERROR
	case  1: fprintf(stderr, "*** CANCEL\n"); fl_beep(FL_BEEP_DEFAULT); break;		// CANCEL

	default: 								// PICKED FILE
		if (native.filename())
		{
			strncpy(projectorPropertiesFileDirectory,(char*)native.filename(),MaxFilePathNameSize);

		}
		break;
	}
}

void callback_return_main_window(Fl_Widget*wdgt, void*)
{
	if (janela_Selecionar_projetor)
	{
		janela_Selecionar_projetor->hide();
		delete janela_Selecionar_projetor;
		janela_Selecionar_projetor = 0;
	}
	window_profile_setup->show();
}

void CB_PROJ_CONF_SELECT(Fl_Widget*, void*)
{
	int index_projetor = index_linha_selecionada_tabela_projetores();

	if(index_projetor>=0)
	{
		extern int selected_proj;
		printf("%s\n\n", globalEnv.getProjector(index_projetor).getName());

		// copy the pointers to the global variable

		selected_proj = index_projetor;

		window_projector_definition->hide();
		delete window_projector_definition;
		window_projector_definition=0;

	}
	else
	{
		printf("Erro ao tentar acessar tabela de projetores.\n");
	}
}
// ---------------------- CALLBACKS DOS BOTOES


// ---- FUNCOES AUXILIARES

//Janela escolha, projetor ou servidor
Fl_Double_Window *desenha_janela_page_1()
{
	Fl_Double_Window* local_window;
	local_window = new Fl_Double_Window(454, 320);
	local_window->color(FL_LIGHT2);
	local_window->position((Fl::w()/2)-(local_window->decorated_w()/2),
						   (Fl::h()/2)-(local_window->decorated_h()/2));
	local_window->label("Setup profile");


	///// ------------------------------------------ BODY
	{ Fl_Box* o = new Fl_Box(5, 65, 440, 205);
      o->box(FL_ENGRAVED_BOX);
      o->color(FL_BACKGROUND2_COLOR);
      o->align(Fl_Align(FL_ALIGN_WRAP));
    } // Fl_Box* o
    { Fl_Box* o = new Fl_Box(15, 75, 420, 90, "ANYDOME 0.40 PROTOTYPE\nTHIS SOFTWARE IS PROTECTED WITH A NDA TERM. USE IS PERMITTED ONLY FOR TESTING.");
      o->box(FL_EMBOSSED_BOX);
      o->color((Fl_Color)215);
      o->labelcolor((Fl_Color)1);
      o->align(Fl_Align(FL_ALIGN_WRAP));
    } // Fl_Box* o
	{ Fl_Box* o = new Fl_Box(15, 274, 80, 21, fpmed_version);
      o->labeltype(FL_EMBOSSED_LABEL);
      o->labelsize(8);
    } // Fl_Box* o
    { Fl_Button* o = new Fl_Button(230, 195, 130, 40, "Projector");
      o->tooltip("Define this software instance as a projector");
      o->color(FL_LIGHT1);
      o->image(image_projector);
	  o->callback(callback_projetor_selecionado);
      o->align(Fl_Align(288));
    } // Fl_Button* o
    { Fl_Button* o = new Fl_Button(90, 195, 130, 40, "Server");
      o->tooltip("Define this software instance as a server");
      o->color(FL_LIGHT1);
      o->image(image_server);
      o->align(Fl_Align(288));
	  o->callback(callback_servidor_selecionado);
    } // Fl_Button* o
    { Fl_Box* o = new Fl_Box(-5, 0, 460, 55);
      o->image(image_psart);
    } // Fl_Box* o
	///// ------------------------------------------ END OF BODY

    local_window->end();
    local_window->show();
	return local_window;
}


// Janela configuracao projetor
Fl_Double_Window *desenha_janela_page_2()
{
	Fl_Double_Window* local_window;
	local_window = new Fl_Double_Window(454, 320);
	local_window->color(FL_LIGHT2);
	local_window->position((Fl::w()/2)-(local_window->decorated_w()/2),
						   (Fl::h()/2)-(local_window->decorated_h()/2));
	local_window->label("Setup profile");
	janela_Selecionar_projetor = local_window;

	///// ------------------------------------------ BODY
	{ Fl_Box* o = new Fl_Box(10, 65, 430, 205);
      o->box(FL_ENGRAVED_BOX);
      o->color((Fl_Color)55);
      o->align(Fl_Align(FL_ALIGN_WRAP));
    } // Fl_Box* o
    { Fl_Box* o = new Fl_Box(15, 70, 420, 70, "Please fill the following atributes to configure this projector.   ATENTION: \
The device Id must be a unique integer number. The number must be pre-defined \
by the you while you are setting up the infrastructure of the multi projection\
 system.");
      o->box(FL_EMBOSSED_BOX);
      o->color(FL_WHITE);
      o->labelcolor((Fl_Color)24);
      o->align(Fl_Align(FL_ALIGN_WRAP));
    } // Fl_Box* o

    { Fl_Box* o = new Fl_Box(-5, 0, 460, 55);
      o->image(image_psart);
    } // Fl_Box* o

    { Fl_Box* o = new Fl_Box(15, 284, 80, 21, "V1.0.0");
      o->labeltype(FL_EMBOSSED_LABEL);
      o->labelsize(8);
    } // Fl_Box* o

    { Fl_Button* o = new Fl_Button(335, 280, 105, 25, "Ok");
      o->tooltip("Define this software instance as a projector");
      o->color((Fl_Color)26);
	  o->callback(CB_PROJ_CONF_SELECT);
    } // Fl_Button* o

    { Fl_Button* o = new Fl_Button(225, 280, 105, 25, "Back");
      o->tooltip("Return to main menu.");
      o->color((Fl_Color)17);
	  o->callback(callback_return_main_window);
    } // Fl_Button* o

    {
		browser_projetores = new Fl_Browser(100, 160, 315, 80, "Environment projectors");
      browser_projetores->tooltip("Choose one of the following projetors");
      browser_projetores->color((Fl_Color)215);
	  browser_projetores->align(Fl_Align(37));
	  browser_projetores->type(FL_HOLD_BROWSER);

	  for(int i = 0; i < globalEnv.getNumberOfProjectors(); i++)
	  {
		  char temp_text[50];
		  sprintf(temp_text, "%d. %s", i, globalEnv.getProjector(i).getName());
		  browser_projetores->add(temp_text);
	  }
    }
	// Fl_Input* o
	/*
    { Fl_Input* o = new Fl_Input(100, 185, 160, 20, "Projector ID");
      o->tooltip("Defines the identification number of this software instance");
      o->color((Fl_Color)213);
    } // Fl_Input* o
    { Fl_Input* o = new Fl_Input(305, 185, 110, 20, "Index");
      o->tooltip("Defines the identification number of this software instance");
      o->color((Fl_Color)213);
    } // Fl_Input* o
	*/

	/*
    { Fl_Input* o = new Fl_Input(100, 210, 315, 20, "Server IP");
      o->tooltip("Defines the identification number of this software instance");
      o->color((Fl_Color)213);
	  o->value("127.0.0.1");
    } // Fl_Input* o*/
	///// ------------------------------------------ END OF BODY

    local_window->end();
    local_window->show();
	return local_window;
}
