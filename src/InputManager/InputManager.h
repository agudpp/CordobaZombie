/*
 * InputManager.h
 *
 *  Created on: 19/03/2012
 *      Author: agustin
 */

#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include <cstdlib>

#include <Common/GlobalObjects/GlobalObjects.h>
#include <Common/Math/CommonMath.h>
#include <Common/DebugUtil/DebugUtil.h>
#include <Utils/MouseCursor/MouseCursor.h>

#include "InputMouse.h"
#include "InputKeyboard.h"


// Forward declarations
//
class CameraController;
class LevelManager;
class MenuManager;
class IInputState;
class GameUnit;

namespace selection {
class SelectionManager;
class SelectableObject;
}

namespace input {

class InputStateMachine;

class InputManager
{
	// define the bounds where the mouse will start to move the camera in %
	// relative to the window size
	static const float	CAMERA_MOVE_BOUNDS_LIMITS;
	static const float	CAMERA_MOVE_BOUNDS_L_LIMIT;
	static const float	CAMERA_MOVE_BOUNDS_R_LIMIT;
	static const float	CAMERA_MOVE_BOUNDS_T_LIMIT;
	static const float	CAMERA_MOVE_BOUNDS_B_LIMIT;

	// Camera Zoom velocity
	static const float	CAMERA_ZOOM_VEL;


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

	// Flags used for the mouse buttons and keys pressed/released logic
	//
	struct InputFlags {
	    InputFlags()
	    {
	        clearFlags();
	    }

	    // mouse flags
	    unsigned char leftButtonPressed : 1;
	    unsigned char rightButtonPressed : 1;
	    unsigned char middleButtonPressed : 1;

	    // keyboard flags


	    void clearFlags(void)
	    {
	        std::memset(this, 0, sizeof(InputFlags));
	    }
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
		NUM_KEYS,
	};


public:
	/**
	 * Singleton
	 */
	static InputManager &getInstance(void)
	{
	    static InputManager instance;
	    return instance;
	}

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


	//				HUDManager Handling
	// TODO: aca deberiamos poner las funciones asociadas al hud. si es que
	// vamos a necesitar hacer cosas desde afuera.

	/**
	 * Update all the logic of the Input system (update the actual state).
	 */
	void update(void);

private:
	InputManager();
	InputManager(const InputManager &);
	InputManager &operator=(const InputManager);


	/**
	 * @brief Configure default keys
	 */
	void configureDefaultKeys(void);

	/**
	 * @brief Handle the raycasted object for the current state
	 * @param raycastedObj  The current raycasted object
	 */
	void handleRaycastedObj(selection::SelectableObject *raycastedObj);

	/**
	 * @brief Function used to handle keyboard input / mouse cursor input /
	 *        raycast / menu
	 */
	void handleKeyboard(void);
	void handleMouse(void);
	void handleRaycast(void);

	/**
	 * @brief Check if we have to handle the Mouse Raycast (using the
	 *        MouseSelectionHandler).
	 */
	inline bool shouldPerformRaycast(void) const;

	/**
	 * @brief State machine associated functions
	 */
	void newStateEvent(Event e);
	void initState(State s);

	/**
	 * @brief Handle camera movement (camera movement using the keyboard and the mouse
	 *        position).
	 *        Also handle camera rotation
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
	LevelManager            *mLevelManager;
	CameraController        *mCameraController;
	MenuManager             *mMenuManager;
	LevelZoneVec            mLevelZones;;
	int                     mKeys[NUM_KEYS];
	InputStateMachine       *mStateMachine;
	int                     mFlags;
	State                   mActualState;
	State                   mLastState;
	Event                   mLastEvent;
	selection::SelectionManager &mSelManager;
	InputFlags              mInputFlags;

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

////////////////////////////////////////////////////////////////////////////////
inline bool
InputManager::shouldPerformRaycast(void) const
{
    //TODO: do the real check here
    return true;

    const sm::Vector2 mousePosition(GLOBAL_CURSOR->getXRelativePos(),
                                    GLOBAL_CURSOR->getYRelativePos());
    // TODO: probably here we have to improve this using the MainMenuManager
    // or sorting by X and Y
    for(size_t i = 0, size = mLevelZones.size(); i < size; ++i){
        if (mLevelZones[i].checkPointInside(mousePosition)){
            // We should perform the raycast
            return true;
        }
    }
    return false;
}

}
#endif /* INPUTMANAGER_H_ */
