/*
 * InputStateMachine.cpp
 *
 *  Created on: 04/08/2012
 *      Author: agustin
 *
 */

#include "InputStateMachine.h"

#include <BillboardManager/BillboardManager.h>
#include <SelectionSystem/SelectionType.h>
#include <SelectionSystem/SelectionManager.h>
#include <SelectionSystem/SelectableObject.h>
#include <LevelManager/LevelManager.h>
#include <GameUnits/PlayerUnit/PlayerUnit.h>

#include "InputManager/RaycastStates/EmptySelection.h"
#include "RaycastStates/SinglePlayerSel.h"
#include "RaycastStates/MultiPlayerSel.h"
#include "InputManager.h"


namespace input {

void
InputStateMachine::createStates(void)
{
    // TODO: complete this
    mStates[IS_EMPTY_SEL].reset(new EmptySelection());
    mStates[IS_SINGLE_PLAYER_SEL].reset(new SinglePlayerSel());
    mStates[IS_MULTI_PLAYER_SEL].reset(new MultiPlayerSel());
}

////////////////////////////////////////////////////////////////////////////////
bool
InputStateMachine::tryToShowOverlaysOnMap(void)
{

}

////////////////////////////////////////////////////////////////////////////////
void
InputStateMachine::calculateOffsets(void)
{
    ASSERT(mAuxVec.size() == mPlayerOffsets.size());
    ASSERT(!mAuxVec.empty());
    ASSERT(mAuxVec[0]->type() == selection::Type::SEL_TYPE_PLAYER);

    // get the player size
    float width, height;
    static_cast<PlayerUnit *>(mAuxVec[0])->getAABBFromEntity(width, height);
    const float max = (width > height) ? ISM_SEPARATION_FACTOR * width :
                                         ISM_SEPARATION_FACTOR * height;

    // now we need to generate the N new offsets, where N = num of players.
    const float deltaAngle = 360.0f / static_cast<float>(mAuxVec.size());
    mPlayerOffsets[0].x = 0.f;
    mPlayerOffsets[0].y = max;

    for (size_t i = 1, size = mAuxVec.size(); i < size; ++i) {
        // save the offset
        mPlayerOffsets[i] = mPlayerOffsets[i-1].rotateDegrees(deltaAngle);
    }

}


////////////////////////////////////////////////////////////////////////////////
void
InputStateMachine::handleRaycastedObjSingle(selection::SelectableObject *raycastedObj)
{
    ASSERT(raycastedObj);

    switch (raycastedObj->type()) {
    case selection::Type::SEL_TYPE_PLAYER:
    case selection::Type::SEL_TYPE_NONE:
    case selection::Type::SEL_TYPE_CIVIL:
    case selection::Type::SEL_TYPE_LVL_OBJECT:
        // Do nothing...
        break;
    case selection::Type::SEL_TYPE_ZOMBIE:
        // attack the zombie
        debugBLUE("ATTACKKKKKKK!!\n");
        break;
    case selection::Type::SEL_TYPE_COL_OBJECT:
    {
        // pick object!
        // get the player first
        ASSERT(mAuxVec.size() == 1);
        ASSERT(dynamic_cast<PlayerUnit*>(mAuxVec.back()));
        PlayerUnit* player = static_cast<PlayerUnit*>(mAuxVec.back());

        ASSERT(dynamic_cast<CollectableObject*>(raycastedObj));
        player->collectObject(static_cast<CollectableObject*>(raycastedObj));
    }
        break;
    default:
        ASSERT(false);
    }
}

void
InputStateMachine::handleRaycastedObjMulti(selection::SelectableObject *raycastedObj)
{
    ASSERT(raycastedObj);

    switch (raycastedObj->type()) {
    case selection::Type::SEL_TYPE_PLAYER:
    case selection::Type::SEL_TYPE_NONE:
    case selection::Type::SEL_TYPE_CIVIL:
    case selection::Type::SEL_TYPE_LVL_OBJECT:
    case selection::Type::SEL_TYPE_COL_OBJECT: // We don't want to pick nothing
        // Do nothing...
        break;
    case selection::Type::SEL_TYPE_ZOMBIE:
        // attack the zombie
        debugBLUE("ATTACKKKKKKK!!\n");
        break;
    default:
        ASSERT(false);
    }
}


////////////////////////////////////////////////////////////////////////////////
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

    debugWARNING("TODO: Change this using the new interface of billboards...\n");
    mMoveBillboards.resize(3);
    mMoveBillbardVisible.resize(3, false);
    for (size_t i = 0, size = mMoveBillbardVisible.size(); i < size; ++i) {
        mMoveBillboards[i] = billboard::BillboardManager::instance().getNewBillboard(0);
    }
}

////////////////////////////////////////////////////////////////////////////////
InputStateMachine::~InputStateMachine()
{

}

