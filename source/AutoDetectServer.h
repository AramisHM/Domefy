#ifndef AUTODETECTSERVER_H
#define AUTODETECTSERVER_H
// Autodetect local network's projector

#include <string>
#include "FpmedCore.h"
#include "tnet/TNet.h"

void initializeAutoDetecServer();
void UpdateAutoDetectProjectorServer();
void deinitializeAutoDetecServer();
int UpdateAuxiliarGlobalCommands();
void sendExternalCommand(std::string command, int ntries = 1);
bool isCommandEquals(std::string command, std::string compareStr);
float getCommandValueAsFloat(std::string command);
std::string updateExternalCommands();
#endif
