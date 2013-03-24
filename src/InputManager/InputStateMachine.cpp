/*
 * InputStateMachine.cpp
 *
 *  Created on: 04/08/2012
 *      Author: agustin
 *
 */

#include "InputStateMachine.h"

#include <SelectionSystem/SelectionType.h>

#include "InputManager/RaycastStates/EmptySelection.h"
#include "RaycastStates/SinglePlayerSel.h"

namespace input {

void
InputStateMachine::createStates(void)
{
    // TODO: complete this
    mStates[IS_EMPTY_SEL].reset(new EmptySelection());
    mStates[IS_SINGLE_PLAYER_SEL].reset(new SinglePlayerSel());
}

////////////////////////////////////////////////////////////////////////////////
InputStateMachine::InputStateMachine(selection::SelectionManager &selManager) :
    mActualState(0)
,   mSelManager(selManager)
{
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
        return;
    }

    // else some selection is there, check for players
    mAuxVec.clear();
    mSelManager.getObjects(selection::Type::SEL_TYPE_PLAYER, mAuxVec);
    if (!mAuxVec.empty()) {
        // players selected..
        mActualState = (mAuxVec.size() == 1) ? mStates[IS_SINGLE_PLAYER_SEL].get()
                                             : mStates[IS_MULTI_PLAYER_SEL].get();
        return;
    }
    // else objects selected??
    ASSERT(false);


}



}
