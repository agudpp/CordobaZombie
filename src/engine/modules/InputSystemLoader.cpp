/*
 * InputSystemLoader.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: agustin
 */

#include "InputSystemLoader.h"

#include <OIS/OISInputManager.h>
#include <OIS/OISMouse.h>
#include <OIS/OISKeyboard.h>

#include <debug/DebugUtil.h>
#include <types/basics.h>

namespace engine {


////////////////////////////////////////////////////////////////////////////////
InputSystemLoader::InputSystemLoader(Ogre::RenderWindow*& renderWindow,
                                     OIS::Mouse*& mouse,
                                     OIS::Keyboard*& keyboard,
                                     OIS::InputManager*& manager) :
    IModuleLoader("InputSystemLoader")
,   mRenderWindow(renderWindow)
,   mMouse(mouse)
,   mKeyboard(keyboard)
,   mManager(manager)
{
}

////////////////////////////////////////////////////////////////////////////////
InputSystemLoader::~InputSystemLoader()
{
}

////////////////////////////////////////////////////////////////////////////////
bool
InputSystemLoader::load(const EngineConfiguration& config)
{
    if (!mRenderWindow) {
        debugERROR("We need the render windows to be able to work here. Probably "
            "this is happening because you are not constructing the render window"
            " in a previous loader?\n");
        return false;
    }
    // read the parameters here
    int disableInputGrabbing = 1;

    if (!config.getValue("InputSystemLoader",
                         "disableInputGrabbing",
                         disableInputGrabbing)) {
        debugERROR("Error getting the information needed to build input system\n");
        return false;
    }


    core::size_t hWnd = 0;

    // override OIS construction to avoid grabbing mouse
    OIS::ParamList pl;
    core::size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mRenderWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));


    if (disableInputGrabbing) {
#if defined OIS_WIN32_PLATFORM
        pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
        pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_EXCLUSIVE")));
        pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
        pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#elif defined OIS_LINUX_PLATFORM
        pl.insert(
            std::make_pair(std::string("x11_mouse_grab"),
                std::string("false")));
        pl.insert(
            std::make_pair(std::string("x11_mouse_hide"),
                std::string("true")));
        pl.insert(
            std::make_pair(std::string("x11_keyboard_grab"),
                std::string("false")));
        pl.insert(
            std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif
        // end override OIS construction to avoid grabbing mouse
    }

    mRenderWindow->getCustomAttribute("WINDOW", &hWnd);
    //mInputManager = OIS::InputManager::createInputSystem(hWnd);
    mManager = OIS::InputManager::createInputSystem(pl);

    // creates the mouse and keyboard objects (we will use unbuffered input
    // system, the second parameter set this)
    mKeyboard = static_cast<OIS::Keyboard*>(mManager->createInputObject(
        OIS::OISKeyboard, false));

    mMouse = static_cast<OIS::Mouse*>(mManager->createInputObject(
        OIS::OISMouse, false));

    ASSERT(mMouse && mKeyboard);

    // config the mouse to the screen size
    debugWARNING("WE ARE NOT SUPPORTING RESIZEABLES WINDOWS!\n");
    unsigned int width, height, depth;
    int left, top;
    mRenderWindow->getMetrics(width, height, depth, left, top);
    const OIS::MouseState &mouseState = mMouse->getMouseState();
    mouseState.width = width;
    mouseState.height = height;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
InputSystemLoader::unload(void)
{
    if (mManager == 0) {
        debugWARNING("Unloading a system that was never built?\n");
        return true;
    }

    mManager->destroyInputObject(mMouse);
    mManager->destroyInputObject(mKeyboard);
    mMouse = 0;
    mKeyboard = 0;
    OIS::InputManager::destroyInputSystem(mManager);
    mManager = 0;


    return true;
}

} /* namespace engine */
