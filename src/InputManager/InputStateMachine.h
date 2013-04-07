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

#include <OgreBillboard.h>

#include <BillboardManager/BillboardWrapper.h>
#include <SelectionSystem/SelectionData.h>
#include <SelectionSystem/SelectionType.h>
#include <SelectionSystem/SelectionManager.h>
#include <Common/GlobalObjects/GlobalObjects.h>
#include <Common/DebugUtil/DebugUtil.h>
#include <Utils/MouseCursor/MouseCursor.h>

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

    /**
     * @brief Auxiliary function used to show a overlay in a position on
     *        the map (over the pathfinding) if it is possible. If not, false
     *        is returned, if it is possible, true is returned.
     *        We will use the current IInputState to get the position in the
     *        map.
     * @returns true if we are showing an overlay over the map, false if not
     */
    bool
    tryToShowOverlaysOnMap(void);

    // Helper functions to handle mouse cursor for different states and object
    // types
    //
    inline void
    showCursorSinglePlayer(selection::Type selType);

    // Helper functions to handle the current object with the raycasted object
    //
    inline void
    handleRaycastedObjSingle(const selection::SelectableObject *raycastedObj);

private:
	IInputState *mActualState;
	IInputStatePtr mStates[SIZE];
	selection::SelectionManager &mSelManager;
	std::vector<selection::SelectableObject *> mAuxVec;
	LevelManager *mLevelManager;
	InputManager &mInputManager;
    State mState;
    billboard::BillboardWrapper mMoveSingleBillboard;
    bool mMoveBillbardVisible;

};


typedef boost::shared_ptr<InputStateMachine> InputStateMachinePtr;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline void
InputStateMachine::showCursorSinglePlayer(selection::Type selType)
{
    ASSERT(!mAuxVec.empty());

    MouseCursor &mc = *GLOBAL_CURSOR;

    switch (selType) {
    case selection::Type::SEL_TYPE_PLAYER:
    case selection::Type::SEL_TYPE_NONE:
    case selection::Type::SEL_TYPE_CIVIL:
        // normal cursor
        mc.setCursor(MouseCursor::Cursor::NORMAL_CURSOR);
        break;
    case selection::Type::SEL_TYPE_ZOMBIE:
        mc.setCursor(MouseCursor::Cursor::ATTACK_CURSOR);
        break;
    case selection::Type::SEL_TYPE_LVL_OBJECT:
        mc.setCursor(MouseCursor::Cursor::PICK_OBJECT_CURSOR);
        break;
    default:
        ASSERT(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
inline void
InputStateMachine::handleRaycastedObjSingle(const selection::SelectableObject *raycastedObj)
{
    ASSERT(raycastedObj);

    switch (raycastedObj->type()) {
    case selection::Type::SEL_TYPE_PLAYER:
    case selection::Type::SEL_TYPE_NONE:
    case selection::Type::SEL_TYPE_CIVIL:
        // Do nothing...
        break;
    case selection::Type::SEL_TYPE_ZOMBIE:
        // attack the zombie
        debugBLUE("ATTACKKKKKKK!!\n");
        break;
    case selection::Type::SEL_TYPE_LVL_OBJECT:
        // pick object!
        debugBLUE("PICKKKKINGGG object!\n");
        break;
    default:
        ASSERT(false);
    }
}

}
#endif /* INPUTSTATEMACHINE_H_ */
