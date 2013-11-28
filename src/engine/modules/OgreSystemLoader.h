/*
 * OgreSystemLoader.h
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#ifndef OGRESYSTEMLOADER_H_
#define OGRESYSTEMLOADER_H_

#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>

#include "IModuleLoader.h"


namespace engine {

class OgreSystemLoader : public IModuleLoader
{
public:

    // This class needs the reference of the variables to be used to load
    // the ogre data.
    //
    OgreSystemLoader(Ogre::RenderWindow*& renderWin,
                     Ogre::SceneManager*& sceneManager,
                     Ogre::Camera*& camera,
                     Ogre::Root*& root);
    virtual
    ~OgreSystemLoader();


    ////////////////////////////////////////////////////////////////////////////
    // Inherited methods
    //

    // @brief Load / initialize the module using the current configuration.
    // @param config        The configuration to be used
    // @return true on success | false on error
    //
    virtual bool
    load(const EngineConfiguration& config);

    // @brief Unload the current module or uninitialize the module.
    // @return true on succes | false otherwise.
    //
    virtual bool
    unload(void);

private:
    // @brief Get the scene manager type from a string
    // @param strType       The string type
    // @return the ogre scene manager type
    //
    Ogre::SceneType
    sceneTypeFromStr(const std::string& strType) const;

private:
    Ogre::RenderWindow*& mRenderWin;
    Ogre::SceneManager*& mSceneManager;
    Ogre::Camera*& mCamera;
    Ogre::Root*& mRoot;
};

} /* namespace engine */
#endif /* OGRESYSTEMLOADER_H_ */
