/*
 * InputManager.cpp
 *
 *  Created on: 19/03/2012
 *      Author: agustin
 */


#include "InputManager.h"

#include "CameraController.h"
#include "LevelManager.h"
#include "MenuManager.h"
#include "InputActionObject.h"
#include "IInputState.h"
#include "GameUnit.h"

////////////////////////////////////////////////////////////////////////////////
void InputManager::configureDefaultKeys(void)
{
	// Keyboard
	mKeys[KEY_GROUP_UNITS] 		= OIS::KC_LCONTROL;
	mKeys[KEY_MOVE_CAM_UP] 		= OIS::KC_W;
	mKeys[KEY_MOVE_CAM_DOWN] 	= OIS::KC_S;
	mKeys[KEY_MOVE_CAM_LEFT] 	= OIS::KC_A;
	mKeys[KEY_MOVE_CAM_RIGHT]	= OIS::KC_D;
	mKeys[KEY_MOVE_CAM_FREE]	= OIS::KC_LCONTROL;
	mKeys[KEY_ROTATE_CAM_X_POS]	= OIS::KC_H;
	mKeys[KEY_ROTATE_CAM_X_NEG]	= OIS::KC_K;
	mKeys[KEY_ROTATE_CAM_Y_POS]	= OIS::KC_U;
	mKeys[KEY_ROTATE_CAM_Y_NEG]	= OIS::KC_J;
	mKeys[KEY_EXIT_GAME]		= OIS::KC_ESCAPE;
	mKeys[KEY_PAUSE_GAME]		= OIS::KC_PAUSE;
	mKeys[KEY_OPEN_CELLPHONE]	= OIS::KC_ADD;

	// Mouse
	mKeys[MOUSE_BUTTON_LEFT]	= OIS::MB_Left;
	mKeys[MOUSE_BUTTON_RIGHT]	= OIS::MB_Right;
	mKeys[MOUSE_BUTTON_MIDDLE]	= OIS::MB_Middle;
}

////////////////////////////////////////////////////////////////////////////////
void InputManager::handleKeyboard(void);

////////////////////////////////////////////////////////////////////////////////
void InputManager::handleMouse(void);

////////////////////////////////////////////////////////////////////////////////
void InputManager::handleRaycast(void);


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
InputManager::InputManager() :
		mLevelManager(0),
		mCameraController(0),
		mMouseCursor(0),
		mMenuManager(0),
		mActualActionObj(0)
{
	configureDefaultKeys();
}

////////////////////////////////////////////////////////////////////////////////
InputManager::~InputManager()
{
	// TODO Auto-generated destructor stub
}

////////////////////////////////////////////////////////////////////////////////
void InputManager::setLevelManager(LevelManager *lm)
{
	ASSERT(lm);
	mLevelManager(lm);
}

////////////////////////////////////////////////////////////////////////////////
void InputManager::setCameraController(CameraController *cc)
{
	ASSERT(cc);
	CameraController = cc;
}


////////////////////////////////////////////////////////////////////////////////
void InputManager::setMouseCursor(MouseCursor *mc)
{
	ASSERT(mc);
	mMouseCursor = mc;
}

////////////////////////////////////////////////////////////////////////////////
void InputManager::setMenuManager(MenuManager *mm)
{
	ASSERT(mm);
	mMenuManager = mm;
}

////////////////////////////////////////////////////////////////////////////////
void InputManager::addLevelZone(const sm::AABB &bb)
{
	ASSERT(bb.tl.x >= 0 && bb.tl.x <= 1);
	ASSERT(bb.tl.y >= 0 && bb.tl.y <= 1);
	ASSERT(bb.br.x >= 0 && bb.br.x <= 1);
	ASSERT(bb.br.y >= 0 && bb.br.y <= 1);

	mLevelZones.push_back(bb);
}

/**
 * Handle a InputActionObject. This way we have to check if we can handle
 * or not the action and if we can, then we just activate the
 * "HandleActionState" to handle it.
 * @param	iao		The input action object to be handled
 * @returns	true	if can handle it,
 * 			false	otherwise
 */
bool InputManager::handleActionObject(InputActionObject *iao)
{
	ASSERT(false); // TODO
}


//				GameUnits handling
/**
 * Select new Unit. Here we will check if we are pressing KEY_GROUP_UNITS
 * to determine if we have to "add" a new unit selected or if we are just
 * selecting one and only one
 * @param	unit	The game unit that we are selecting
 * @note	This function must be called EVERYTIME a unit is selected.
 */
void InputManager::unitSelected(GameUnit *unit)
{
	ASSERT(false); // TODO
}
void InputManager::unitUnselected(GameUnit *unit)
{
	ASSERT(false); // TODO
}
void InputManager::unselectAll(void)
{
	ASSERT(false); // TODO
}

/**
 * TODO: set menu system? aca podemos meter el menu system asociado
 */

////////////////////////////////////////////////////////////////////////////////
void InputManager::update(void)
{

}
