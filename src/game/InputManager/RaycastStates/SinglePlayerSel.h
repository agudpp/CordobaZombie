/*
 * SinglePlayerSel.h
 *
 *  Created on: Mar 3, 2013
 *      Author: agustin
 */

#ifndef SINGLEPLAYERSEL_H_
#define SINGLEPLAYERSEL_H_

#include <vector>

#include <InputManager/IInputState.h>
#include <collision/CollisionTypedefs.h>


namespace selection {
class SelectionManager;
class SelectionData;
class SelectableObject;
}

namespace input {

class SinglePlayerSel : public IInputState
{
    // define the mask we want to use to perform the raycast
    static const uint32 RAYCAST_MASK =  c_p::COL_FLAG_UNIT_PLAYER    |
                                        c_p::COL_FLAG_UNIT_ZOMBIE    |
                                        c_p::COL_FLAG_UNIT_CIVIL     |
                                        c_p::COL_FLAG_LEVEL_INFO_OBJ |
                                        c_p::COL_FLAG_COLLECTABLE_OBJ;
public:
    SinglePlayerSel();

    virtual
    ~SinglePlayerSel();

    /**
     * @brief Configure the State using the current selection data
     * @param selData   The current selection data
     */
    virtual void
    configure(const selection::SelectionData &selData);

    /**
     * @brief Perform the needed raycast depending on the last selection.
     *        This function will show all the neccessary stuff in the level,
     *        select whatever is needed, and so forth.
     *        We will use the GLOBAL_CURSOR position to generate the RayCast
     */
    virtual void
    executeRayCast(void);

private:
    typedef std::vector<selection::SelectableObject *> SelectedObjVec;

    SelectedObjVec mPlayers;
};

} /* namespace input */
#endif /* SINGLEPLAYERSEL_H_ */
