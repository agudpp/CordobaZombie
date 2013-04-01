/* Class that will handle the selection and the main input system (select a
 * player, move players, select objects, etc)
 *
 * InputStateMachine.h
 *
 *  Created on: 04/08/2012
 *      Author: agustin
 *
 */

#ifndef INPUTSTATEMACHINE_H_
#define INPUTSTATEMACHINE_H_

#include <vector>

#include <boost/shared_ptr.hpp>

#include <SelectionSystem/SelectionData.h>
#include <SelectionSystem/SelectionManager.h>

#include "IInputState.h"


// Forward decl
//
namespace selection {
class SelectableObject;
class SelectionManager;
}

namespace input {
class InputManager;
}

class LevelManager;


namespace input {


class InputStateMachine {

    // the possible states
    enum State {
        IS_EMPTY_SEL = 0,
        IS_MULTI_PLAYER_SEL,
        IS_OBJECT_SEL,
        IS_SINGLE_PLAYER_SEL,
        SIZE
    };


public:
	InputStateMachine(selection::SelectionManager &selManager,
	                  LevelManager *lvlManager,
	                  InputManager &inputManager);
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
     * @brief Function called to update the logic of the InputManager
     */
    void
    update(void);

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
	LevelManager *mLevelManager;
	InputManager &mInputManager;
    State mState;

};


typedef boost::shared_ptr<InputStateMachine> InputStateMachinePtr;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



}
#endif /* INPUTSTATEMACHINE_H_ */
