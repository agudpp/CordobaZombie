/*
 * InputManager.cpp
 *
 *  Created on: 19/03/2012
 *      Author: agustin
 */


#include "InputManager.h"

#include <boost/bind.hpp>

#include <CameraController/CameraController.h>
#include <LevelManager/LevelManager.h>
#include <SelectionSystem/SelectableObject.h>

#include "IInputState.h"
#include "InputStateMachine.h"



// Helper MACRO for input checking and release keys
//
#define INPUT_CHECK(buttonCondition, pressedFlag, codeToRunOnPressed) \
    if(buttonCondition){\
        if(!pressedFlag){\
            pressedFlag = 1;\
            codeToRunOnPressed\
        }\
    } else {\
        pressedFlag = 0;\
    }

namespace input {

////////////////////////////////////////////////////////////////////////////////
const float  InputManager::CAMERA_MOVE_BOUNDS_LIMITS   =   0.01f;
const float  InputManager::CAMERA_MOVE_BOUNDS_L_LIMIT  =   CAMERA_MOVE_BOUNDS_LIMITS;
const float  InputManager::CAMERA_MOVE_BOUNDS_R_LIMIT  =   1.0f-CAMERA_MOVE_BOUNDS_LIMITS;
const float  InputManager::CAMERA_MOVE_BOUNDS_T_LIMIT  =   CAMERA_MOVE_BOUNDS_LIMITS;
const float  InputManager::CAMERA_MOVE_BOUNDS_B_LIMIT  =   1.0f-CAMERA_MOVE_BOUNDS_LIMITS;

const float  InputManager::CAMERA_ZOOM_VEL  =   5.f;

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

}

void
InputManager::handleRaycastedObj(selection::SelectableObject *raycastedObj)
{
    // if the object is already selected probably we don't want to do anything..
    if (mSelManager.isSelected(raycastedObj)) {
        return;
    }

    // check if Control is pressed
    if(!input::InputKeyboard::isKeyDown(static_cast<input::KeyCode>(
                           mKeys[inputID::KEY_GROUP_UNITS]))) {
        // add selection
        mSelManager.select(raycastedObj);
    } else {
        // we want to remove all the selection and just select this one
        mSelManager.unselectAll();
        mSelManager.select(raycastedObj);
    }

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
        scrollZoom += CAMERA_ZOOM_VEL;
    } else if (lMouseZ < 0.0f) {
        scrollZoom -= CAMERA_ZOOM_VEL;
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
		mStateMachine(new InputStateMachine(selection::SelectionManager::getInstance())),
		mFlags(~0),
		mActualState(S_NORMAL),
		mLastState(S_NORMAL),
		mLastEvent(E_NONE),
		mSelManager(selection::SelectionManager::getInstance())
{
	configureDefaultKeys();

	// register the InputStateMachine to the selection manager to receive
	// events
	mSelManager.addCallback(boost::bind(&InputStateMachine::selectionChanged,
	                                    mStateMachine,
	                                    _1));
}

////////////////////////////////////////////////////////////////////////////////
InputManager::~InputManager()
{
    delete mStateMachine;
}

////////////////////////////////////////////////////////////////////////////////
void
InputManager::setLevelManager(LevelManager *lm)
{
	ASSERT(lm);
	mLevelManager = lm;
}

////////////////////////////////////////////////////////////////////////////////
void
InputManager::setCameraController(CameraController *cc)
{
	ASSERT(cc);
	mCameraController = cc;
}


////////////////////////////////////////////////////////////////////////////////
void
InputManager::setMenuManager(MenuManager *mm)
{
	ASSERT(mm);
	mMenuManager = mm;
}

////////////////////////////////////////////////////////////////////////////////
void
InputManager::addLevelZone(const sm::AABB &bb)
{
	ASSERT(bb.tl.x >= 0 && bb.tl.x <= 1);
	ASSERT(bb.tl.y >= 0 && bb.tl.y <= 1);
	ASSERT(bb.br.x >= 0 && bb.br.x <= 1);
	ASSERT(bb.br.y >= 0 && bb.br.y <= 1);

	mLevelZones.push_back(bb);
}


/**
 * TODO: set menu system? aca podemos meter el menu system asociado
 */
////////////////////////////////////////////////////////////////////////////////
void
InputManager::update(void)
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

    // We have to perform raycast?
    if (shouldPerformRaycast()){
        // perform it and do whatever we need
        mStateMachine->executeRayCast();
    } else {
        // probably we don't wan to do nothing? because for example the mouse
        // is still pressed from the last frame, here we can handle the
        // rectangle selection?
        return;
    }

    // check for raycasted objects now
    selection::SelectableObject *raycastedObj = mStateMachine->lastRaycastedObj();

    // now check of mouse left button press, if the button is pressed then we
    // will want to:
    // 1) Unselect players (if raycastedObj == 0
    // 2) Select a new player?
    // for now we will select the object only

    INPUT_CHECK(InputMouse::isMouseDown(input::MB_Left),\
                mInputFlags.leftButtonPressed,\
                // if there are no object raycasted then we want to unselect
                // all the objects
                if (raycastedObj == 0) {
                    mSelManager.unselectAll();
                    return;
                }

                // else we want to handle the raycasted object
                handleRaycastedObj(raycastedObj);
    )


}

}
