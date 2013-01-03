/*
 * InputManager.cpp
 *
 *  Created on: 19/03/2012
 *      Author: agustin
 */


#include "InputManager.h"

#include "CameraController.h"
#include "LevelManager.h"
#include "InputActionObject.h"
#include "IInputState.h"
#include "GameUnit.h"
#include "MouseCursor.h"

////////////////////////////////////////////////////////////////////////////////
const float  InputManager::CAMERA_MOVE_BOUNDS_L_LIMIT  =   CAMERA_MOVE_BOUNDS_LIMITS;
const float  InputManager::CAMERA_MOVE_BOUNDS_R_LIMIT  =   1.0f-CAMERA_MOVE_BOUNDS_LIMITS;
const float  InputManager::CAMERA_MOVE_BOUNDS_T_LIMIT  =   CAMERA_MOVE_BOUNDS_LIMITS;
const float  InputManager::CAMERA_MOVE_BOUNDS_B_LIMIT  =   1.0f-CAMERA_MOVE_BOUNDS_LIMITS;

////////////////////////////////////////////////////////////////////////////////
void
InputManager::configureDefaultKeys(void)
{
	// Keyboard
	mKeys[KEY_GROUP_UNITS] 		= input::KC_LCONTROL;
	mKeys[KEY_MOVE_CAM_UP] 		= input::KC_W;
	mKeys[KEY_MOVE_CAM_DOWN] 	= input::KC_S;
	mKeys[KEY_MOVE_CAM_LEFT] 	= input::KC_A;
	mKeys[KEY_MOVE_CAM_RIGHT]	= input::KC_D;
	mKeys[KEY_MOVE_CAM_FREE]	= input::KC_LCONTROL;
	mKeys[KEY_EXIT_GAME]		= input::KC_ESCAPE;
	mKeys[KEY_PAUSE_GAME]		= input::KC_PAUSE;
	mKeys[KEY_OPEN_CELLPHONE]	= input::KC_ADD;

	// Mouse
	mKeys[MOUSE_BUTTON_LEFT]	= input::MB_Left;
	mKeys[MOUSE_BUTTON_RIGHT]	= input::MB_Right;
	mKeys[MOUSE_BUTTON_MIDDLE]	= input::MB_Middle;
}

////////////////////////////////////////////////////////////////////////////////
void
InputManager::handleKeyboard(void)
{
    ASSERT(false);
}

////////////////////////////////////////////////////////////////////////////////
void
InputManager::handleMouse(void)
{

}

////////////////////////////////////////////////////////////////////////////////
void
InputManager::handleRaycast(void)
{

}

////////////////////////////////////////////////////////////////////////////////
bool
InputManager::shouldPerformRaycast(void) const
{
    ASSERT(false);
}

////////////////////////////////////////////////////////////////////////////////
void
InputManager::newStateEvent(Event e)
{
    switch(e){
    ////////////////////////////////////////////////////////////
    case E_DONE:
    {
        switch(mActualState){
        ////////////////////////////////////////////////
        case S_ROTATING_CAMERA:
        {
            // we need to restore the cursor visibility and position
            GLOBAL_CURSOR->setVisible(true);
            GLOBAL_CURSOR->restorePosition();
            initState(S_NORMAL);
        }
        break;
        ////////////////////////////////////////////////
        default:
            debugERROR("State not contemplated %d\n", static_cast<int>(mActualState));
            ASSERT(false);
        }
    }
    break;

    ////////////////////////////////////////////////////////////
    case E_ROTATE_CAMERA:
    {
        ASSERT(mActualState == S_NORMAL);
        initState(S_ROTATING_CAMERA);
    }
    break;
    ////////////////////////////////////////////////////////////
    default:
        // ?
        debugERROR("Event not contemplated %d\n", static_cast<int>(e));
        ASSERT(false);

    }
    mLastEvent = e;
}

void
InputManager::initState(State s)
{
    switch(s) {
    case S_NORMAL:

        break;

    case S_ROTATING_CAMERA:
    {
        // save the position of the mouse and hide it
        GLOBAL_CURSOR->setVisible(false);
        GLOBAL_CURSOR->savePosition();
    }
    break;

    default:
        // ?
        debugERROR("State not contemplated %d\n", static_cast<int>(s));
        ASSERT(false);
    }

    mActualState = s;
}

