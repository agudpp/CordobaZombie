/*
 * LevelGeometryLoader.cpp
 *
 *  Created on: 22/04/2012
 *      Author: agustin
 *
 */

#include <OgreEntity.h>
#include <OgreSubEntity.h>
#include <algorithm>
#include <cmath>

#include "DotSceneLoader.h"
#include "DebugUtil.h"
#include "GlobalObjects.h"

#include "LevelGeometryLoader.h"



////////////////////////////////////////////////////////////////////////////////
bool LevelGeometryLoader::processEntityStaticGeoemtry(TiXmlElement *XMLNode,
		Ogre::StaticGeometry *sgeo)
{
	ASSERT(XMLNode);
	ASSERT(sgeo);

	// Process attributes
	Ogre::String meshFile = Ogre::DotSceneLoader::getAttrib(XMLNode, "meshFile");
	Ogre::Vector3 position = Ogre::Vector3::ZERO;
	Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;
	Ogre::Vector3 scale = Ogre::Vector3::UNIT_SCALE;
	std::vector<Ogre::String> subEntities;


	// parse the entity

	// get the origin
	TiXmlElement *auxPElem = XMLNode->FirstChildElement("position");
	if(auxPElem) {
		position = Ogre::DotSceneLoader::parseVector3(auxPElem);
	}

	// get the rotation
	auxPElem = XMLNode->FirstChildElement("rotation");
	if(auxPElem) {
		rotation = Ogre::DotSceneLoader::parseQuaternion(auxPElem);
	}

	// get the scale
	auxPElem = XMLNode->FirstChildElement("scale");
	if(auxPElem) {
		scale = Ogre::DotSceneLoader::parseVector3(auxPElem);
	}

	size_t count = 0;
	// load the subentities
	TiXmlElement *pElement = XMLNode->FirstChildElement("subentities");
	if(pElement){
		auxPElem = pElement->FirstChildElement("subentity");
		// ugly! very
		while(auxPElem) {
			auxPElem = auxPElem->NextSiblingElement("subentity");
			count++;
		}
		subEntities.resize(count);
		pElement = pElement->FirstChildElement("subentity");
		while(pElement){
			int index = Ogre::DotSceneLoader::getAttribInt(pElement, "index");

			subEntities[index] = Ogre::DotSceneLoader::getAttrib(pElement, "materialName");

			pElement = pElement->NextSiblingElement("subentity");
		}
	}

	// creates the entity
	Ogre::Entity *ent = 0;
	try {
		ent = Common::GlobalObjects::sceneManager->createEntity("entity_" + meshFile, meshFile);
	} catch (...) {
		return false;
	}

	// set the subentities config
	size_t subentityCount = std::min(subEntities.size(), (size_t)ent->getNumSubEntities());
	for (size_t subentityIndex = 0; subentityIndex < subentityCount; subentityIndex++)
	{
		Ogre::SubEntity* subentity = ent->getSubEntity((unsigned int)subentityIndex);
		subentity->setMaterialName(subEntities[subentityIndex]);
	}

	// add the entitie to the static geometry
	sgeo->addEntity(ent, position, rotation, scale);
	Common::GlobalObjects::sceneManager->destroyEntity(ent);


	return true;
}

////////////////////////////////////////////////////////////////////////////////
bool LevelGeometryLoader::processStaticGeometries(TiXmlElement *XMLRoot)
{
	ASSERT(XMLRoot);

	TiXmlElement *pElement, *auxPElem;

	// Process nodes (?)
	pElement = XMLRoot->FirstChildElement("staticGeometries");
	if(!pElement){
		debug("Error: Invalid .scene processStaticGeometries\n" );
		return false;
	}

	mStaticGeometry.clear();

	Ogre::StaticGeometry	*sgeo = 0;
	Ogre::Vector3			origin;
	Ogre::Vector3			dimension;
	bool					visible;
	Ogre::String			name;
	bool					castShadow;

	// parse the staticGeometries
	pElement = pElement->FirstChildElement("staticGeometry");
	while(pElement)
	{
		// Parse a StaticGeometry
		visible = Ogre::DotSceneLoader::getAttribBool(pElement, "visible");
		name = Ogre::DotSceneLoader::getAttrib(pElement, "name");
		castShadow = Ogre::DotSceneLoader::getAttribBool(pElement, "castShadows");

		try {
			sgeo = Common::GlobalObjects::sceneManager->createStaticGeometry(name);
			debug("StaticGeometry %s created\n", name.c_str());
		} catch(...) {
			debug("Error: Invalid name to StaticGeometry\n" );
			return false;
		}

		mStaticGeometry.push_back(sgeo);

		sgeo->setCastShadows(castShadow);
		sgeo->setVisible(visible);

		// get the origin
		auxPElem = pElement->FirstChildElement("origin");
		if(auxPElem) {
			sgeo->setOrigin(Ogre::DotSceneLoader::parseVector3(auxPElem));
		}

		// get the dimensions
		auxPElem = pElement->FirstChildElement("dimensions");
		if(auxPElem) {
			sgeo->setRegionDimensions(Ogre::DotSceneLoader::parseVector3(auxPElem));
		}

		// iterate over the entity list
		auxPElem = pElement;
		auxPElem = auxPElem->FirstChildElement("entities");
		if(!auxPElem){
			debug("Error: StaticGeometry without Entity! %s\n", sgeo->getName().c_str());
			ASSERT(false);
			mStaticGeometry.erase(mStaticGeometry.end()-1);
			Common::GlobalObjects::sceneManager->destroyStaticGeometry(sgeo);
			pElement = pElement->NextSiblingElement("staticGeometry");
			continue;
		}
		auxPElem = auxPElem->FirstChildElement("entity");
		while(auxPElem){
			// get the entities associated
			if(!processEntityStaticGeoemtry(auxPElem, sgeo)){
				debug("Error: Invalid entity of StaticGeometry\n" );
				return false;
			}

			auxPElem = auxPElem->NextSiblingElement("entity");
		}

		// build the staticGeometry
		sgeo->build();

		pElement = pElement->NextSiblingElement("staticGeometry");
	}


	return true;

}

