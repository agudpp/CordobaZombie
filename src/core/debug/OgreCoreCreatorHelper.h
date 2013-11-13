/*
 * OgreCoreCreatorHelper.h
 *
 *  Created on: Nov 13, 2013
 *      Author: agustin
 */

#ifndef OGRECORECREATORHELPER_H_
#define OGRECORECREATORHELPER_H_


#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreMeshManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <OgreTimer.h>
#include <OgreSceneQuery.h>
#include <OgreColourValue.h>

#include "DebugUtil.h"

namespace core {

// @brief This class will be used to create an ogre instance and load all the
//        basic stuff
//

struct OgreCoreCreatorHelper
{
    // The variables to be used if you need
    Ogre::Root *mRoot;
    Ogre::SceneManager* mSceneMgr;
    Ogre::RenderWindow* mWindow;
    Ogre::Camera *mCamera;


    OgreCoreCreatorHelper();
    ~OgreCoreCreatorHelper();

    // @brief Set how much information we want to print in the console from
    //        ogre engine
    // @param high      High volume of information
    //
    void
    setLogLevel(bool high);

    // @brief Build everything ogre core system.
    // @param resources         The resource .cfg file to use (or empty)
    // @param plugins           The plugins.cfg file to use
    // @param config            The ogre configuration file for the window (or
    //                          empty to use the last one)
    // @return true on success | false otherwise
    //
    bool
    build(const Ogre::String& resources = "",
          const Ogre::String& plugins = "plugins.cfg",
          const Ogre::String& config = "");

    // @brief Load a resource file
    // @param rf            The resource file
    // @param prependPath   The prepend path to be used if the file contains
    //                      relative paths instead of absolutes
    // @return true on success | false otherwise
    //
    bool
    loadConfigFile(const Ogre::String& rf, const Ogre::String& prependPath = "");

    // @brief Helper method to load the game resource file using an environment
    //        variable name (the environment varaible that points to the resource
    //        directory).
    // @param envVarName        The environment variable name
    // @param rscFileName       The resource filename ("resources.cfg")
    // @return true if it was loaded | false otherwise
    //
    bool
    loadConfigFromEnvVar(const char* envVarName,
                         const char* rscFileName = "resources.cfg");

};

} /* namespace core */
#endif /* OGRECORECREATORHELPER_H_ */
