/*	This file is part of the FpMED (Anydome) Project Copyright (c) 2014-2017 Aramis Hornung Moraes
	See FpmedCore.h for license and conditions for using this code
*/

#include "Environment.h"
#include "CoreParameters.h"
#include "irrXML.h"
#include <string>

//Irrlicht namespaces
using namespace irr;
using namespace io;

namespace fpmed
{

Environment::Environment()
{
	memset(infoName,0,MediumDataLen-1);
	memset(manufacturerName,0,MediumDataLen-1);
	memset(authorName,0,MediumDataLen-1);
	strncpy(infoName, "No description", MediumDataLen-1);
	strncpy(manufacturerName, "Undefined manufacturer", MediumDataLen-1);
	strncpy(authorName, "Undefined author", MediumDataLen-1);
	location = fpmed::Vec2<float>(-1, -1); // location of the digital theater
	// General atributes for environment
	projection_type = projection_configuration = 0; // Type refers to either fulldome or planar.
													// Configuration refers to the way the system
													// is displaying, i.e. 3 monitor in line, duet
													// fulldome with stereoscopy etc...
	resolution = fpmed::Vec2<int>(800,600); // You may note by now that we only work with multiple equipaments
									// but with the same resolution.
	// Only for plannar projections
	screen_coluns = screen_rows = 1;

	// Fulldome properties
	dome_position = fpmed::Vec3<float>(0,0,0);
	dome_rotation = dome_position = fpmed::Vec3<float>(0,0,0);
	dome_scale = dome_position = fpmed::Vec3<float>(1,1,1);
}
Environment::~Environment()
{
}

//File functions
void Environment::loadEnvironmentFromFile(char* path)
{
	IrrXMLReader* xml = createIrrXMLReader(path);

	// strings for storing the data we want to get out of the file
	std::string modelFile;
	std::string messageText;
	std::string caption;

	//if(!xml)
	//	return -1;
	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case EXN_ELEMENT:
			{
				if (!strcmp("info", xml->getNodeName())) // INFO SEGMENT
				{
					setInfoName((char*)xml->getAttributeValue("name"));	// set THIS environment object ManufacturerName
					do {
						if (xml->getNodeType() == EXN_ELEMENT)
						{
							if(!strcmp("manufacturer", xml->getNodeName()))
								setManufacturerName((char*)xml->getAttributeValue("name"));	// set THIS environment object ManufacturerName
							if(!strcmp("author", xml->getNodeName()))
								setAuthorName((char*)xml->getAttributeValue("name"));	// set THIS environment object AuthorName
							if(!strcmp("location", xml->getNodeName()))
							{
								fpmed::Vec2<float> vector;
								vector.atovec2((char*)xml->getAttributeValue("value"));
								setLocation(vector);
							}
						}
						xml->read();

					}while(xml->getNodeType() != EXN_ELEMENT_END || strcmp("info", xml->getNodeName()));
				} // END OF INFO SEGMENT


				if (!strcmp("attributes", xml->getNodeName())) // ATTRIBUTES SEGMENT
				{
					do {
						if (xml->getNodeType() == EXN_ELEMENT)
						{
							if(!strcmp("ProjectionType", xml->getNodeName()))
							{
								setProjectionType(xml->getAttributeValueAsInt("value"));
							}
							if(!strcmp("ProjectionConfigurationId", xml->getNodeName()))
							{
								setConfiguration(atoi(xml->getAttributeValue("value")));
							}
							if(!strcmp("vector2d", xml->getNodeName()) && !strcmp("resolution", xml->getAttributeValue("name")))
							{
								fpmed::Vec2<int> vector;
								vector.atovec2((char*)xml->getAttributeValue("value"));
								setScreenResolution(vector);
							}
							if(!strcmp("integer", xml->getNodeName()))
							{
								if(!strcmp("ScreenColuns", (char*)xml->getAttributeValue("name")))
								{
									setNumberOfScreenHorizontal(xml->getAttributeValueAsInt("value"));
								}
								if(!strcmp("ScreenRows", (char*)xml->getAttributeValue("name")))
								{
									setNumberOfScreensVertical(xml->getAttributeValueAsInt("value"));
								}
							}
							if(!strcmp("vector3d", xml->getNodeName()))
							{
								if(!strcmp("DomePosition", xml->getAttributeValue("name")))
								{
									fpmed::Vec3<float> vector;
									vector.atovec3((char*)xml->getAttributeValue("value"));
									setDomePosition(vector);
								}
								if(!strcmp("DomeRotation", xml->getAttributeValue("name")))
								{
									fpmed::Vec3<float> vector;
									vector.atovec3((char*)xml->getAttributeValue("value"));
									setDomeRotation(vector);
								}
								if(!strcmp("DomeScale", xml->getAttributeValue("name")))
								{
									fpmed::Vec3<float> vector;
									vector.atovec3((char*)xml->getAttributeValue("value"));
									setDomeScale(vector);
								}
							}
							if(!strcmp("FullScreen", xml->getNodeName()))
							{
								setFullScreen(atoi(xml->getAttributeValue("value")));
							}
						}
						xml->read();
					}while(xml->getNodeType() != EXN_ELEMENT_END || strcmp("attributes", xml->getNodeName()));
				} // END OF ATTRIBUTES SEGMENT


				if (!strcmp("projector", xml->getNodeName())) // PROJECTOR SEGMENT
				{
					Projector projector;
					projector.setIndex(xml->getAttributeValueAsInt("index"));
					projector.setName((char*)xml->getAttributeValue("name"));
					do {
						if (xml->getNodeType() == EXN_ELEMENT)
						{

							if(!strcmp("EyeView", xml->getNodeName()))
							{
								projector.setSterioscopicEye(xml->getAttributeValueSafe("value")[0]);
							}

							if(!strcmp("float", xml->getNodeName()))
							{
								if(!strcmp("FOV", xml->getAttributeValue("name")))
								{
									projector.setFov(xml->getAttributeValueAsFloat("value"));
								}
								if(!strcmp("FarClip", xml->getAttributeValue("name")))
								{
									projector.setFarClip(xml->getAttributeValueAsFloat("value"));
								}
							}
							if(!strcmp("vector4d", xml->getNodeName()))
							{
								if(!strcmp("ScreenClip", xml->getAttributeValue("name")))
								{
									fpmed::Vec4<int> vector;
									vector.atovec4((char*)xml->getAttributeValue("value"));
									projector.setClip(vector);
								}
							}
							if(!strcmp("vector3d", xml->getNodeName()))
							{
								if(!strcmp("OffsetPosition", xml->getAttributeValue("name"))) // refered to the camera we are simulating from the server scene
								{
									fpmed::Vec3<float> vector;
									vector.atovec3((char*)xml->getAttributeValue("value"));
									projector.setOffsetPosition(vector);
								}
								if(!strcmp("OffsetRotation", xml->getAttributeValue("name"))) // refered to the camera we are simulating from the server scene
								{
									fpmed::Vec3<float> vector;
									vector.atovec3((char*)xml->getAttributeValue("value"));
									projector.setOffsetRotation(vector);
								}

								if(!strcmp("ProjectorPosition", xml->getAttributeValue("name"))) // refered to the virtual dome
								{
									fpmed::Vec3<float> vector;
									vector.atovec3((char*)xml->getAttributeValue("value"));
									projector.setPosition(vector);
								}
								if(!strcmp("ProjectorRotation", xml->getAttributeValue("name")))
								{
									fpmed::Vec3<float> vector;
									vector.atovec3((char*)xml->getAttributeValue("value"));
									projector.setRotation(vector);
								}
								if(!strcmp("ProjectorScale", xml->getAttributeValue("name")))
								{
									fpmed::Vec3<float> vector;
									vector.atovec3((char*)xml->getAttributeValue("value"));
									projector.setScale(vector);
								}
							}
						}
						xml->read();
					}while(xml->getNodeType() != EXN_ELEMENT_END || strcmp("projector", xml->getNodeName()));

					pushProjector(projector);

				} // END OF PROJECTOR SEGMENT



			}
			break;
		case EXN_ELEMENT_END:
			if (!strcmp("FpMED", xml->getNodeName()))
				{
					printf("End of FpMED file reached2.\n");
					break;
				}
	break;

		}

	}
	// delete the xml parser after usage
	delete xml;

	//return 1;
}
void Environment::saveEnvironmentToFile(char* path)
{

}

