/*
 * CommandLine.cpp
 *
 *  Created on: Nov 10, 2013
 *      Author: agustin
 */

#include "CommandLine.h"


#include <OgreWindowEventUtilities.h>
#include <OgreFontManager.h>
#include <OgreConfigFile.h>

#include <os_utils/OSHelper.h>

namespace core {


////////////////////////////////////////////////////////////////////////////////
bool
CommandLine::loadOgreCore(const std::string& pluginFile)
{
    if (Ogre::Root::getSingletonPtr()){
        mRoot = Ogre::Root::getSingletonPtr();
    } else {
        mRoot = new Ogre::Root(pluginFile);
    }

    if (!mRoot) {
        debugERROR("Error creating Ogre::Root\n");
        return false;
    }
    debugGREEN("Created the Ogre::Root\n");
    Ogre::LogManager::getSingleton().setLogDetail(Ogre::LoggingLevel::LL_LOW);

    // we will try to load everything from the .cfg file directly
    if (!mRoot->restoreConfig()) {
        debugERROR("Error restoring the configuration file, probably not "
            "ogre.cfg is in the binary folder\n");
        return false;
    }

    debugGREEN("Set the configuration for the root\n");

    // if we are here, then we can safely init the window
    mWindow = mRoot->initialise(true);
//    ASSERT(mWindow != 0);
//    mWindow->setHidden(true);
//    mWindow->setVisible(false);

    debugGREEN("Initialized the window\n");

    // creates a generic sceneManager
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
    if (!mSceneMgr) {
        debugERROR("Could not construct the SceneManager\n");
        return false;

    }
    debugGREEN("Created the SceneManager\n");

    ////////////////////////////////////////////////////////////////////////////
    // creates the sceneNode of the camera
    OGRELOG("Creating Camera and CameraHandler");
    mCamera = mSceneMgr->createCamera("Camera");
    mCamera->setQueryFlags(0);
    mCamera->setVisibilityFlags(0);
    mSceneMgr->createSceneNode()->attachObject(mCamera);

    debugGREEN("Created the main camera\n");

    ////////////////////////////////////////////////////////////////////////////
    // creates a default viewport
    OGRELOG("Creating the viewport");
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
    vp->setAutoUpdated(true);

    debugGREEN("Created viewport\n");

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

    ////////////////////////////////////////////////////////////////////////////
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    debugGREEN("Created all the ogre stuff\n");

    return true;
}

CommandLine::CommandLine(bool loadDropboxRsc,
                         const std::string& pluginsFile) :
    mRoot(0)
,   mSceneMgr(0)
,   mWindow(0)
,   mCamera(0)
{
    if (!loadOgreCore(pluginsFile)) {
        debugERROR("Error loading plugin file %s\n", pluginsFile.c_str());
        ASSERT(false);
    }

    if (loadDropboxRsc) {
        // get the dropbox path
        const char* strPath = std::getenv(RESOURCES_PATH);
        if (strPath == 0) {
            debugERROR("Error trying to get the environment variable %s\n",
                RESOURCES_PATH);
        } else {
            // load everything
            // everything is fine
            std::string path = strPath;

            // append the resources.cfg

            // add the last path var
            OSHelper::addEndPathVar(path);

            const std::string fname = path + "resources.cfg";
            debugGREEN("Trying to parse the resource file %s\n", fname.c_str());
            if (!loadAditionalResourcesFromFile(fname, path)) {
                debugERROR("Error loading resource file!\n");
            }
        }
    }
}

CommandLine::~CommandLine()
{
//    // destroy everything
//    if (mSceneMgr) {
//        mSceneMgr->destroyAllCameras();
//        mRoot->destroySceneManager(mSceneMgr);
//        mSceneMgr = 0;
//    }
//    delete mWindow;
//    delete mRoot;
    mRoot->shutdown();
}

bool
CommandLine::loadAditionalResourcesFromFile(const Ogre::String &fname,
                                            const Ogre::String &p)
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

   core::OSHelper::addEndPathVar(path);

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements()) {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i) {
            typeName = i->first;
            archName = path + i->second;
            //debugGREEN("DEBUG: %s -- %s -- %s\n",archName, typeName, secName);
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    return true;
}

} /* namespace core */
