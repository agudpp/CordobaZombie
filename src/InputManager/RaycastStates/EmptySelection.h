/*
 * EmptySelection.h
 *
 *  Created on: Mar 3, 2013
 *      Author: agustin
 */

#ifndef EMPTYSELECTION_H_
#define EMPTYSELECTION_H_

#include <InputManager/IInputState.h>
#include <CollisionSystem/CollisionTypedefs.h>

namespace selection {
class SelectionManager;
class SelectionData;
}

namespace input {

class EmptySelection : public IInputState
{
    // define the mask we want to use to perform the raycast
    static const uint32 RAYCAST_MASK =  COL_FLAG_UNIT_PLAYER    |
                                        COL_FLAG_UNIT_ZOMBIE    |
                                        COL_FLAG_UNIT_CIVIL     |
                                        COL_FLAG_LEVEL_INFO_OBJ |
                                        COL_FLAG_COLLECTABLE_OBJ;
public:
    EmptySelection();

    virtual
    ~EmptySelection();

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
};

} /* namespace input */
#endif /* EMPTYSELECTION_H_ */