void Environment::setInfoName (char name[512]){
	strcpy(infoName, name);
}
void Environment::setManufacturerName (char name[512]){
	strcpy(manufacturerName, name);
}
void Environment::setAuthorName (char name[512]){
	strcpy(authorName, name);
}
char *Environment::getInfoName (){
	return infoName;
}
char *Environment::getManufacturerName (){
	return manufacturerName;
}
char *Environment::getAuthorName (){
	return authorName;
}


void Environment::pushProjector(Projector proj)
{
	environmentProjectors.push_back(proj);
}

void Environment::clearProjectors()
{
	environmentProjectors.clear();
}

Projector Environment::getProjector(int index)
{
	return environmentProjectors[index];
}

int Environment::countProjectors()
{
	return environmentProjectors.size();
}

// Construtor
void Environment::setProjectionType(int proj_t)
{
	projection_type = proj_t;
}
void Environment::setConfiguration(int proj_c)
{
	projection_configuration = proj_c;
}
void Environment::setScreenResolution(fpmed::Vec2<int> res){
	resolution = res;
}
void Environment::setNumberOfScreenHorizontal(int n_screens){
	screen_coluns = n_screens;
}
void Environment::setNumberOfScreensVertical(int n_screens){
	screen_rows = n_screens;
}

int Environment::getProjectionType(){
	return projection_type;
}
int Environment::getConfiguration(){
	return projection_configuration;
}
Vec2<int> Environment::getScreenResolution(){
	return resolution;
}
int Environment::getNumberOfScreenHorizontal(){
	return screen_coluns;
}
int Environment::getNumberOfScreensVertical(){
	return screen_rows;
}

//Fulldome projection properties

void Environment::setDomePosition(fpmed::Vec3<float> position){
	dome_position = position;
}
void Environment::setDomeRotation(fpmed::Vec3<float> rotation){
	dome_rotation = rotation;
}
void Environment::setDomeScale(fpmed::Vec3<float> scale){
	dome_scale = scale;
}

Vec3<float> Environment::getDomePosition(){
	return dome_position;
}
Vec3<float> Environment::getDomeRotation(){
	return dome_rotation;
}
Vec3<float> Environment::getDomeScale(){
	return dome_scale;
}

// digital theather location
fpmed::Vec2<float> Environment::getLocation()
{
	return location;
}
void Environment::setLocation(fpmed::Vec2<float> loc)
{
	location = loc;
}
int Environment::getNumberOfProjectors()
{
	return environmentProjectors.size();
}

int Environment::getFullScreen()
{
	return fullScreen;
}
void Environment::setFullScreen(int val)
{
	fullScreen = val;
}

}// fpmed namespace