////////////////////////////////////////////////////////////////////////////////
Ogre::Entity *LevelGeometryLoader::processEntity(TiXmlElement *XMLNode)
{
	ASSERT(XMLNode);

	// Process attributes
	Ogre::String meshFile = Ogre::DotSceneLoader::getAttrib(XMLNode, "meshFile");
	Ogre::Vector3 position = Ogre::Vector3::ZERO;
	Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;
	Ogre::Vector3 scale = Ogre::Vector3::UNIT_SCALE;
	std::vector<Ogre::String> subEntities;


	// parse the entity

	// get the origin
	TiXmlElement *auxPElem = XMLNode->FirstChildElement("position");
	if(auxPElem) {
		position = Ogre::DotSceneLoader::parseVector3(auxPElem);
	}

	// get the rotation
	auxPElem = XMLNode->FirstChildElement("rotation");
	if(auxPElem) {
		rotation = Ogre::DotSceneLoader::parseQuaternion(auxPElem);
	}

	// get the scale
	auxPElem = XMLNode->FirstChildElement("scale");
	if(auxPElem) {
		scale = Ogre::DotSceneLoader::parseVector3(auxPElem);
	}

	size_t count = 0;
	// load the subentities
	TiXmlElement *pElement = XMLNode->FirstChildElement("subentities");
	if(pElement){
		auxPElem = pElement->FirstChildElement("subentity");
		// ugly! very
		while(auxPElem) {
			auxPElem = auxPElem->NextSiblingElement("subentity");
			count++;
		}
		subEntities.resize(count);
		pElement = pElement->FirstChildElement("subentity");
		while(pElement){
			int index = Ogre::DotSceneLoader::getAttribInt(pElement, "index");

			subEntities[index] = Ogre::DotSceneLoader::getAttrib(pElement, "materialName");

			pElement = pElement->NextSiblingElement("subentity");
		}
	}

	// creates the entity
	Ogre::Entity *ent = 0;
	try {
		ent = Common::GlobalObjects::sceneManager->createEntity("entity_" + meshFile, meshFile);
	} catch (...) {
		return 0;
	}

	// set the subentities config
	size_t subentityCount = std::min(subEntities.size(), (size_t)ent->getNumSubEntities());
	for (size_t subentityIndex = 0; subentityIndex < subentityCount; subentityIndex++)
	{
		Ogre::SubEntity* subentity = ent->getSubEntity((unsigned int)subentityIndex);
		subentity->setMaterialName(subEntities[subentityIndex]);
	}

	return ent;
}

