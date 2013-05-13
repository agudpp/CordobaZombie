/* This class will contain all the elements associated to a particular player
 * and will be updated when the elements of that player's backpack change
 *
 * InternalBackpack.h
 *
 *  Created on: May 4, 2013
 *      Author: agustin
 */

#ifndef INTERNALBACKPACK_H_
#define INTERNALBACKPACK_H_

#include <vector>

#include <GameUnits/PlayerUnit/Backpack/Backpack.h>

#include "BackpackElement.h"

// Forward
//
class PlayerUnit;

namespace hud {

// forward
//
class BackpackElementBuilder;

class InternalBackpack
{

    typedef std::vector<BackpackElementPtr> ElementVec;

public:
    InternalBackpack(PlayerUnit *pu, BackpackElementBuilder& builder);
    ~InternalBackpack();

    /**
     * @brief Get the player unit associated to this backpack representation
     */
    inline const PlayerUnit*
    playerUnit(void) const;
    inline void
    setPlayerUnit(PlayerUnit* pu);

    /**
     * @brief Hide/show all the elements of this backpack
     */
    void
    hideAll(void);
    void
    showAll(void);

    // Callbacks handling
    //

    /**
     * @brief This method is a callback for when the backpack changes
     */
    void
    backpackChange(Backpack*, const BackpackItem*, Backpack::Event);

private:

    /**
     * @brief Generate all the needed elements from a player backpack using
     *        the current player
     */
    void
    loadElements(void);

private:
    PlayerUnit *mPlayer;
    Backpack &mPlayerBackpack;
    Backpack::Connection mBackpackUpdates;
    ElementVec mElements;
    BackpackElementBuilder& mBuilder;
};



// Inline impl
//
inline const PlayerUnit*
InternalBackpack::playerUnit(void) const
{
    return mPlayer;
}
inline void
InternalBackpack::setPlayerUnit(PlayerUnit* pu)
{
    mPlayer = pu;
}

} /* namespace hud */
#endif /* INTERNALBACKPACK_H_ */
