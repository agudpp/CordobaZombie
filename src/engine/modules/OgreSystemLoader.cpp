/*
 * OgreSystemLoader.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#include "OgreSystemLoader.h"

#include <debug/DebugUtil.h>

namespace engine {

////////////////////////////////////////////////////////////////////////////////
Ogre::SceneType
OgreSystemLoader::sceneTypeFromStr(const std::string& strType) const
{
    Ogre::SceneType ret = Ogre::SceneType::ST_GENERIC;
    if (strType == "ST_GENERIC") {
        // do nothing
    } else if (strType == "ST_EXTERIOR_CLOSE") {
        ret = Ogre::SceneType::ST_EXTERIOR_CLOSE;
    } else if (strType == "ST_EXTERIOR_FAR") {
        ret = Ogre::SceneType::ST_EXTERIOR_FAR;
    } else if (strType == "ST_EXTERIOR_REAL_FAR") {
        ret = Ogre::SceneType::ST_EXTERIOR_REAL_FAR;
    } else if (strType == "ST_INTERIOR") {
        ret = Ogre::SceneType::ST_INTERIOR;
    } else {
        debugWARNING("Scene Manager type not recognized: %s\n", strType.c_str());
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
OgreSystemLoader::OgreSystemLoader(Ogre::RenderWindow*& renderWin,
                                   Ogre::SceneManager*& sceneManager,
                                   Ogre::Camera*& camera,
                                   Ogre::Root*& root) :
    IModuleLoader("OgreSystemLoader")
,   mRenderWin(renderWin)
,   mSceneManager(sceneManager)
,   mCamera(camera)
,   mRoot(root)
{
}
////////////////////////////////////////////////////////////////////////////////
OgreSystemLoader::~OgreSystemLoader()
{
}

////////////////////////////////////////////////////////////////////////////////
bool
OgreSystemLoader::load(const EngineConfiguration& config)
{
    // check if the system was already loaded
    if (mRoot != 0) {
        debugWARNING("System already loaded\n");
        return true;
    }

    // get the information needed to configure ogre.
    //<OgreSystemLoader resources="resources.cfg" plugins="plugins.cfg" ogre="ogre.cfg" />
    //

    std::string globalResources;
    std::string plugins;
    std::string ogreConfig;
    std::string ogreLogLevel;
    std::string sceneManagerType;

    if (!config.getValue("OgreSystemLoader", "resources", globalResources) ||
        !config.getValue("OgreSystemLoader", "plugins", plugins) ||
        !config.getValue("OgreSystemLoader", "ogre", ogreConfig) ||
        !config.getValue("OgreSystemLoader", "ogreLogLevel", ogreLogLevel) ||
        !config.getValue("OgreSystemLoader", "sceneManagerType", sceneManagerType)) {
        debugERROR("Error getting the information needed to build ogre system\n");
        return false;
    }

    if (Ogre::Root::getSingletonPtr()){
        mRoot = Ogre::Root::getSingletonPtr();
    } else {
        mRoot = new Ogre::Root(plugins, ogreConfig);
    }

    if (!mRoot) {
        debugERROR("Error creating Ogre::Root\n");
        return false;
    }

    // set the debug level info
    if (ogreLogLevel == "High") {
        Ogre::LogManager::getSingleton().setLogDetail(Ogre::LoggingLevel::LL_NORMAL);
    } else if (ogreLogLevel == "Low") {
        Ogre::LogManager::getSingleton().setLogDetail(Ogre::LoggingLevel::LL_LOW);
    } else {
        debugWARNING("Ogre log leve is not recognized %s\n", ogreLogLevel.c_str());
    }

    // we will try to load everything from the .cfg file directly
    if (!ogreConfig.empty()) {
        if (!mRoot->restoreConfig()) {
            debugERROR("Error restoring the configuration file, probably not "
                "ogre.cfg is in the binary folder\n");
            return false;
        }
    } else {
        debugERROR("This should never happen!\n REMOVE THIS FOR DEBUG ONLY!\n");
        if (!mRoot->showConfigDialog()) {
            debugERROR("Error when configuring the windows with the ConfigDialog\n");
            return false;
        }
    }


    // if we are here, then we can safely init the window
    mRenderWin = mRoot->initialise(true, "CordobaZombie");

    // creates a generic sceneManager
    mSceneManager = mRoot->createSceneManager(sceneTypeFromStr(sceneManagerType));
    if (!mSceneManager) {
        debugERROR("Could not construct the SceneManager\n");
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////
    // creates the sceneNode of the camera
    mCamera = mSceneManager->createCamera("Camera");
    mCamera->setQueryFlags(0);
    mCamera->setVisibilityFlags(0);
    mSceneManager->createSceneNode()->attachObject(mCamera);


    ////////////////////////////////////////////////////////////////////////////
    // creates a default viewport
    Ogre::Viewport* vp = mRenderWin->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
    vp->setAutoUpdated(true);


    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));


    ////////////////////////////////////////////////////////////////////////////
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
OgreSystemLoader::unload(void)
{
    // TODO:
    debugERROR("DO NOTHING FOR NOW\n");

    return true;
}

} /* namespace engine */