////////////////////////////////////////////////////////////////////////////////
bool LevelGeometryLoader::processNode(TiXmlElement *XMLNode, Ogre::SceneNode *&pNode)
{
	ASSERT(XMLNode);
	ASSERT(pNode == 0);

	pNode = 0;

	// create the node
	pNode = GLOBAL_SCN_MNGR->getRootSceneNode()->createChildSceneNode();

	// Process other attributes
	TiXmlElement *pElement;

	// Process position (?)
	pElement = XMLNode->FirstChildElement("position");
	if(pElement)
	{
	  pNode->setPosition(Ogre::DotSceneLoader::parseVector3(pElement));
	  pNode->setInitialState();
	}

	// Process rotation (?)
	pElement = XMLNode->FirstChildElement("rotation");
	if(pElement)
	{
	  pNode->setOrientation(Ogre::DotSceneLoader::parseQuaternion(pElement));
	  pNode->setInitialState();
	}

	// Process scale (?)
	pElement = XMLNode->FirstChildElement("scale");
	if(pElement)
	{
	  pNode->setScale(Ogre::DotSceneLoader::parseVector3(pElement));
	  pNode->setInitialState();
	}

#ifdef DEBUG
	// no animations for this
	pElement = XMLNode->FirstChildElement("animations");
	if(pElement)
	{
		debugERROR("Animation en geometry... algo ta mal...\n");
		ASSERT(false);
	}

	// Process lookTarget (?)
	pElement = XMLNode->FirstChildElement("lookTarget");
	if(pElement){
		debugERROR("lookTarget en geometry... algo ta mal...\n");
		ASSERT(false);
	}

	// Process trackTarget (?)
	pElement = XMLNode->FirstChildElement("trackTarget");
	if(pElement){
		debugERROR("trackTarget en geometry... algo ta mal...\n");
		ASSERT(false);
	}

#endif
	// Process node (*)
	pElement = XMLNode->FirstChildElement("node");
	while(pElement)
	{
		debugERROR("Estamos teniendo Nested Nodes, por ahora no queremos eso "
				"xD\n");
		ASSERT(false);
//	  processNode(pElement, pNode);
//	  pElement = pElement->NextSiblingElement("node");
	}

	// Process entity (*)
	pElement = XMLNode->FirstChildElement("entity");
	while(pElement)
	{
	  Ogre::Entity *ent = processEntity(pElement);
	  ASSERT(ent);
	  pNode->attachObject(ent);
	  pElement = pElement->NextSiblingElement("entity");

	}

#ifdef DEBUG

	// Process light (*)
	pElement = XMLNode->FirstChildElement("light");
	ASSERT(!pElement);

	// Process camera (*)
	pElement = XMLNode->FirstChildElement("camera");
	ASSERT(!pElement);

	// Process particleSystem (*)
	pElement = XMLNode->FirstChildElement("particleSystem");
	ASSERT(!pElement);

	// Process billboardSet (*)
	pElement = XMLNode->FirstChildElement("billboardSet");
	ASSERT(!pElement);

	// Process plane (*)
	pElement = XMLNode->FirstChildElement("plane");
	ASSERT(!pElement);

	// Process userDataReference (?)
	pElement = XMLNode->FirstChildElement("userDataReference");
	ASSERT(!pElement);

#endif

	return true;
}

////////////////////////////////////////////////////////////////////////////////
bool LevelGeometryLoader::processNodes(TiXmlElement *XMLNode)
{
	ASSERT(XMLNode);

    TiXmlElement *pElement;

    XMLNode = XMLNode->FirstChildElement("nodes");
    if(!XMLNode){
    	debugWARNING("No entities found in the scene\n");
    	return false;
    }

    // Process node (*)
    pElement = XMLNode->FirstChildElement("node");
    while(pElement)
    {
    	Ogre::SceneNode *node = 0;
        if(!processNode(pElement, node)){
        	debugERROR("Error processing node\n");
        	ASSERT(false);
        	return false;
        }
        pElement = pElement->NextSiblingElement("node");
        ASSERT(node);
        ASSERT(node->getAttachedObject(0)); // we have something attached
        mEntities.push_back(node);
    }

    return true;
}


////////////////////////////////////////////////////////////////////////////////
LevelGeometryLoader::LevelGeometryLoader() :
		Loader("LevelGeometryLoader")
{
	debugOPTIMIZATION("Probablemente este de mas que \"Trackiemos\" las "
			"staticGeometry cuando en realidad el SceneManager las tiene todas\n");
}


////////////////////////////////////////////////////////////////////////////////
LevelGeometryLoader::~LevelGeometryLoader()
{
	unload();
}


// Functoin used to load something
int LevelGeometryLoader::load(TiXmlElement *elem, LoaderData *data)
{
	ASSERT(elem);

	// clear all the elements load before
	unload();

	mStaticGeometry.clear();

	debugGREEN("Loading LevelGeometry\n");
	if( Ogre::String( elem->Value()) != "scene"  ) {
		debugERROR("Error: Invalid .scene File. Missing <scene>" );
		ASSERT(false);
		return -1;
	}

	debugGREEN("Parsing StaticGeometries\n");
	// first parse the static geometries
	if(!processStaticGeometries(elem)){
		debugERROR("Error Processing static Geometries\n");
		ASSERT(false);
		return -1;
	}

	debugGREEN("Parsing the entities now\n");
	if(!processNodes(elem)){
		debugERROR("Error parsing entities\n");
		ASSERT(false);
		return -1;
	}

	debugGREEN("All geometry level loaded\n");
	return 0;
}

// Unload the information?
int LevelGeometryLoader::unload()
{
	debugRED("TODO: ?\n");
	// first of all we will destroy all the static geometries
	for(int i = 0; i < mStaticGeometry.size(); ++i){
		GLOBAL_SCN_MNGR->destroyStaticGeometry(mStaticGeometry[i]);
	}
	// TODO: podemos usar GLOBAL_SCN_MNGR->destroyAllStaticGeometry(void);
	mStaticGeometry.clear();

	// destroy all the entities
	debugWARNING("Solo estamos borrando los scene nodes y entities del scene "
			"manager pero probablemente no los meshes... Habria que hacer un "
			"destructor general\n");
	for(int i = 0; i < mEntities.size(); ++i){
		Ogre::MovableObject *ent = mEntities[i]->getAttachedObject(0);
		// TODO: deberiamos obtener todos los "attachedobjects" usando el iterator
		GLOBAL_SCN_MNGR->destroyMovableObject(ent);
		GLOBAL_SCN_MNGR->getRootSceneNode()->removeAndDestroyChild(
				mEntities[i]->getName());
	}
	mEntities.clear();

	return 0;
}
