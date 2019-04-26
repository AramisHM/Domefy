// calibrator 1.0.1
// PROTOTYPE VERSION!!!
// Used to calibrate the fulldome projection for the Anydome
// Copyright (C) Aramis Hornung Moraes 2016

#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H> //fl_alert
#include <FL/Fl_Box.H> //fl_alert
#include <FL/Fl_Browser.H> //fl_alert

#ifdef WIN32
#include <windows.h>
#endif // WIN32

#include <string.h>
#include <tnet/TNet.h>

#include "dv_asset.h"

#include <vector>
#include <string>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#define PORT_ASK_FOR_PROJECTORS_LOCAL_NETWORK 7491

// default values for editing
char  posDefVal[32] = {'0','.','0','1', 0},
	  rotDefVal[32] = {'0','.','6', 0},
	  fovDefVal[32] = {'0','.','1', 0};

TNETClient *calibratorClinet;
Fl_Input* ipInput;
Fl_Input* portInput;
Fl_Input *inputValToSend;

char ip[16];
int port = 0;

typedef struct localProjectors
{
	char name[128];
	char ip[30];
	int port;
}localProjectors;

Fl_Double_Window* Pg1 = 0; // conect
Fl_Double_Window* Pg2 = 0; // calibrate
Fl_Double_Window* Pg3 = 0; // select server

Fl_Double_Window *desenha_janela_page_1();
Fl_Double_Window *desenha_janela_page_2();

#define qntBts 12 // quantidade de botoes na janela com o tecladino {a,s,d,w,q,e,p,r,f,c}
Fl_Button *butts[qntBts];  // botoes de controle

// these arrays are used to analyse comands mapped in the keyboard
#define numberOfKeys 256
int key_state[numberOfKeys];
int key_past_state[numberOfKeys];

int mode = 1; // 1 position. 2 rotation 3. fov
int continuous = 0; // 1 it floods with comamnds, 0 sends only one time per key pressed

// USE ASCII TO LOOF FOR KEYS
void flushKeyStates(){

	for(int i = 0; i < numberOfKeys; i++)
		key_state[i] = key_past_state[i] = 0;
}
void updateKePastStates(){

	for(int i = 0; i < numberOfKeys; i++)
		key_past_state[i] = key_state[i];
}
void updateKeyStates(){

	for(int i = 0; i < numberOfKeys; i++){

		key_state[i] = 0;
		if(Fl::get_key(i))
			key_state[i] = 1;
	}
}
int keyPressed(int keyCode){
	if(key_state[keyCode] == 1 && key_past_state[keyCode] == 0)
		return 1;
	else
		return 0;
}
int keyReleased(int keyCode){
	if(key_state[keyCode] == 0 && key_past_state[keyCode] == 1)
		return 1;
	else
		return 0;
}
int iskeyPressed(int keyCode)
{
	return key_state[keyCode];
}


Fl_Browser* browser_projetores; // has server list


void flush_browser_projetores()
{
	for(int p = 1; p <= (browser_projetores->size()); ++p)
	{
		localProjectors *selectedProj;
		selectedProj = (localProjectors*)browser_projetores->data(p);
		free(selectedProj);
	}
}


