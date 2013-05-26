/*
 * CameraLimitsLoader.h
 *
 *  Created on: 29/03/2012
 *      Author: Emi Daniele
 */

#ifndef CAMERALIMITSLOADER_H_
#define CAMERALIMITSLOADER_H_

#include <OgreEntity.h>
#include <OgreString.h>

#include <string>
#include <list>

#include "DebugUtil.h"
#include "Loader.h"
#include "tinyxml.h"
#include "GlobalObjects.h"

#include "CameraController.h"


class TiXmlElement;


class CameraLimitsLoader : public Loader
{

private:	
	void parseLimits(TiXmlElement*);
	Ogre::AxisAlignedBox* box;
	void configureController();

	CameraController *mController;
	Ogre::Real minX, minY, minZ, maxX, maxY, maxZ;



public:
	CameraLimitsLoader(CameraController * controller, const Ogre::String &name = "CameraLimitsLoader");
	virtual ~CameraLimitsLoader();
	int load(TiXmlElement*);
	int unload(void);

	Ogre::AxisAlignedBox* getCameraBox(){return box;};


};

#endif /* CAMERALIMITSLOADER_H_*/
