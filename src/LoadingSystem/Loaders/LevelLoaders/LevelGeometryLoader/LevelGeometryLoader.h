/*
 * LevelGeometryLoader.h
 *
 *  Created on: 22/04/2012
 *      Author: agustin
 *
 */

#ifndef LEVELGEOMETRYLOADER_H_
#define LEVELGEOMETRYLOADER_H_

#include <OgreStaticGeometry.h>
#include <OgreSceneNode.h>

#include <vector>

#include "Loader.h"
#include "LoaderData.h"
#include "tinyxml.h"

class TiXmlElement;


class LevelGeometryLoader : public Loader {
public:
	LevelGeometryLoader();
	virtual ~LevelGeometryLoader();


	// Functoin used to load something
	virtual int load(TiXmlElement*, LoaderData *data);

	// Unload the information?
	virtual int unload();

private:
	bool processEntityStaticGeoemtry(TiXmlElement *XMLNode, Ogre::StaticGeometry *sgeo);
	bool processStaticGeometries(TiXmlElement *XMLRoot);
	Ogre::Entity *processEntity(TiXmlElement *XMLRoot);
	bool processNode(TiXmlElement *XMLRoot, Ogre::SceneNode *&node);
	bool processNodes(TiXmlElement *XMLRoot);

private:
	std::vector<Ogre::StaticGeometry *> 			mStaticGeometry;
	std::vector<Ogre::SceneNode *>					mSceneNodes;
};

#endif /* LEVELGEOMETRYLOADER_H_ */