// try to find any projector instance running in the local network
// WARNING ! The enet engine must be initialized before calling this function
int findProjectorsInLocalNetwork()
{
	TNETClient *fndProjClient = new TNETClient();
	TNETServer *fndProjServer = 0;

	int n_tries = 7,								// numero de tentativas pra tentar achar um projetor na rede local
		n_tries_create_server = 10;					// numero de tentativas pra tentar subir um servidor para escutar respostas

	int tryPort = -1;								// porta utilizada para tentar escutar resposta

	while(n_tries_create_server > 0)
	{
		fndProjServer = NULL;
		tryPort = rand() % 4000 + 5222;				// ports 5222 to 9222
		fndProjServer = new TNETServer(tryPort);

		if(fndProjServer)
			break;
	}

	if(!fndProjServer)
		exit(-6234);// random code for user to cntrl+f to find it here. Failed to create a server to try to detect projectors in local network

	bool foundSomething = false;
	browser_projetores->clear();

	flush_browser_projetores();

	while(n_tries > 0)
	{
		char *broadcastIp = "255.255.255.255";
 		char message[9] = "APH?";
		sprintf(message, "APH?%d\0", tryPort); // message should be like: APH?PORT_TO_ANSWER
											   // the answer should be something like "MACHINE_NAME;REACH_PORT"

		fndProjClient->send(message, PORT_ASK_FOR_PROJECTORS_LOCAL_NETWORK,
			broadcastIp,
			strlen(message));



		// try listen for incomming messages
		while(fndProjServer->update(100) == TEE_MESSAGE)
		{

			// just debbuging the info :)
			printf("[%s]>: %s\n", fndProjServer->getPacket().getSenderIP(), fndProjServer->getMessage());
			char *recMess = fndProjServer->getMessage();

			if (strncmp(recMess,"YAPH\%",5) == 0) // the response should be something like: YAPH-MACHINE_NAME-PORT_TO_LISTEN_TO
			{
				// we will store the retrieved server as: "MACHINE_NAME;REACH_PORT"
				localProjectors *detectedProj;
				detectedProj = (localProjectors*)malloc(sizeof(localProjectors));

				strtok(fndProjServer->getMessage(), "\%\0");
				char *name = strtok(NULL, "\%\0");

				memset(detectedProj->name,0,127);
				strncpy(detectedProj->name,name,strlen(name));

				char *ip = fndProjServer->getPacket().getSenderIP();
				memset(detectedProj->ip,0,29);
				strncpy(detectedProj->ip,ip,strlen(ip));


				detectedProj->port = atoi(strtok(NULL, "\%\0"));


				char temp_text[64];
				sprintf(temp_text, "%s (%s)", detectedProj->name, detectedProj->ip);



				for(int p = 1; p <= (browser_projetores->size() && !foundSomething); ++p)
				{
					char *curLineText  = (char*)browser_projetores->text(p);
					if(strncmp(curLineText, temp_text, strlen(curLineText)) == 0)
						foundSomething = true;
				}
				if(!foundSomething)
				{
					browser_projetores->add(temp_text,detectedProj);

				}
			}
		}

		--n_tries;
	};
	delete fndProjServer;
	delete fndProjClient;
	if(foundSomething == true)
		return 1;
	else
		return 0;

}
void callback_go_pg2(Fl_Widget*, void*)
{
	memset(ip,0,16);
	if(ipInput->value())
		strncpy(ip, ipInput->value(), 16);
	else
		strncpy(ip,"127.0.0.1" , 16);
	port = 0;
	if(portInput->value())
		port = atoi(portInput->value());

	Pg1->hide();
	Pg2->show();
	//fl_alert("desenha a janela de perfil");
}
void callback_return_pg1(Fl_Widget*, void*)
{
	Pg1->show();
	Pg2->hide();
}

void calibrateS_selected_proj_cb(Fl_Widget*, void*)
{
	localProjectors *selectedProj;
	int selectedIndexInBrowser = browser_projetores->value();
	selectedProj = (localProjectors*)browser_projetores->data(selectedIndexInBrowser);


	strncpy(ip,selectedProj->ip, 16); ip[15] = 0;
	port = selectedProj->port;
	char porta[12];
	sprintf(porta,"%d\0",port);
	ipInput->value(ip);
	portInput->value(porta);

	Pg3->hide();
	Pg1->hide();
	Pg2->show();


}
void show_detect_cb(Fl_Widget*, void*)
{
	if(findProjectorsInLocalNetwork())
	{
		Pg2->hide();
		Pg1->hide();
		Pg3->show();
	}
	else
	{
		Pg2->hide();
		Pg1->show();
		Pg3->hide();

		fl_alert("No projectors found in your local network.");
	}
}

