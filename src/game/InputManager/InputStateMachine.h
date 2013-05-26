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
#include <Common/Math/CommonMath.h>
#include <Utils/MouseCursor/MouseCursor.h>

#include "IInputState.h"


// UGLY define of how much distance (factor) we want to separate the players
// once they are selected and need to be moved
#define ISM_SEPARATION_FACTOR       0.5f

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
    inline void
    showCursorMultiPlayer(selection::Type selType);

    // Helper functions to handle the current object with the raycasted object
    //
    void
    handleRaycastedObjSingle(selection::SelectableObject *raycastedObj);
    void
    handleRaycastedObjMulti(selection::SelectableObject *raycastedObj);

    // Helper method to calculate the offsets to be applied to the "destination
    // position" to obtain the 3 different players movement positions.
    //
    void
    calculateOffsets(void);

    // Helper method to calculate the destination locations for the selected
    // players given a pointInPath and using the mSavedPositions.
    // After calling this method, the mSavedPositions will contain the destination
    // positions of the selected players in the same order they are in the
    // mAuxVec
    //
    inline void
    calculateLocations(const Ogre::Vector3 &pointInPath);

    // Helper method to hide all the current billboards using the mAuxVec size
    // to iterate over them
    //
    inline void
    hideCurrentBillboards(void);

private:

    // Helper typedefs
    typedef std::vector<billboard::BillboardWrapper> BillboardVec;


	IInputState *mActualState;
	IInputStatePtr mStates[SIZE];
	selection::SelectionManager &mSelManager;
	std::vector<selection::SelectableObject *> mAuxVec;
	LevelManager *mLevelManager;
	InputManager &mInputManager;
    State mState;
    BillboardVec mMoveBillboards;
    std::vector<bool> mMoveBillbardVisible;
    Ogre::Vector3 mLastPointInPlane;

    // last saved "location" points for multi player selection
    std::vector<Ogre::Vector3> mSavedPositions;
    // and the PlayerOffsets used to separate the destination locations of the
    // players
    std::vector<sm::Vector2> mPlayerOffsets;

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
    case selection::Type::SEL_TYPE_LVL_OBJECT:
        // normal cursor
        mc.setCursor(MouseCursor::Cursor::NORMAL_CURSOR);
        break;
    case selection::Type::SEL_TYPE_ZOMBIE:
        mc.setCursor(MouseCursor::Cursor::ATTACK_CURSOR);
        break;
    case selection::Type::SEL_TYPE_COL_OBJECT:
        mc.setCursor(MouseCursor::Cursor::PICK_OBJECT_CURSOR);
        break;
    case selection::Type::SEL_TYPE_SPECIAL_ACTION:
        debugERROR("NOT IMPLEMENTED!! NEED TO PUT THE CURSOR HERE\n");
        break;
    case selection::Type::SEL_TYPE_BOMB:
        debugERROR("NOT IMPLEMENTED!! NEED TO PUT THE CURSOR HERE\n");
        break;
    default:
        ASSERT(false);
    }
}

inline void
InputStateMachine::showCursorMultiPlayer(selection::Type selType)
{
    ASSERT(!mAuxVec.empty());

    MouseCursor &mc = *GLOBAL_CURSOR;

    switch (selType) {
    case selection::Type::SEL_TYPE_PLAYER:
    case selection::Type::SEL_TYPE_NONE:
    case selection::Type::SEL_TYPE_CIVIL:
    case selection::Type::SEL_TYPE_LVL_OBJECT:
    case selection::Type::SEL_TYPE_COL_OBJECT: // NOTE we cannot pick objects?
    case selection::Type::SEL_TYPE_SPECIAL_ACTION: // NOTE we cannot do actions?
        // normal cursor
        mc.setCursor(MouseCursor::Cursor::NORMAL_CURSOR);
        break;
    case selection::Type::SEL_TYPE_ZOMBIE:
        mc.setCursor(MouseCursor::Cursor::ATTACK_CURSOR);
        break;
    default:
        ASSERT(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
inline void
InputStateMachine::calculateLocations(const Ogre::Vector3 &pointInPath)
{
    ASSERT(mAuxVec.size() == mSavedPositions.size());
    ASSERT(mAuxVec.size() == mPlayerOffsets.size());
    ASSERT(!mAuxVec.empty());

    for (size_t i = 0, size = mAuxVec.size(); i < size; ++i) {
        // calculate the new possible position of the player
        const sm::Vector2 newPos(pointInPath.x + mPlayerOffsets[i].x,
                                 pointInPath.z + mPlayerOffsets[i].y);
        if (mActualState->isPointInPath(newPos)) {
            // we have to update the mSavedPositions
            mSavedPositions[i].x = newPos.x;
            mSavedPositions[i].z = newPos.y;
        } // else we have 2 options:
          // 1) Let the last mSavedPosition like it is, and assume that was the
          //    the last "good" position we want.
          // 2) Put in the same place that pointInPath points to (note that this
          //    will implies that more than one player will be moving to the
          //    same place....)
          // For now we will let the last saved position and see whats happens
    }
}

////////////////////////////////////////////////////////////////////////////////
inline void
InputStateMachine::hideCurrentBillboards(void)
{
    for (size_t i = 0, size = mAuxVec.size(); i < size; ++i) {
        mMoveBillboards[i]->setPosition(0.f, -9999.f, 0.f);
        mMoveBillbardVisible[i] = false;
    }
}

}
#endif /* INPUTSTATEMACHINE_H_ */
