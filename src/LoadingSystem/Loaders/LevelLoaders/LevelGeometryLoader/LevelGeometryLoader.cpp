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

#include "LevelGeometryLoader.h"
#include "DotSceneLoader.h"
#include "LoaderManager.h"
#include "DebugUtil.h"
#include "GlobalObjects.h"


////////////////////////////////////////////////////////////////////////////////
void LevelGeometryLoader::setChunkWeight(TiXmlElement* elem)
{
	TiXmlElement* item(0);
	uint itemsToProcess(0u);

	for (item = elem->FirstChildElement("staticGeometries")->
				FirstChildElement("staticGeometry");
		 item != 0;
		 item = item->NextSiblingElement("staticGeometry")) {
		itemsToProcess++;
	}

	for (TiXmlElement* item = elem->FirstChildElement("nodes")->
				FirstChildElement("node");
		 item != 0;
		 item = item->NextSiblingElement("node")) {
		itemsToProcess++;
	}

	// mChunkWeight is a percentage.
	mChunkWeight = 1.0f / itemsToProcess;
}


////////////////////////////////////////////////////////////////////////////////
bool LevelGeometryLoader::processEntityStaticGeometry(TiXmlElement *XMLNode,
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
		ent = Common::GlobalObjects::sceneManager->createEntity(meshFile);
	} catch (...) {
	    debugERROR("Error creating entity with name %s\n", meshFile.c_str());
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
	std::string				msg;
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
			goto nextElement;
		}
		auxPElem = auxPElem->FirstChildElement("entity");
		while(auxPElem){
			// get the entities associated
			if(!processEntityStaticGeometry(auxPElem, sgeo)){
				debug("Error: Invalid entity of StaticGeometry\n" );
				return false;
			}

			auxPElem = auxPElem->NextSiblingElement("entity");
		}

		// build the staticGeometry
		sgeo->build();

		nextElement:
			// Update the loading bar.
			msg = "Static geometry \"" + name + "\" loaded.\n";
			if (mCallback)
				(*(LoaderManager::LoaderCallback*)mCallback)(mChunkWeight, msg);

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
	} else {
	    debugWARNING("No position found for %s\n", meshFile.c_str());
	}

	// get the rotation
	auxPElem = XMLNode->FirstChildElement("rotation");
	if(auxPElem) {
		rotation = Ogre::DotSceneLoader::parseQuaternion(auxPElem);
	} else {
        debugWARNING("No rotation found for %s\n", meshFile.c_str());
    }

	// get the scale
	auxPElem = XMLNode->FirstChildElement("scale");
	if(auxPElem) {
		scale = Ogre::DotSceneLoader::parseVector3(auxPElem);
	} else {
        debugWARNING("No scale found for %s\n", meshFile.c_str());
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
			const int index = Ogre::DotSceneLoader::getAttribInt(pElement,
			                                                     "index");
			ASSERT(index < count);
			subEntities[index] = Ogre::DotSceneLoader::getAttrib(pElement,
			                                                    "materialName");
			pElement = pElement->NextSiblingElement("subentity");
		}
	}

	// creates the entity
	Ogre::Entity *ent = 0;
	try {
		ent = GLOBAL_SCN_MNGR->createEntity(meshFile);
	} catch (...) {
	    debugERROR("Error creating an entity with name %s\n", meshFile.c_str());
		return 0;
	}

	// set the subentities config
	size_t subentityCount = std::min(subEntities.size(),
	                                 static_cast<size_t>(ent->getNumSubEntities()));
	for (size_t subentityIndex = 0; subentityIndex < subentityCount;
	        subentityIndex++) {
		Ogre::SubEntity* subentity = ent->getSubEntity(
		        static_cast<unsigned int>(subentityIndex));
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

	Ogre::SceneNode* node(0);
    TiXmlElement *pElement;
    std::string msg;

    XMLNode = XMLNode->FirstChildElement("nodes");
    if(!XMLNode){
    	debugWARNING("No entities found in the scene\n");
    	return false;
    }

    // Process node (*)
    pElement = XMLNode->FirstChildElement("node");
    while(pElement)
    {
        if(!processNode(pElement, node)){
        	debugERROR("Error processing node\n");
        	ASSERT(false);
        	return false;
        } else {
            ASSERT(node);
            ASSERT(node->getAttachedObject(0)); // we have something attached
            mSceneNodes.push_back(node);
        }

        // Update the loading bar.
        msg = pElement->Attribute("name");
        msg = "Node \"" + msg + "\" loaded.\n";
		if (mCallback)
			(*(LoaderManager::LoaderCallback*)mCallback)(mChunkWeight, msg);

        pElement = pElement->NextSiblingElement("node");
        node = 0;
    }

    return true;
}


////////////////////////////////////////////////////////////////////////////////
LevelGeometryLoader::LevelGeometryLoader() :
		Loader("LevelGeometryLoader")
{
	debugOPTIMIZATION("Probablemente este de mas que \"Trackiemos\" las "
			"staticGeometry cuando en realidad el SceneManager las tiene todas\n");
	// set the weight associated to this Loader...
	setWeight(40);
}


////////////////////////////////////////////////////////////////////////////////
LevelGeometryLoader::~LevelGeometryLoader()
{
	unload();
}


////////////////////////////////////////////////////////////////////////////////
// Resources loading
int LevelGeometryLoader::load(TiXmlElement *elem, LoaderData *data)
{
	uint itemsToProcess(0u);

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

	// Check the number of items to process, in order to customize
	// the loading bar *increase effect*
	setChunkWeight(elem);

	// Load Static Geometries.
	debugGREEN("Parsing StaticGeometries\n");
	if(!processStaticGeometries(elem)){
		debugERROR("Error parsing static Geometries\n");
		ASSERT(false);
		return -1;
	}

	// Load Nodes and Entities.
	debugGREEN("Parsing the entities\n");
	if(!processNodes(elem)){
		debugERROR("Error parsing entities\n");
		ASSERT(false);
		return -1;
	}

	debugGREEN("Level geometry loaded.\n");
	return 0;
}

// Unload the information?
int LevelGeometryLoader::unload()
{
	// first of all we will destroy all the static geometries
	for(int i = 0; i < mStaticGeometry.size(); ++i){
		GLOBAL_SCN_MNGR->destroyStaticGeometry(mStaticGeometry[i]);
	}
	mStaticGeometry.clear();

	// destroy all the entities
	debugColor(DEBUG_BROWN, "Solo estamos borrando los scene nodes y entities "
			"del scene manager, pero probablemente no los meshes.\nHabria que "
			"hacer un destructor general. TODO: hacer un memory test sobre "
			"esto, para ver si realmente estamos liberando todo o no. "
	        "Para la demo NO hace falta.\n");
	for(int i = 0; i < mSceneNodes.size(); ++i){
		Ogre::MovableObject *ent = mSceneNodes[i]->getAttachedObject(0);
		// TODO: deberiamos obtener todos los "attachedobjects" usando el iterator
		GLOBAL_SCN_MNGR->destroyMovableObject(ent);
		GLOBAL_SCN_MNGR->getRootSceneNode()->removeAndDestroyChild(
				mSceneNodes[i]->getName());
	}
	mSceneNodes.clear();

	return 0;
}
