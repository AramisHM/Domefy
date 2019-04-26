/*	This file is part of the FpMED (Domefy) Project Copyright (c) 2014-2017 Aramis Hornung Moraes
	See Fpmed.h for license and conditions for using this code
*/

#include "Projector.h"
#include "CoreParameters.h"
namespace fpmed
{

// Construtor
Projector::Projector(){
	memset(name,0,name_len);

	index = -1;
	fov = 45; // in degrees
	strncpy(name, "Untitled projector/0", name_len-1);

	// Fulldome properties
	position = fpmed::Vec3<float>(0,0,0);
	rotation = fpmed::Vec3<float>(0,0,0);
	scale = fpmed::Vec3<float>(0,0,0);

	far_clip = 250.0f;

	offsetPosition = fpmed::Vec3<float>(0,0,0);
	offsetRotation = fpmed::Vec3<float>(0,0,0);

	// Planar properties
	Clip = fpmed::Vec4<int>(0,0,-1,-1);// screen clipping -1 indicates a invalid delta. so we know its not initialized

	//Indicates the eyee to simulate stereoscopy
	eye_side = 'n'; // values are either l, or r, both lowercased. "n" identifies none of them.
}
Projector::~Projector()
{
}
void Projector::setPosition(Vec3<float> pos)
{
	position = pos;
}

void Projector::setRotation(Vec3<float> rot)
{
	rotation = rot;
}
void Projector::setScale(Vec3<float> s)
{
	scale = s;
}
void Projector::setFov(float f)
{
	fov = f;
}
void Projector::setClip(Vec4<int> c)
{
	Clip = c;
}

Vec3<float> Projector::getPosition()
{
	return position;
}
Vec3<float> Projector::getRotation()
{
	return rotation;
}
Vec3<float> Projector::getScale()
{
	return scale;
}
float Projector::getFov()
{
	return fov;
}

Vec4<int> Projector::getClip()
{
	return Clip;
}
void Projector::setIndex(int idx)
{
	index = idx;
}
int Projector::getIndex()
{
	return index;
}

void Projector::setName (char *pname)
{
	memset(name,0,MediumDataLen);
	strncpy(name,pname,strlen(pname));
}
char *Projector::getName ()
{
	return name;
}

// eye side, for stereoscopic features
void Projector::setSterioscopicEye(char v_eye) // possible values are: "l" "r" "n"
{
	eye_side = v_eye;
}
char Projector::getSterioscopicEye()
{
	return eye_side;
}

void Projector::setFarClip(float fc)
{
	far_clip = fc;
}
float Projector::getFarClip()
{
	return far_clip;
}

void Projector::setOffsetPosition(fpmed::Vec3<float> opos)
{
	offsetPosition = opos;
}
fpmed::Vec3<float>  Projector::getOffsetPosition()
{
	return offsetPosition;
}

void Projector::setOffsetRotation(fpmed::Vec3<float> orot)
{
	offsetRotation = orot;
}
fpmed::Vec3<float> Projector::getOffsetRotation()
{
	return offsetRotation;
}
}// fpmed namespace
