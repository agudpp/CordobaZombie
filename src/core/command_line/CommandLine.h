/*
 * CommandLine.h
 *
 *  Created on: Nov 10, 2013
 *      Author: agustin
 */

#ifndef COMMANDLINE_H_
#define COMMANDLINE_H_

#include <vector>
#include <string>

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

#include <debug/DebugUtil.h>



// Some defines
#define PLUGINS_CONF_FILE       "plugins.cfg"
#define RESOURCES_PATH          "CZ01_RC_PATH"


namespace core {

class CommandLine
{
public:
    // @brief Some useful arguments to load some stuff if needed
    // @param loadDropboxRsc        Load the dropbox resources
    // @param
    CommandLine(bool loadDropboxRsc = true,
                const std::string& pluginsFile = PLUGINS_CONF_FILE);
    virtual
    ~CommandLine();

    // @brief This method will be called with all the arguments read in the
    //        command line.
    // @param args      The argument list
    // @return the code ID of the tool (0 on success | errorCode otherwise)
    //
    virtual int
    execute(const std::vector<std::string>& args) = 0;

    // @brief Method used to load resources files
    // @param fname     The file name of the .cfg
    // @param path      The path where the file .cfg is (to be used to load all
    //                  the internal directories also
    // @return true on success | false otherwise
    bool
    loadAditionalResourcesFromFile(const Ogre::String &fname,
                                   const Ogre::String &path = "");

private:
    // @brief Load all the ogre stuff
    //
    bool
    loadOgreCore(const std::string& pluginFile);


protected:
    // The ogre instances information ready to be used by the child classes
    //
    Ogre::Root *mRoot;
    Ogre::SceneManager* mSceneMgr;
    Ogre::RenderWindow* mWindow;

    // camera
    Ogre::Camera *mCamera;
};

} /* namespace core */
#endif /* COMMANDLINE_H_ */
