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
//#include "InputStateMachine.h"
#include "DebugUtil.h"
#include "InputMouse.h"
#include "InputKeyboard.h"

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
	static const float	CAMERA_MOVE_BOUNDS_L_LIMIT;
	static const float	CAMERA_MOVE_BOUNDS_R_LIMIT;
	static const float	CAMERA_MOVE_BOUNDS_T_LIMIT;
	static const float	CAMERA_MOVE_BOUNDS_B_LIMIT;


	// typedefs
	typedef std::vector<sm::AABB>		LevelZoneVec;

	// Internal flags
	enum Flag {
	    RaycastEnabled = (1 << 1),
	    CameraMovementEnabled = (1 << 2),
	    CameraRotationEnabled = (1 << 3),
	    CameraZoomEnabled =     (1 << 4),

	};

	// Events and state
	enum Event {
	    E_NONE = 0,
	    E_DONE,
	    E_ROTATE_CAMERA,
	};

	enum State {
	    S_NORMAL = 0,
	    S_ROTATING_CAMERA,

	};

public:
	// Here we let public the possibility to configure the keys used in the game
	enum inputID {
		KEY_GROUP_UNITS = 0,
		KEY_MOVE_CAM_UP,
		KEY_MOVE_CAM_DOWN,
		KEY_MOVE_CAM_LEFT,
		KEY_MOVE_CAM_RIGHT,
		KEY_MOVE_CAM_FREE,		// key used to move the cam with the mouse
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

	/**
	 * Check if we have to handle the Mouse Raycast (using the
	 * MouseSelectionHandler).
	 */
	bool shouldPerformRaycast(void) const;

	/**
	 * State machine associated functions
	 */
	void newStateEvent(Event e);
	void initState(State s);

	/**
	 * Handle camera movement (camera movement using the keyboard and the mouse
	 * position).
	 * Also handle camera rotation
	 */
	void handleCameraMovement(void);
	void handleCameraRotation(void);
	void handleCameraZoom(void);


	// Flag handling helper functions
	//
	inline void setFlag(int f);
	inline bool isSet(int f) const;
	inline void clear(int f);
	inline void clearAll(void);
	inline void setAll(void);

private:
	LevelManager			*mLevelManager;
	CameraController		*mCameraController;
	MenuManager				*mMenuManager;
	LevelZoneVec			mLevelZones;;
	int 					mKeys[NUM_KEYS];
//	InputStateMachine		mStateMachine;
	InputActionObject		*mActualActionObj;
	UnitSelVec				mUnitsSelected;
	int                     mFlags;
	State                   mActualState;
	State                   mLastState;
	Event                   mLastEvent;

};




////////////////////////////////////////////////////////////////////////////////
inline CameraController *InputManager::getCameraController(void)
{
	return mCameraController;
}

////////////////////////////////////////////////////////////////////////////////
inline MenuManager *InputManager::getMenuManager(void)
{
	return mMenuManager;
}

////////////////////////////////////////////////////////////////////////////////
inline bool InputManager::isKeyDown(InputManager::inputID key) const
{
	return input::InputKeyboard::isKeyDown(static_cast<input::KeyCode>(
	        mKeys[key]));
}
inline bool InputManager::isMosuseDown(InputManager::inputID key) const
{
	return input::InputMouse::isMouseDown(static_cast<input::MouseButtonID>(
	        mKeys[key]));
}
inline float InputManager::getMouseRelativeX(void) const
{
	return GLOBAL_CURSOR->getXRelativePos();
}
inline float InputManager::getMouseRelativeY(void) const
{
	return GLOBAL_CURSOR->getYRelativePos();
}

////////////////////////////////////////////////////////////////////////////////
inline InputManager::UnitSelVec &InputManager::getUnitSelected(void)
{
	return mUnitsSelected;
}

////////////////////////////////////////////////////////////////////////////////
inline void InputManager::setFlag(int f)
{
    mFlags |= f;
}
inline bool InputManager::isSet(int f) const
{
    return mFlags & f;
}
inline void InputManager::clear(int f)
{
    mFlags &= ~f;
}
inline void InputManager::clearAll(void)
{
    mFlags = 0;
}
inline void InputManager::setAll(void)
{
    mFlags = ~0;
}


#endif /* INPUTMANAGER_H_ */
