/*
 * InputStateMachine.cpp
 *
 *  Created on: 04/08/2012
 *      Author: agustin
 *
 */

#include "InputStateMachine.h"

#include <SelectionSystem/SelectionType.h>
#include <SelectionSystem/SelectionManager.h>
#include <SelectionSystem/SelectableObject.h>
#include <LevelManager/LevelManager.h>
#include <Common/DebugUtil/DebugUtil.h>

#include "InputManager/RaycastStates/EmptySelection.h"
#include "RaycastStates/SinglePlayerSel.h"
#include "InputManager.h"


namespace input {

void
InputStateMachine::createStates(void)
{
    // TODO: complete this
    mStates[IS_EMPTY_SEL].reset(new EmptySelection());
    mStates[IS_SINGLE_PLAYER_SEL].reset(new SinglePlayerSel());
}

////////////////////////////////////////////////////////////////////////////////
InputStateMachine::InputStateMachine(selection::SelectionManager &selManager,
                                     LevelManager *lvlManager,
                                     InputManager &inputManager) :
    mActualState(0)
,   mSelManager(selManager)
,   mLevelManager(lvlManager)
,   mInputManager(inputManager)
,   mState(State::IS_EMPTY_SEL)
{
    ASSERT(lvlManager != 0);

    createStates();

    // by default use the empty selectio state
    mActualState = mStates[IS_EMPTY_SEL].get();
}

////////////////////////////////////////////////////////////////////////////////
InputStateMachine::~InputStateMachine()
{

}

////////////////////////////////////////////////////////////////////////////////
void
InputStateMachine::selectionChanged(const selection::SelectionData &selData)
{
    // TODO: here we should perform all the logic to set the correct state
    if (selData.selected.empty()){
        // we have no selection
        mActualState = mStates[IS_EMPTY_SEL].get();
        mActualState->configure(selData);
        mActualState->resetLastRaycastedObj();
        mState = State::IS_EMPTY_SEL;
        return;
    }

    // else some selection is there, check for players
    mAuxVec.clear();
    mSelManager.getObjects(selection::Type::SEL_TYPE_PLAYER, mAuxVec);
    if (!mAuxVec.empty()) {
        // players selected..
        if (mAuxVec.size() == 1) {
            mState = State::IS_SINGLE_PLAYER_SEL;
            mActualState = mStates[IS_SINGLE_PLAYER_SEL].get();
        } else {
            mState = State::IS_MULTI_PLAYER_SEL;
            mActualState = mStates[IS_MULTI_PLAYER_SEL].get();
        }

        mActualState->configure(selData);
        mActualState->resetLastRaycastedObj();

        return;
    }
    // else objects selected??
    ASSERT(false);
}

////////////////////////////////////////////////////////////////////////////////
void
InputStateMachine::update(void)
{
    // We have to perform raycast?
    if (mInputManager.shouldPerformRaycast()){
        // perform it and do whatever we need
        mActualState->executeRayCast();
    } else {
        // probably we don't wan to do nothing? because for example the mouse
        // is still pressed from the last frame, here we can handle the
        // rectangle selection?
//        return;
    }

    // check for raycasted objects now
    selection::SelectableObject *raycastedObj = mActualState->lastRaycastedObj();

    // now check of mouse left button press, if the button is pressed then we
    // will want to:
    // 1) Unselect players (if raycastedObj == 0
    // 2) Select a new player?
    // for now we will select the object only
    if (mInputManager.isKeyDown(inputID::MOUSE_BUTTON_LEFT)) {
        // if the mouse was pressed in the last frame then we are doing the
        // box selection probably... note that the frame rate is very high
        // so we need to use some kind of time counter here.
        if (mInputManager.wasKeyPressed(inputID::MOUSE_BUTTON_LEFT)) {
            // doing box selection... return for now: TODO
            return;
        }
    }

    const bool leftButtonReleased =
        mInputManager.isKeyReleased(inputID::MOUSE_BUTTON_LEFT);
    const bool groupUnitPressed =
        mInputManager.isKeyDown(inputID::KEY_GROUP_UNITS);

    // if we had just released the mouse check for some cases
    if (leftButtonReleased) {

        // check if the selection is empty, in that case then we want to clear the
        // selection if and only if we are not pressing the "add selection" key
        if (raycastedObj == 0) {
            if (!groupUnitPressed) {
                // we want to unselect all
                mSelManager.unselectAll();
            }
            // else, we raycast and press a click in an empty place, do nothing?
            return;
        }

        // if we are here then we had a raycasted object and we just click on it
        const bool playerClicked =
            raycastedObj->type() == selection::Type::SEL_TYPE_PLAYER;
        if (groupUnitPressed) {
            // trying to add a player to the selection list?
            if (playerClicked) {
                // yeah, add it in the selection manager and return
                mSelManager.select(raycastedObj);
                return;
            } else {
                // no player clicked, do nothing...
                return;
            }
        } else {
            // doesn't care the current selection
            mSelManager.unselectAll();
            mSelManager.select(raycastedObj);
            return;
        }
    }

    // now we have to execute the logic depending on the current selectio-state
    const bool rightButtonReleased =
        mInputManager.isKeyReleased(inputID::MOUSE_BUTTON_RIGHT);

    switch (mState) {
    case State::IS_EMPTY_SEL:

        break;
    case State::IS_MULTI_PLAYER_SEL:

        break;
    case State::IS_OBJECT_SEL:

        break;
    case State::IS_SINGLE_PLAYER_SEL:

        break;
    default:
        // WTF!
        ASSERT(false);
    }
}



}
