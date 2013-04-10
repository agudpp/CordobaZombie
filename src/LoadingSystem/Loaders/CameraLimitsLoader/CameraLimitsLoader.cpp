/*
 * CameraLimitsLoader.cpp
 *
 *  Created on: 29/03/2012
 *      Author: Emi Daniele
 */

#include "DotSceneLoader.h"
#include "CameraLimitsLoader.h"



void CameraLimitsLoader::parseLimits(TiXmlElement *pElem)
{
	ASSERT(pElem);
	ASSERT(Ogre::String(pElem->Value()) == "Limits");

	minX = Ogre::DotSceneLoader::getAttribReal(pElem, "minX");
	maxX = Ogre::DotSceneLoader::getAttribReal(pElem, "maxX");
	minY = Ogre::DotSceneLoader::getAttribReal(pElem, "minY");
	maxY = Ogre::DotSceneLoader::getAttribReal(pElem, "maxY");
	minZ = Ogre::DotSceneLoader::getAttribReal(pElem, "minZ");
	maxZ = Ogre::DotSceneLoader::getAttribReal(pElem, "maxZ");

	configureController();


}


CameraLimitsLoader::CameraLimitsLoader(CameraController * controller,const Ogre::String &name) :
Loader(name)
{
	box = 0;
	mController = controller;

}


CameraLimitsLoader::~CameraLimitsLoader()
{
	delete box;

}


int CameraLimitsLoader::load(TiXmlElement *elem)
{
	if( Ogre::String( elem->Value()) != "Camera"  ) {
		debug("Error: Invalid Environment File. Missing \"Camera element in XML\"" );
		ASSERT(false);
		return -1;
	}

	TiXmlElement *pElem = elem->FirstChildElement("Limits");
	if(pElem){
		debug("Setting limits to camera movements\n");
		parseLimits(pElem);
	}


	return 0;
}


int CameraLimitsLoader::unload(void)
{
	return 0;
}

void CameraLimitsLoader::configureController(){
	box = new Ogre::AxisAlignedBox();
	box->setMaximum(maxX, maxY, maxZ);
	box->setMinimum(minX, minY, minZ);

	ASSERT(false); // TODO
//	mController->setCameraMoveZone(*box);
//	mController->setMaxRotateX(Ogre::Radian(0), Ogre::Radian((-Ogre::Math::PI-0.1f)/2.0f));
//	mController->setCamPos(Ogre::Vector3(500,500,500));
//	mController->setRotationX(Ogre::Radian(-Ogre::Math::PI/2.0f));
//	mController->setCameraVelocity(5.0f);
}
