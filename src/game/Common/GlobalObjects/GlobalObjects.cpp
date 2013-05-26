/*
 * GlobalObjects.cpp
 *
 *  Created on: 01/05/2011
 *      Author: agustin
 */

#include "GlobalObjects.h"

namespace Common {

Ogre::SceneManager*     GlobalObjects::sceneManager = 0;
Ogre::Real              GlobalObjects::lastTimeFrame = 0;
Ogre::Camera*           GlobalObjects::camera = 0;
OIS::Keyboard*          GlobalObjects::keyboard = 0;
OIS::Mouse*             GlobalObjects::mouse = 0;
Ogre::Root*             GlobalObjects::ogreRoot = 0;
Ogre::RenderWindow*     GlobalObjects::window = 0;
LevelManager*           GlobalObjects::levelMngr = 0;
f_e::MouseCursor*       GlobalObjects::mouseCursor = 0;
MenuManager*            GlobalObjects::menuManager = 0;

Ogre::SceneManager*
GlobalObjects::getSceneManager(void)
{
    ASSERT(GlobalObjects::sceneManager);
    return GlobalObjects::sceneManager;
}

}
