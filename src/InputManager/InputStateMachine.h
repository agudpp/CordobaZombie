/*
 * InputStateMachine.h
 *
 *  Created on: 04/08/2012
 *      Author: agustin
 *
 */

#ifndef INPUTSTATEMACHINE_H_
#define INPUTSTATEMACHINE_H_

#include <vector>

#include <SelectionSystem/SelectionData.h>
#include <SelectionSystem/SelectionManager.h>

#include "DebugUtil.h"
#include "IInputState.h"


// Forward decl
//
namespace selection {
class SelectableObject;
}

namespace input {


class InputStateMachine {

    enum {
        IS_EMPTY_SEL = 0,
        IS_MULTI_PLAYER_SEL,
        IS_OBJECT_SEL,
        IS_SINGLE_PLAYER_SEL,
        SIZE
    };

public:
	InputStateMachine(selection::SelectionManager &selManager);
	~InputStateMachine();

    /**
     * @brief Function called when a selection has changed. This will configure
     *        the state machine to perform the correct ray cast selection the
     *        adecuated state.
     * @param selData   The selection data used to re-configure the state machine
     */
    void
    selectionChanged(const selection::SelectionData &selData);

    /**
     * @brief Perform the needed raycast depending on the last selection.
     *        This function will show all the neccessary stuff in the level,
     *        select whatever is needed, and so forth.
     *        We will use the GLOBAL_CURSOR position to generate the RayCast
     */
    inline void
    executeRayCast(void);

    /**
     * @brief Returns the last raycasted object
     * @returns the last raycasted object for any of the InputStates
     */
    inline selection::SelectableObject *
    lastRaycastedObj(void);


private:

    /**
     * @brief Creates the states used by this class
     */
    void
    createStates(void);

private:
	IInputState *mActualState;
	IInputStatePtr mStates[SIZE];
	selection::SelectionManager &mSelManager;
	std::vector<selection::SelectableObject *> mAuxVec;

};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
inline void
InputStateMachine::executeRayCast(void)
{
    ASSERT(mActualState != 0);
    mActualState->executeRayCast();
}

////////////////////////////////////////////////////////////////////////////////
inline selection::SelectableObject *
InputStateMachine::lastRaycastedObj(void)
{
    return mActualState->lastRaycastedObj();
}

}
#endif /* INPUTSTATEMACHINE_H_ */