Fl_Double_Window *desenha_selecionar_projetor()
{
	Fl_Double_Window* local_window_projector_selector;
	local_window_projector_selector = new Fl_Double_Window(460, 420, "Select projector for calibration");
	{
		local_window_projector_selector->labelcolor((Fl_Color)30);
		{ Fl_Box* o = new Fl_Box(0, 13, 470, 55);
		  o->image( bg_image17112016 );
		} // Fl_Box* o
		{ Fl_Box* o = new Fl_Box(-10, 23, 20, 42, "Calibrator 1.0.1");
		  o->color(FL_BACKGROUND2_COLOR);
		  o->labelfont(8);
		  o->labelsize(50);
		  o->align(Fl_Align(FL_ALIGN_RIGHT));
		} // Fl_Box* o
		{ Fl_Box* o = new Fl_Box(-10, 20, 20, 42, "Calibrator 1.0.1");
		  o->color(FL_BACKGROUND2_COLOR);
		  o->labelfont(8);
		  o->labelsize(50);
		  o->labelcolor(FL_BACKGROUND2_COLOR);
		  o->align(Fl_Align(FL_ALIGN_RIGHT));
		} // Fl_Box* o
		{ Fl_Group* o = new Fl_Group(0, 94, 459, 318, "Projection");
		  o->box(FL_UP_BOX);
		  o->color((Fl_Color)25);
		  o->labelcolor((Fl_Color)17);
		  o->align(Fl_Align(FL_ALIGN_TOP_LEFT));
		  { Fl_Box* o = new Fl_Box(10, 100, 442, 302);
			o->box(FL_ENGRAVED_BOX);
			o->color((Fl_Color)24);
			o->align(Fl_Align(FL_ALIGN_WRAP));
		  } // Fl_Box* o
		  { Fl_Box* o = new Fl_Box(25, 133, 410, 52, "Autodetect found the following projectors in your local network");
			o->box(FL_EMBOSSED_BOX);
			o->color((Fl_Color)24);
			o->labelcolor(FL_BACKGROUND2_COLOR);
			o->align(Fl_Align(133|FL_ALIGN_INSIDE));
		  } // Fl_Box* o
		  {
			  browser_projetores = new Fl_Browser(25, 208, 410, 154, "Choose one of the following projetors");
			  browser_projetores->tooltip("Choose one of the following projetors");
			  browser_projetores->color((Fl_Color)215);
			  browser_projetores->align(Fl_Align(37));
			  browser_projetores->type(FL_HOLD_BROWSER);
			  browser_projetores->labelcolor(FL_BACKGROUND2_COLOR);

			  /*
			  for(int i = 0; i < 5; i++)
			  {
				  char temp_text[50];
				  sprintf_s(temp_text, "%d. %s", i, "teste");
				  browser_projetores->add(temp_text);
			  }*/
			  o->end();

		  }
		} // Fl_Group* o
		{ Fl_Button* o = new Fl_Button(334, 369, 98, 23, "Calibrate!");
		  o->tooltip("Define this software instance as a projector");
		  o->box(FL_GLEAM_UP_BOX);
		  o->color((Fl_Color)58);
		  o->labelcolor(FL_BACKGROUND2_COLOR);
		  o->callback(calibrateS_selected_proj_cb);
		} // Fl_Button* o
  } // Fl_Double_Window* o

	findProjectorsInLocalNetwork(); // fill the servers list

	local_window_projector_selector->end();
    local_window_projector_selector->show();
	return local_window_projector_selector;
}
Fl_Double_Window *desenha_janela_page_1()
{
	Fl_Double_Window* local_window;
	local_window = new Fl_Double_Window(454, 320);
	local_window->color(FL_LIGHT2);
	local_window->position((Fl::w()/2)-(local_window->decorated_w()/2),
						   (Fl::h()/2)-(local_window->decorated_h()/2));
	local_window->label("Setup profile");
	local_window->color((Fl_Color)30);
	local_window->labelcolor((Fl_Color)30);

	///// ------------------------------------------ BODY

	{ Fl_Box* o = new Fl_Box(0, 10, 460, 55);
		o->image( bg_image17112016 );
    } // Fl_Box* o
	{ Fl_Box* o = new Fl_Box(10, 65, 430, 205);
      o->box(FL_ENGRAVED_BOX);
      o->color((Fl_Color)55);
      o->align(Fl_Align(FL_ALIGN_WRAP));
	  o->color((Fl_Color)30);
	  o->labelcolor((Fl_Color)30);
    } // Fl_Box* o
	{ Fl_Box* o = new Fl_Box(-10, 23, 20, 42, "Calibrator [1.0.1] for Anydome");
      o->color(FL_BACKGROUND2_COLOR);
      o->labelfont(8);
      o->labelsize(25);
      o->align(Fl_Align(FL_ALIGN_RIGHT));
    } // Fl_Box* o
    { Fl_Box* o = new Fl_Box(-11, 22, 20, 42, "Calibrator [1.0.1] for Anydome");
      o->color(FL_BACKGROUND2_COLOR);
      o->labelfont(8);
      o->labelsize(25);
      o->labelcolor(FL_BACKGROUND2_COLOR);
      o->align(Fl_Align(FL_ALIGN_RIGHT));
    } // Fl_Box* o

    { Fl_Box* o = new Fl_Box(15, 70, 420, 70, "Inform the Projector\'s IP and operating port to initiate maintance mode.");
      o->box(FL_EMBOSSED_BOX);
      o->color(FL_WHITE);
      o->labelcolor((Fl_Color)24);
      o->align(Fl_Align(FL_ALIGN_WRAP));
	  o->color((Fl_Color)24);
      o->labelcolor(FL_BACKGROUND2_COLOR);
    } // Fl_Box* o
    { Fl_Box* o = new Fl_Box(-5, 0, 460, 55);
      //o->image(image_psart);
    } // Fl_Box* o
    { Fl_Button* o = new Fl_Button(310, 230, 105, 25, "Ok");
      o->tooltip("Define this software instance as a projector");
      o->color((Fl_Color)26);
	  o->callback(callback_go_pg2);
    } // Fl_Button* o
	{ Fl_Button* o = new Fl_Button(200, 230, 105, 25, "Detect");
		  o->tooltip("Try to discover any projectors in local network");
		  o->box(FL_GLEAM_UP_BOX);
		  o->color((Fl_Color)30);
		  o->labelcolor(FL_BACKGROUND2_COLOR);
		  o->callback(show_detect_cb);
	} // Fl_Button* o
    { ipInput = new Fl_Input(100, 160, 315, 20, "IP");
      ipInput->tooltip("Projector IP");
      ipInput->color((Fl_Color)215);
	  ipInput->value("127.0.0.1");
	  ipInput->labelcolor(FL_BACKGROUND2_COLOR);
    } // Fl_Input* o
    { portInput = new Fl_Input(100, 190, 315, 20, "PORT");
      portInput->tooltip("Projector port");
      portInput->color((Fl_Color)213);
	  portInput->labelcolor(FL_BACKGROUND2_COLOR);
    } // Fl_Input* o
	///// ------------------------------------------ END OF BODY

    local_window->end();
    local_window->show();
	return local_window;
}
Fl_Double_Window *desenha_janela_page_2()
{
	int i = 0; // index to amp the buttons into an array
	Fl_Double_Window* local_window;
	local_window = new Fl_Double_Window(460, 410);
	local_window->color(FL_LIGHT2);
	local_window->position((Fl::w()/2)-(local_window->decorated_w()/2),
						   (Fl::h()/2)-(local_window->decorated_h()/2));
	local_window->label("Setup profile");
	local_window->color((Fl_Color)30);
	local_window->labelcolor((Fl_Color)30);

	///// ------------------------------------------ BODY
	{ Fl_Box* o = new Fl_Box(0, 10, 460, 55);
		o->image( bg_image17112016 );
    } // Fl_Box* o

	Fl_Group *mbc=(Fl_Group *)0;

	Fl_Box *mbcb=(Fl_Box *)0;

	Fl_Button *bt=(Fl_Button *)0;

	Fl_Box *ib1=(Fl_Box *)0;

	Fl_Button *ba=(Fl_Button *)0;

	Fl_Button *bs=(Fl_Button *)0;

	Fl_Button *bd=(Fl_Button *)0;

	Fl_Button *bw=(Fl_Button *)0;

	Fl_Button *be=(Fl_Button *)0;

	Fl_Button *bq=(Fl_Button *)0;

	Fl_Button *br=(Fl_Button *)0;

	Fl_Button *bf=(Fl_Button *)0;

	Fl_Button *bc=(Fl_Button *)0;

	Fl_Box *ib2=(Fl_Box *)0;

	Fl_Input *ev=(Fl_Input *)0;

	Fl_Button *bp=(Fl_Button *)0;

	Fl_Button *bb=(Fl_Button *)0;




	{ Fl_Box* o = new Fl_Box(-10, 24, 20, 42, "Calibrator 1.0.1");
      o->color(FL_BACKGROUND2_COLOR);
      o->labelfont(8);
      o->labelsize(40);
      o->align(Fl_Align(FL_ALIGN_RIGHT));
    } // Fl_Box* o
    { Fl_Box* o = new Fl_Box(-9, 22, 20, 42, "Calibrator 1.0.1");
      o->color(FL_BACKGROUND2_COLOR);
      o->labelfont(8);
      o->labelsize(40);
      o->labelcolor(FL_BACKGROUND2_COLOR);
      o->align(Fl_Align(FL_ALIGN_RIGHT));
    } // Fl_Box* o
      mbc = new Fl_Group(4, 116, 455, 296, "Projection");
      mbc->box(FL_UP_BOX);
      mbc->color((Fl_Color)25);
      mbc->labelcolor((Fl_Color)17);
      mbc->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      { mbcb = new Fl_Box(10, 128, 442, 277);
        mbcb->box(FL_ENGRAVED_BOX);
        mbcb->color((Fl_Color)24);
        mbcb->align(Fl_Align(FL_ALIGN_WRAP));
      } // Fl_Box* mbcb

      { ib1 = new Fl_Box(49, 148, 147, 70, "Use this keys to iterate the mode");
        ib1->box(FL_EMBOSSED_BOX);
        ib1->color((Fl_Color)24);
        ib1->labelcolor(FL_BACKGROUND2_COLOR);
        ib1->align(Fl_Align(FL_ALIGN_WRAP));
      } // Fl_Box* ib1

	  { bq = new Fl_Button(54, 224, 40, 40, "Q");
        bq->tooltip("Define this software instance as a projector");
        bq->color(FL_GRAY0);
        bq->labelcolor((Fl_Color)17);
		butts[i++] = bq;
      } // Fl_Button* bq
      { bw = new Fl_Button(94, 224, 40, 40, "W");
        bw->tooltip("Define this software instance as a projector");
        bw->color(FL_GRAY0);
        bw->labelcolor((Fl_Color)17);
		butts[i++] = bw;
      } // Fl_Button* bw
      { be = new Fl_Button(134, 224, 40, 40, "E");
        be->tooltip("Define this software instance as a projector");
        be->color(FL_GRAY0);
        be->labelcolor((Fl_Color)17);
		butts[i++] = be;
      } // Fl_Button* be
	  { ba = new Fl_Button(64, 264, 40, 40, "A");
        ba->tooltip("Define this software instance as a projector");
        ba->color(FL_GRAY0);
        ba->labelcolor((Fl_Color)17);
		butts[i++] = ba;
      } // Fl_Button* ba
      { bs = new Fl_Button(104, 264, 40, 40, "S");
        bs->tooltip("Define this software instance as a projector");
        bs->color(FL_GRAY0);
        bs->labelcolor((Fl_Color)17);
		butts[i++] = bs;
      } // Fl_Button* bs
      { bd = new Fl_Button(144, 264, 40, 40, "D");
        bd->tooltip("Define this software instance as a projector");
        bd->color(FL_GRAY0);
        bd->labelcolor((Fl_Color)17);
		butts[i++] = bd;
      } // Fl_Button* bd


      { br = new Fl_Button(255, 228, 125, 25, "r");
        br->tooltip("Define this software instance as a projector");
        br->color((Fl_Color)24);
		butts[i++] = br;
      } // Fl_Button* br
	  { bt = new Fl_Button(380, 228, 50, 25, "t");
        bt->tooltip("Define this software instance as a projector");
        bt->color((Fl_Color)24);
		butts[i++] = bt;
      } // Fl_Button* bt
      { bf = new Fl_Button(205, 253, 115, 25, "f");
        bf->tooltip("Define this software instance as a projector");
        bf->color((Fl_Color)24);
		butts[i++] = bf;
      } // Fl_Button* bf
	  { bp = new Fl_Button(320, 253, 110, 25, "p");
        bp->tooltip("Define this software instance as a projector");
        bp->color((Fl_Color)24);
		butts[i++] = bp;
      } // Fl_Button* bp
      { bb = new Fl_Button(205, 278, 50, 25, "b");
        bb->tooltip("Define this software instance as a projector");
        bb->color((Fl_Color)24);
		butts[i++] = bb;
      } // Fl_Button* bb
      { bc = new Fl_Button(255, 278, 125, 25, "c");
        bc->tooltip("Define this software instance as a projector");
        bc->color((Fl_Color)24);
		butts[i++] = bc;
      } // Fl_Button* bf
      { ib2 = new Fl_Box(216, 150, 214, 68, "Select the mode (Position, Rotatio, FOV) - Continuous, back (reset) and write\
 data (t)");
        ib2->box(FL_EMBOSSED_BOX);
        ib2->color((Fl_Color)24);
        ib2->labelcolor(FL_BACKGROUND2_COLOR);
        ib2->align(Fl_Align(FL_ALIGN_WRAP));
      } // Fl_Box* ib2
      { inputValToSend = new Fl_Input(50, 351, 230, 24, "Current tool val: ");
        inputValToSend->color(FL_GRAY0);
        inputValToSend->selection_color((Fl_Color)2);
        inputValToSend->labelcolor(FL_BACKGROUND2_COLOR);
        inputValToSend->textcolor((Fl_Color)62);
        inputValToSend->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Input* ev

	 { Fl_Button* o = new Fl_Button(310, 351, 105, 25, "Return");
      o->tooltip("Return to the main menu to define the projector address for calibration.");
      o->box(FL_GLEAM_UP_BOX);
      o->color((Fl_Color)81);
      o->labelcolor(FL_BACKGROUND2_COLOR);
	  o->callback(callback_return_pg1);
    } // Fl_Button* o

	///// ------------------------------------------ END OF BODY



    local_window->end();
    local_window->show();
	return local_window;
}

void updateButtonsActivation()
{
	for(int i = 0; i < 6; i++) // the six first must be directly activate by the keyboard, the order is defined in the creation order of the  "desenha_janela_page_2()" function
	{
		butts[i]->value(iskeyPressed(butts[i]->label()[0]));
	}
	butts[7]->value(iskeyPressed(butts[7]->label()[0])); // save/write data (letter t)
	butts[10]->value(iskeyPressed(butts[10]->label()[0])); // reset calibration (letter b)

	// togglable buttons
	butts[9]->value(mode == 1); // p
	butts[6]->value(mode == 2); // r
	butts[8]->value(mode == 3); // f
	butts[11]->value(continuous);



}
void saveCurrentDefVal()
{
	if(mode == 1)
		strcpy(posDefVal, (char *)inputValToSend->value());
	if(mode == 2)
		strcpy(rotDefVal, (char *)inputValToSend->value());
	if(mode == 3)
		strcpy(fovDefVal, (char *)inputValToSend->value());
}
void handleKeysProgram() // this function is the one that tells what we want to do when we press the keys.
{
	// send the data to calibrate
	// CC{Pp,Rr,F};(p,r){X,Y,Z};VALUE;   <- this is the tag formula
	char commandLabel[6] = {'c', 'c', 0, ';', 'a', ';'};;
	char finalCommandString[32]; memset(finalCommandString,0,32); //32 bytes is a estimated  number that we that we think that it can fit all the possible command lengths
	char modeSelected = 0, axisSelected;
	strncpy(finalCommandString,commandLabel, 6);

	if(mode == 1)
		modeSelected = 'p'; // position
	else if(mode == 2)
		modeSelected = 'r'; // rotation
	else if(mode == 3)
		modeSelected = 'f'; // scale

	finalCommandString[2] = modeSelected;


	if((continuous && iskeyPressed('w')) || (!continuous) && keyPressed('w'))
	{
		printf("W");

		//determinate the axis according to the mode we are

		if(modeSelected == 'p') // position mode
		{
			finalCommandString[3] = ';';
			finalCommandString[4] = 'z'; // use this axis
			finalCommandString[5] = ';';
			strcat(finalCommandString,inputValToSend->value());
			strcat(finalCommandString,";");
		}
		else if(modeSelected == 'r') // position mode
		{
			finalCommandString[3] = ';';
			finalCommandString[4] = 'x'; // use this axis
			finalCommandString[5] = ';';
			strcat(finalCommandString,inputValToSend->value());
			strcat(finalCommandString,";");
		}
		if(modeSelected != 'f') // dont send if its i nthe fov mode for this key
			calibratorClinet->send(finalCommandString, port, ip, strlen("finalCommandString"));
	}
	if((continuous && iskeyPressed('s')) || (!continuous) && keyPressed('s'))
	{
		printf("S");
		if(modeSelected == 'p') // position mode
		{
			finalCommandString[3] = ';';
			finalCommandString[4] = 'z'; // use this axis
			finalCommandString[5] = ';';
			finalCommandString[6] = '-';
			strcat(finalCommandString,inputValToSend->value());
			strcat(finalCommandString,";");
		}
		else if(modeSelected == 'r') // position mode
		{
			finalCommandString[3] = ';';
			finalCommandString[4] = 'x'; // use this axis
			finalCommandString[5] = ';';
			finalCommandString[6] = '-';
			strcat(finalCommandString,inputValToSend->value());
			strcat(finalCommandString,";");
		}
		if(modeSelected != 'f') // dont send if its i nthe fov mode for this key
			calibratorClinet->send(finalCommandString, port, ip, strlen("finalCommandString"));
	}
	if((continuous && iskeyPressed('d')) || (!continuous) && keyPressed('d'))
	{
		printf("D");

		if(modeSelected == 'p') // position mode
		{
			finalCommandString[3] = ';';
			finalCommandString[4] = 'x'; // use this axis
			finalCommandString[5] = ';';
			strcat(finalCommandString,inputValToSend->value());
			strcat(finalCommandString,";");
		}
		else if(modeSelected == 'r') // position mode
		{
			finalCommandString[3] = ';';
			finalCommandString[4] = 'z'; // use this axis
			finalCommandString[5] = ';';
			strcat(finalCommandString,inputValToSend->value());
			strcat(finalCommandString,";");
		}
		if(modeSelected != 'f') // dont send if its i nthe fov mode for this key
			calibratorClinet->send(finalCommandString, port, ip, strlen("finalCommandString"));
	}
	if((continuous && iskeyPressed('a')) || (!continuous) && keyPressed('a'))
	{
		printf("A");

		if(modeSelected == 'p') // position mode
		{
			finalCommandString[3] = ';';
			finalCommandString[4] = 'x'; // use this axis
			finalCommandString[5] = ';';
			finalCommandString[6] = '-';
			strcat(finalCommandString,inputValToSend->value());
			strcat(finalCommandString,";");
		}
		else if(modeSelected == 'r') // position mode
		{
			finalCommandString[3] = ';';
			finalCommandString[4] = 'z'; // use this axis
			finalCommandString[5] = ';';
			finalCommandString[6] = '-';
			strcat(finalCommandString,inputValToSend->value());
			strcat(finalCommandString,";");
		}
		if(modeSelected != 'f') // dont send if its i nthe fov mode for this key
			calibratorClinet->send(finalCommandString, port, ip, strlen("finalCommandString"));
	}
	if((continuous && iskeyPressed('q')) || (!continuous) && keyPressed('q'))
	{
		printf("Q");
		if(modeSelected == 'p') // position mode
		{
			finalCommandString[3] = ';';
			finalCommandString[4] = 'y'; // use this axis
			finalCommandString[5] = ';';
			finalCommandString[6] = '-';
			strcat(finalCommandString,inputValToSend->value());
			strcat(finalCommandString,";");
		}
		else if(modeSelected == 'r') // position mode
		{
			finalCommandString[3] = ';';
			finalCommandString[4] = 'y'; // use this axis
			finalCommandString[5] = ';';
			finalCommandString[6] = '-';
			strcat(finalCommandString,inputValToSend->value());
			strcat(finalCommandString,";");
		}
		else if(modeSelected == 'f')
		{
			finalCommandString[3] = ';';
			finalCommandString[4] = '-';
			strcat(finalCommandString,inputValToSend->value());
			strcat(finalCommandString,";");
		}
		calibratorClinet->send(finalCommandString, port, ip, strlen("finalCommandString"));
	}
	if((continuous && iskeyPressed('e')) || (!continuous) && keyPressed('e'))
	{
		printf("E");

		if(modeSelected == 'p') // position mode
		{
			finalCommandString[3] = ';';
			finalCommandString[4] = 'y'; // use this axis
			finalCommandString[5] = ';';
			strcat(finalCommandString,inputValToSend->value());
			strcat(finalCommandString,";");
		}
		else if(modeSelected == 'r') // position mode
		{
			finalCommandString[3] = ';';
			finalCommandString[4] = 'y'; // use this axis
			finalCommandString[5] = ';';
			strcat(finalCommandString,inputValToSend->value());
			strcat(finalCommandString,";");
		}
		else if(modeSelected == 'f')
		{
			finalCommandString[3] = ';';
			strcat(finalCommandString,inputValToSend->value());
			strcat(finalCommandString,";");
		}
		calibratorClinet->send(finalCommandString, port, ip, strlen("finalCommandString"));
	}




	if(keyPressed('t')) // dump save write the data
	{
		calibratorClinet->send("dmp;", port, ip, strlen("dmp;"));
	}
	if(keyPressed('b')) // back, reset the calibration
	{
		calibratorClinet->send("rst;", port, ip, strlen("rst;"));
	}




	// switch the modes
	if((keyPressed('p'))) // position mode
	{
		saveCurrentDefVal();
		inputValToSend->value(posDefVal);
		mode = 1;
	}
	if(( keyPressed('r'))) // rotation mode
	{
		saveCurrentDefVal();
		inputValToSend->value(rotDefVal);
		mode = 2;
	}
	if((keyPressed('f'))) // fov mode
	{
		saveCurrentDefVal();
		inputValToSend->value(fovDefVal);
		mode = 3;
	}
	if(keyPressed('c')) // continuous
	{
		continuous = !continuous;
	}
}




int main()
{
	 /* initialize random seed: */
	 srand (time(NULL));

	char my_message[1024];

	// hide windows console
	//ShowWindow( GetConsoleWindow(), SW_HIDE );

	flushKeyStates();

	// start the engine
	startTNetEngine(); // obligatory to make the engine work
	calibratorClinet = new TNETClient();


	// setup a simple udp server
	//calibratorClinet = new TNETClient(9001, "127.0.0.1");

	Pg1 = desenha_janela_page_1();Pg1->hide();
	Pg2 = desenha_janela_page_2(); Pg2->hide();
	Pg3 = desenha_selecionar_projetor(); //Pg3->hide();
	if(browser_projetores->size() < 1)
	{
		Pg3->hide();
		Pg1->show();
	}


	while(Fl::check())
	{

		if(Pg2->visible())
		{
			updateKeyStates();

			handleKeysProgram();

			updateButtonsActivation();
			updateKePastStates();
		}
	}
	flush_browser_projetores(); // free the list of pointers
	return 1;
}