////////////////////////////////////////////////////////////////////////////////
void
InputStateMachine::selectionChanged(const selection::SelectionData &selData)
{
    // reset the mouse cursor
    MouseCursor &mc = *GLOBAL_CURSOR;
    if (mc.getCursor() != MouseCursor::Cursor::NORMAL_CURSOR) {
        mc.setCursor(MouseCursor::Cursor::NORMAL_CURSOR);
    }

    // hide move billboards (all of them just in case)
    for (size_t i = 0, size = mMoveBillboards.size(); i < size; ++i){
        mMoveBillboards[i]->setPosition(0.0f, -9999.0f, 0.f);
        mMoveBillbardVisible[i] = false;
    }

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
        const size_t numPlayersSel = mAuxVec.size();
        if (numPlayersSel == 1) {
            mState = State::IS_SINGLE_PLAYER_SEL;
            mActualState = mStates[IS_SINGLE_PLAYER_SEL].get();
        } else {
            mState = State::IS_MULTI_PLAYER_SEL;
            mActualState = mStates[IS_MULTI_PLAYER_SEL].get();

            // reserve the space needed for the last location saved
            mSavedPositions.resize(numPlayersSel);
            mPlayerOffsets.resize(numPlayersSel);

            // re-calculate the offsets (TODO: probably we can avoid recalculating
            // this each time we change selection, since we can pre-define it
            // but just in the future we want to select more than 3 players)
            calculateOffsets();

            // check if we need to reallocate new billboards
            if (numPlayersSel > mMoveBillboards.size()) {
                debugOPTIMIZATION("If we know the size of the max number of GameUnits "
                    "that we will be able to select at the same time then we"
                    " can avoid this calculations\n");

                size_t numBillboards = mMoveBillboards.size();
                const size_t remainingBillboards = numPlayersSel - numBillboards;
                mMoveBillbardVisible.resize(numPlayersSel);
                mMoveBillboards.resize(numPlayersSel);
                for (; numBillboards < numPlayersSel; ++numBillboards) {
                    mMoveBillbardVisible[numBillboards] = false;
                    // TODO: check if we have to change this way to get the
                    // billboards?
                    mMoveBillboards[numBillboards] =
                        billboard::BillboardManager::instance().getNewBillboard(0);
                }
            }
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
    MouseCursor &mc = *GLOBAL_CURSOR;

    // We have to perform raycast?
    if (mInputManager.shouldPerformRaycast()){
        // perform it and do whatever we need
        mActualState->executeRayCast();
    } else {
        // probably we don't wan to do nothing? because for example the mouse
        // is still pressed from the last frame, here we can handle the
        // rectangle selection?
        return;
    }

    // check for raycasted objects now
    selection::SelectableObject *raycastedObj = mActualState->lastRaycastedObj();

    // now check of mouse left button press, if the button is pressed then we
    // will want to:
    // 1) Unselect players (if raycastedObj == 0
    // 2) Select a new player?
    // for now we will select the object only
    const bool leftMousePressed =
        mInputManager.isKeyDown(inputID::MOUSE_BUTTON_LEFT);

    if (leftMousePressed) {
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

            // if it is a player then select it
            if (playerClicked) {
                mSelManager.select(raycastedObj);
            }
            return;
        }
    }

    // now we have to execute the logic depending on the current selectio-state
    const bool rightButtonReleased =
        mInputManager.isKeyReleased(inputID::MOUSE_BUTTON_RIGHT);
    const bool rightButtonPressed =
            mInputManager.isKeyDown(inputID::MOUSE_BUTTON_RIGHT);
    const bool isAPlayerSelected = raycastedObj &&
        raycastedObj->type() == selection::Type::SEL_TYPE_PLAYER;

    switch (mState) {
    ////////////////////////////////////////////////////////////////////////////
    case State::IS_EMPTY_SEL:
        // we have nothing to do?..
        break;
    ////////////////////////////////////////////////////////////////////////////
    case State::IS_MULTI_PLAYER_SEL:
    {
        // We have more than one player selected.... we have to:
        // 1) If we are not pressing the right button:
        //      a) We are over a Zombie (since objects we cannot select) we
        //         have to change the mouse icon (attack icon). Probably if we
        //         are over an object we want to show a "INVALID_ACTION" icon?
        //      b) If we are over a free space (in the world) we need to show
        //         the possible new locations for the selected players. These
        //         new locations should be always inside the level (for example
        //         if sudently we put the mouse outside the pathfinding, then
        //         a good approach is to show the last saved "good" location).
        //
        // 2) We are pressing the right button:
        //      a) If we are over a free area in the world we need to move the
        //         players to the new location (inside the pathfinding area)
        //      b) If we are over a Zombie then we need to send all the units
        //         to kill him
        //

        Ogre::Vector3 pointInPlane;
        mActualState->performRayAgainstPlane(pointInPlane);
        const bool isPointInPath = mActualState->isPointInPath(pointInPlane);

        if (!rightButtonReleased) {
            // check if we are over an object
            if (raycastedObj != 0) {
                // we are over an object... then we have to handle the mouse
                // icon depending of the object
                showCursorMultiPlayer(raycastedObj->type());

                // and hide the current billboards
                hideCurrentBillboards();
            } else {
                // we have not raycasted any object, check if we have to show
                // the billboard of movement in the map
                if (isPointInPath) {
                    // check if the point is the same than the last frame
                    if (pointInPlane != mLastPointInPlane) {
                        // it is a new point, we need to recalculate everything

                        // show the billboard and the cursor.. We need to calculate
                        // the new locations of the players in the map and then
                        // show the billboards
                        mc.setCursor(MouseCursor::Cursor::MOVE_CRUSOR);
                        calculateLocations(pointInPlane);

                        ASSERT(mMoveBillboards.size() >= mSavedPositions.size());
                        for (size_t i = 0, size = mSavedPositions.size(); i < size;
                             ++i) {
                            mMoveBillboards[i]->setPosition(mSavedPositions[i]);
                            mMoveBillbardVisible[i] = true;
                        }
                    }
                    // else, the mouse didn't move, so we do nothing

                } else {
                    // we have nothing to show nor do...
                    mc.setCursor(MouseCursor::Cursor::NORMAL_CURSOR);
                    if (mMoveBillbardVisible[0]) {
                        hideCurrentBillboards();
                    }
                }
            }
        } else {
            // we just release the button
            if (raycastedObj != 0) {
                // we just click over an object...
                handleRaycastedObjMulti(raycastedObj);
            } else {
                // no object raycasted... check if we have to move the player
                // to that position
                if (isPointInPath) {
                    // move the players
                    ASSERT(mAuxVec.size() == mSavedPositions.size());

                    for (size_t i = 0, size = mAuxVec.size(); i < size; ++i) {
                        ASSERT(dynamic_cast<PlayerUnit *>(mAuxVec[i]));
                        PlayerUnit *player = static_cast<PlayerUnit *>(mAuxVec[i]);
                        const Ogre::Vector3& destPos = mSavedPositions[i];
                        player->moveUnitTo(math::Vector2(destPos.x, destPos.z));
                    }
                } else {
                    // emit sound that the player couldn't move over there?
                    // TODO:
                }
            }
        }

        // if we are pressing the button then we probably want to show
        // other mouse cursor (to give an idea of animation)
        // TODO
        mLastPointInPlane = pointInPlane;

    }
        break;
    ////////////////////////////////////////////////////////////////////////////
    case State::IS_OBJECT_SEL:

        break;
    ////////////////////////////////////////////////////////////////////////////
    case State::IS_SINGLE_PLAYER_SEL:
    {
        // we have a player selected, we have to do:
        // 1) If we are not pressing the right button, then we want to show
        //    a overlay in the pathfinding place
        // 2) If we are not pressing the right button and we are over an object
        //    we have to change the mouse cursor icon (depending on the object
        //    we are over).
        // 3) If we are pressing the right button and we are over a free space
        //    then we have to move the player over there.
        // 4) If we are over an object and pressing the button then we have
        //    to send the player over the object and pick it / activate it..

        Ogre::Vector3 pointInPlane;
        mActualState->performRayAgainstPlane(pointInPlane);
        const bool isPointInPath = mActualState->isPointInPath(pointInPlane);

        if (!rightButtonReleased) {
            // check if we are over an object
            if (raycastedObj != 0) {
                // we are over an object... then we have to handle the mouse
                // icon depending of the object
                showCursorSinglePlayer(raycastedObj->type());

                // hide the current billboard if we need
                if (mMoveBillbardVisible[0]) {
                    hideCurrentBillboards();
                }
            } else {
                // we have not raycasted any object, check if we have to show
                // the billboard of movement in the map
                if (isPointInPath) {
                    // check if the mouse was moved from the last frame
                    if (pointInPlane != mLastPointInPlane) {
                        // the mouse was moved, do the logic
                        // show the billboard and the cursor
                        mc.setCursor(MouseCursor::Cursor::MOVE_CRUSOR);
                        mMoveBillboards[0]->setPosition(pointInPlane);
                        mMoveBillbardVisible[0] = true;
                    }
                    // else, the mouse is in the same place, do nothing

                } else {
                    // we have nothing to show nor do...
                    mc.setCursor(MouseCursor::Cursor::NORMAL_CURSOR);
                    if (mMoveBillbardVisible[0]) {
                        mMoveBillbardVisible[0] = false;
                        mMoveBillboards[0]->setPosition(0.0f, -9999.0f, 0.f);
                    }
                }
            }
        } else {
            // we just release the button
            if (raycastedObj != 0) {
                // we just click over an object...
                handleRaycastedObjSingle(raycastedObj);
            } else {
                // no object raycasted... check if we have to move the player
                // to that position
                if (isPointInPath) {
                    // move the player
                    ASSERT(dynamic_cast<PlayerUnit *>(mAuxVec.back()));
                    PlayerUnit *player = static_cast<PlayerUnit *>(mAuxVec.back());
                    player->moveUnitTo(math::Vector2(pointInPlane.x, pointInPlane.z));
                } else {
                    // emit sound that the player couldn't move over there?
                    // TODO:
                }
            }
        }

        // if we are pressing the button then we probably want to show
        // other mouse cursor (to give an idea of animation)
        // TODO
        mLastPointInPlane = pointInPlane;
    }
        break;
    ////////////////////////////////////////////////////////////////////////////
    default:
        // WTF!
        ASSERT(false);
    }
}



}
