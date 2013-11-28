/*
 * InputSystemLoader.h
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#ifndef INPUTSYSTEMLOADER_H_
#define INPUTSYSTEMLOADER_H_


#include <OgreRenderWindow.h>
#include <OIS/OISMouse.h>
#include <OIS/OISKeyboard.h>

#include "IModuleLoader.h"

namespace engine {

class InputSystemLoader : public IModuleLoader
{
public:
    // We need the ogre render window to set some specific attributes
    InputSystemLoader(Ogre::RenderWindow*& renderWindow,
                      OIS::Mouse*& mouse,
                      OIS::Keyboard*& keyboard,
                      OIS::InputManager*& manager);
    virtual
    ~InputSystemLoader();


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
    Ogre::RenderWindow*& mRenderWindow;
    OIS::Mouse*& mMouse;
    OIS::Keyboard*& mKeyboard;
    OIS::InputManager*& mManager;
};

} /* namespace engine */
#endif /* INPUTSYSTEMLOADER_H_ */