////////////////////////////////////////////////////////////////////////////////
void
InputManager::handleCameraMovement(void)
{
    ASSERT(isSet(Flag::CameraMovementEnabled));

    const Ogre::Real xRel = GLOBAL_CURSOR->getXRelativePos();
    const Ogre::Real yRel = GLOBAL_CURSOR->getYRelativePos();
    Ogre::Vector3 translationVec = Ogre::Vector3::ZERO;

    if ((xRel <= CAMERA_MOVE_BOUNDS_L_LIMIT) ||
            (input::InputKeyboard::isKeyDown(static_cast<input::KeyCode>(
                    mKeys[inputID::KEY_MOVE_CAM_LEFT])))) {
        // we need to move to the left
        translationVec.x -= 1.0f;
    } else if ((xRel >= CAMERA_MOVE_BOUNDS_R_LIMIT) ||
            (input::InputKeyboard::isKeyDown(static_cast<input::KeyCode>(
                    mKeys[inputID::KEY_MOVE_CAM_RIGHT])))) {
        // we need to move to the right
        translationVec.x += 1.0f;
    }

    if ((yRel >= CAMERA_MOVE_BOUNDS_B_LIMIT) ||
            (input::InputKeyboard::isKeyDown(static_cast<input::KeyCode>(
                    mKeys[inputID::KEY_MOVE_CAM_DOWN])))) {
        // we need to move to the left
        translationVec.z += 1.0f;
    } else if ((yRel <= CAMERA_MOVE_BOUNDS_T_LIMIT) ||
            (input::InputKeyboard::isKeyDown(static_cast<input::KeyCode>(
                    mKeys[inputID::KEY_MOVE_CAM_UP])))) {
        // we need to move to the right
        translationVec.z -= 1.0f;
    }

    if (translationVec != Ogre::Vector3::ZERO) {
        mCameraController->moveCamera(translationVec);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
InputManager::handleCameraRotation(void)
{
    ASSERT(isSet(Flag::CameraRotationEnabled));

    const float lMouseX = float(input::InputMouse::relX());
    const float lMouseY = float(input::InputMouse::relY());

    // should perform any rotation?
    if (lMouseX == 0.0f && lMouseY == 0.0f) {
        // nothing to rotate
        return;
    }

    // rotate the camera
    const float factor = -0.01 * mCameraController->getRotationVelocity();
    mCameraController->rotateCamera(Ogre::Radian(lMouseX * factor),
                                    Ogre::Radian(lMouseY * factor));

}

////////////////////////////////////////////////////////////////////////////////
void
InputManager::handleCameraZoom(void)
{
    ASSERT(isSet(Flag::CameraZoomEnabled));
    // use the mouse scroll
    const float lMouseZ = float(input::InputMouse::relZ());
    float scrollZoom = mCameraController->zoom();
    if (lMouseZ > 0.0f) {
        scrollZoom += 1.f;
    } else if (lMouseZ < 0.0f) {
        scrollZoom -= 1.f;
    }
    if(scrollZoom != mCameraController->zoom()){
        mCameraController->zoomCamera(scrollZoom);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
InputManager::InputManager() :
		mLevelManager(0),
		mCameraController(0),
		mMenuManager(0),
		mActualActionObj(0),
		mFlags(~0),
		mActualState(S_NORMAL),
		mLastState(S_NORMAL),
		mLastEvent(E_NONE)
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
	mLevelManager = lm;
}

////////////////////////////////////////////////////////////////////////////////
void InputManager::setCameraController(CameraController *cc)
{
	ASSERT(cc);
	mCameraController = cc;
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
    // update the GLOBAL_CURSOR (TODO: probably we don't want to do this always)
    GLOBAL_CURSOR->updatePosition(input::InputMouse::absX(),
                                  input::InputMouse::absY());

    // process the actual state
    switch(mActualState) {
    case S_NORMAL:
    {
        // if it is enable the camera movement then we handle it
        if (isSet(Flag::CameraMovementEnabled)) {
            handleCameraMovement();
        }

        // check if we have to rotate the camera
        if(isSet(Flag::CameraRotationEnabled) &&
                (input::InputKeyboard::isKeyDown(static_cast<input::KeyCode>(
                       mKeys[inputID::KEY_MOVE_CAM_FREE])))) {
            // new event
            newStateEvent(E_ROTATE_CAMERA);
            return;
        }

        if (isSet(Flag::CameraZoomEnabled)){
            handleCameraZoom();
        }
        // TODO: continue doing all the others things
    }
    break;

    case S_ROTATING_CAMERA:
    {
        // check if we finish rotating the camera
        if(!input::InputKeyboard::isKeyDown(static_cast<input::KeyCode>(
                               mKeys[inputID::KEY_MOVE_CAM_FREE]))) {
            // stop rotating camera
            newStateEvent(E_DONE);
            return;
        }

        // handle rotation
        handleCameraRotation();
    }
    break;

    default:
        // ?
        ASSERT(false);
    }
}
