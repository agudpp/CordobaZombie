/*
 * InputManager.h
 *
 *  Created on: 19/03/2012
 *      Author: agustin
 */

#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_


#include "GlobalObjects.h"
#include "MouseCursor.h"
#include "CommonMath.h"
#include "InputStateMachine.h"
#include "DebugUtil.h"

class CameraController;
class LevelManager;
class MenuManager;
class InputActionObject;
class IInputState;
class GameUnit;

class InputManager
{
	// define the bounds where the mouse will start to move the camera in %
	// relative to the window size
	static const float	CAMERA_MOVE_BOUNDS_LIMITS	=	0.01f;
	static const float	CAMERA_MOVE_BOUNDS_L_LIMIT	=	CAMERA_MOVE_BOUNDS_LIMITS;
	static const float	CAMERA_MOVE_BOUNDS_R_LIMIT	=	1.0f-CAMERA_MOVE_BOUNDS_LIMITS;
	static const float	CAMERA_MOVE_BOUNDS_T_LIMIT	=	CAMERA_MOVE_BOUNDS_LIMITS;
	static const float	CAMERA_MOVE_BOUNDS_B_LIMIT	=	1.0f-CAMERA_MOVE_BOUNDS_LIMITS;


	// typedefs
	typedef std::vector<sm::AABB>		LevelZoneVec;


public:
	// Here we let public the possibility to configure the keys used in the game
	enum inputID {
		KEY_GROUP_UNITS = 0,
		KEY_MOVE_CAM_UP,
		KEY_MOVE_CAM_DOWN,
		KEY_MOVE_CAM_LEFT,
		KEY_MOVE_CAM_RIGHT,
		KEY_MOVE_CAM_FREE,		// key used to move the cam with the mouse
		KEY_ROTATE_CAM_X_POS,	// rotate positive the camera throw axis X
		KEY_ROTATE_CAM_X_NEG,	// rotate negative the camera throw axis X
		KEY_ROTATE_CAM_Y_POS,
		KEY_ROTATE_CAM_Y_NEG,
		KEY_EXIT_GAME,
		KEY_PAUSE_GAME,
		KEY_OPEN_CELLPHONE,

		///////////////////////////////////////////////////////////////////////
		MOUSE_BUTTON_LEFT,
		MOUSE_BUTTON_RIGHT,
		MOUSE_BUTTON_MIDDLE,

		///////////////////////////////////////////////////////////////////////
		NUM_KEYS,
	};

	typedef std::vector<GameUnit *>		UnitSelVec;

public:
	InputManager();
	~InputManager();

	/**
	 * Set the levelManager
	 */
	void setLevelManager(LevelManager *lm);

	/**
	 * Set the camera controller
	 */
	void setCameraController(CameraController *cc);
	inline CameraController *getCameraController(void);

	/**
	 * Set the mouse cursor used to handle the camera and ray casts...
	 */
	void setMouseCursor(MouseCursor *mc);
	inline MouseCursor *getMouseCursor(void);

	/**
	 * Set the MenuManager to use. This automatically will update the MenuManager
	 */
	void setMenuManager(MenuManager *mm);
	inline MenuManager *getMenuManager(void);

	/**
	 * Configure the RaycastZones (this will be the zones that we will use
	 * to check for raycast and we assume that there are no "menues" there).
	 *
	 * @param bb	The boundingbox used to delimite the zone where the mouse
	 * 				cursor will produce a raycast if we click...
	 */
	void addLevelZone(const sm::AABB &bb);

	/**
	 * Check key / mouse buttons pressed and mouse position functions (interface)
	 */
	inline bool isKeyDown(inputID key) const;
	inline bool isMosuseDown(inputID key) const;
	inline float getMouseRelativeX(void) const;
	inline float getMouseRelativeY(void) const;

	/**
	 * Handle a InputActionObject. This way we have to check if we can handle
	 * or not the action and if we can, then we just activate the
	 * "HandleActionState" to handle it.
	 * @param	iao		The input action object to be handled
	 * @returns	true	if can handle it,
	 * 			false	otherwise
	 */
	bool handleActionObject(InputActionObject *iao);


	//				GameUnits handling
	/**
	 * Select new Unit. Here we will check if we are pressing KEY_GROUP_UNITS
	 * to determine if we have to "add" a new unit selected or if we are just
	 * selecting one and only one
	 * @param	unit	The game unit that we are selecting
	 * @note	This function must be called EVERYTIME a unit is selected.
	 */
	void unitSelected(GameUnit *unit);
	void unitUnselected(GameUnit *unit);
	void unselectAll(void);
	inline UnitSelVec &getUnitSelected(void);


	//				HUDManager Handling
	// TODO: aca deberiamos poner las funciones asociadas al hud. si es que
	// vamos a necesitar hacer cosas desde afuera.

	/**
	 * Update all the logic of the Input system (update the actual state).
	 */
	void update(void);

private:

	/**
	 * Configure default keys
	 */
	void configureDefaultKeys(void);

	/**
	 * Function used to handle keyboard input / mouse cursor input / raycast / menu
	 */
	void handleKeyboard(void);
	void handleMouse(void);
	void handleRaycast(void);


private:
	LevelManager			*mLevelManager;
	CameraController		*mCameraController;
	MouseCursor				*mMouseCursor;
	MenuManager				*mMenuManager;
	LevelZoneVec			mLevelZones;;
	int 					mKeys[NUM_KEYS];
	InputStateMachine		mStateMachine;
	InputActionObject		*mActualActionObj;
	UnitSelVec				mUnitsSelected;

};




////////////////////////////////////////////////////////////////////////////////
inline CameraController *InputManager::getCameraController(void)
{
	return mCameraController;
}


////////////////////////////////////////////////////////////////////////////////
inline MouseCursor *InputManager::getMouseCursor(void)
{
	return mMouseCursor;
}

////////////////////////////////////////////////////////////////////////////////
inline MenuManager *InputManager::getMenuManager(void)
{
	return mMenuManager;
}

////////////////////////////////////////////////////////////////////////////////
inline bool InputManager::isKeyDown(InputManager::inputID key) const
{
	return GLOBAL_KEYBOARD->isKeyDown(mKeys[key]);
}
inline bool InputManager::isMosuseDown(InputManager::inputID key) const
{
	return GLOBAL_MOUSE->getMouseState().buttonDown(mKeys[key]);
}
inline float InputManager::getMouseRelativeX(void) const
{
	ASSERT(mMouseCursor);
	return mMouseCursor->getXRelativePos();
}
inline float InputManager::getMouseRelativeY(void) const
{
	ASSERT(mMouseCursor);
	return mMouseCursor->getYRelativePos();
}

////////////////////////////////////////////////////////////////////////////////
inline InputManager::UnitSelVec &InputManager::getUnitSelected(void)
{
	return mUnitsSelected;
}


#endif /* INPUTMANAGER_H_ */
