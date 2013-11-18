/*
 * OgreCoreCreatorHelper.cpp
 *
 *  Created on: Nov 13, 2013
 *      Author: agustin
 */

#include "OgreCoreCreatorHelper.h"

#include <os_utils/OSHelper.h>

namespace core {
////////////////////////////////////////////////////////////////////////////////
OgreCoreCreatorHelper::OgreCoreCreatorHelper() :
    mRoot(0)
,   mSceneMgr(0)
,   mWindow(0)
,   mCamera(0)
{
}
////////////////////////////////////////////////////////////////////////////////
OgreCoreCreatorHelper::~OgreCoreCreatorHelper()
{
}

////////////////////////////////////////////////////////////////////////////////
void
OgreCoreCreatorHelper::setLogLevel(bool high)
{
    if (high) {
        Ogre::LogManager::getSingleton().setLogDetail(Ogre::LoggingLevel::LL_NORMAL);
    } else {
        Ogre::LogManager::getSingleton().setLogDetail(Ogre::LoggingLevel::LL_LOW);
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
OgreCoreCreatorHelper::build(const Ogre::String& resources,
                             const Ogre::String& plugins,
                             const Ogre::String& config)
{
    if (Ogre::Root::getSingletonPtr()){
        mRoot = Ogre::Root::getSingletonPtr();
    } else {
        mRoot = new Ogre::Root(plugins);
    }

    if (!mRoot) {
        debugERROR("Error creating Ogre::Root\n");
        return false;
    }

    // we will try to load everything from the .cfg file directly
    if (config.empty()) {
        if (!mRoot->restoreConfig()) {
            debugERROR("Error restoring the configuration file, probably not "
                "ogre.cfg is in the binary folder\n");
            return false;
        }
    } else {
        if (!mRoot->showConfigDialog()) {
            debugERROR("Error when configuring the windows with the ConfigDialog\n");
            return false;
        }
    }


    // if we are here, then we can safely init the window
    mWindow = mRoot->initialise(true);

    // creates a generic sceneManager
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
    if (!mSceneMgr) {
        debugERROR("Could not construct the SceneManager\n");
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////
    // creates the sceneNode of the camera
    mCamera = mSceneMgr->createCamera("Camera");
    mCamera->setQueryFlags(0);
    mCamera->setVisibilityFlags(0);
    mSceneMgr->createSceneNode()->attachObject(mCamera);


    ////////////////////////////////////////////////////////////////////////////
    // creates a default viewport
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
    vp->setAutoUpdated(true);


    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

    // check if we have to load any resource
    if (!resources.empty()) {
        // yes we have to
        if (!loadConfigFile(resources)) {
            debugERROR("Error loading resources file\n");
            return false;
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    return true;
}


////////////////////////////////////////////////////////////////////////////////
bool
OgreCoreCreatorHelper::loadConfigFile(const Ogre::String& fname,
                                      const Ogre::String& p)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    try {
        cf.load(fname);
    } catch (...) {
        debugERROR("ERROR: file: %s not found\n", fname.c_str());
        return false;
    }

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    Ogre::String path = p;

    // add the end path var if needed
    OSHelper::addEndPathVar(path);

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements()) {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i) {
            typeName = i->first;
            archName = path + i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    return true;
}

bool
OgreCoreCreatorHelper::loadConfigFromEnvVar(const char* envVarName,
                                            const char* rscFileName)
{
    if (envVarName == 0 || rscFileName == 0) {
        debugERROR("Invalid params\n");
        return false;
    }

    // get the dropbox path
    char* strPath = 0;
    if (!OSHelper::getEnvVar(envVarName, strPath)) {
        debugERROR("Error trying to get the environment variable %s\n",
            envVarName);
        return false;
    }
    // load everything
    // everything is fine
    std::string path = strPath;

    // add the last path var
    OSHelper::addEndPathVar(path);
    const std::string fname = path + rscFileName;

    return loadConfigFile(fname, path);
}


} /* namespace core */
