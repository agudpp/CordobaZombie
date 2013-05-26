/* Here we will put all the GlobalShared data like SceneManager, and others.
 * GlobalObjects.h
 *
 *  Created on: 01/05/2011
 *      Author: agustin
 */

#ifndef GLOBALOBJECTS_H_
#define GLOBALOBJECTS_H_

#include <OgreSceneManager.h>
#include <OgreRoot.h>
#include <OgreCamera.h>
#include <OIS.h>
#include <OgreRenderWindow.h>

#include "DebugUtil.h"

#define GLOBAL_SCN_MNGR				Common::GlobalObjects::sceneManager
#define GLOBAL_CAMERA				Common::GlobalObjects::camera
#define GLOBAL_TIME_FRAME			Common::GlobalObjects::lastTimeFrame
#define GLOBAL_KEYBOARD				Common::GlobalObjects::keyboard
#define GLOBAL_MOUSE				Common::GlobalObjects::mouse
#define GLOBAL_ROOT					Common::GlobalObjects::ogreRoot
#define GLOBAL_WINDOW				Common::GlobalObjects::window
#define GLOBAL_LVL_MNGR				Common::GlobalObjects::levelMngr
#define GLOBAL_CURSOR				Common::GlobalObjects::mouseCursor
#define GLOBAL_MENU_MNGR			Common::GlobalObjects::menuManager

// Forward
//
namespace f_e {
class MouseCursor;
}

class LevelManager;
class MenuManager;


namespace Common {

class GlobalObjects {
public:
	/* The scene manager that we are using */
	static Ogre::SceneManager   *sceneManager;

	// The OgreRoot
	static Ogre::Root           *ogreRoot;

	// Render Window
	static Ogre::RenderWindow   *window;

	/* The last timeFrame (this should be actualized every frame) */
	static Ogre::Real           lastTimeFrame;
	static Ogre::Camera         *camera;

	static OIS::Keyboard        *keyboard;
	static OIS::Mouse           *mouse;

	/* The LevelManager used actually */
	static LevelManager         *levelMngr;

	// Globarl Mouse cursor
	static f_e::MouseCursor     *mouseCursor;

	static MenuManager          *menuManager;


public:
	/* Get the SceneManager */
	static Ogre::SceneManager *getSceneManager(void);






private:
	GlobalObjects();
	GlobalObjects(GlobalObjects &other);
	~GlobalObjects();
};

}

#endif /* GLOBALOBJECTS_H_ */
