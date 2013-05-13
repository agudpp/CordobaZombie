/*
 * InternalBackpack.cpp
 *
 *  Created on: May 4, 2013
 *      Author: agustin
 */

#include "InternalBackpack.h"

#include <Common/DebugUtil/DebugUtil.h>
#include <GameUnits/PlayerUnit/Backpack/Backpack.h>
#include <GameUnits/PlayerUnit/PlayerUnit.h>


namespace hud {


////////////////////////////////////////////////////////////////////////////////
void
InternalBackpack::loadElements(void)
{
    ASSERT(mPlayer);
    Backpack* backpack = mPlayer->getBackpack();
    ASSERT(backpack);

    // get all the elements of the backpack and construct them
    std::vector<BackpackItemPtr> items;
    backpack->getAllItems(items);

    // now using the backpackElementBuilder we will create all the backpack
    // elements we need
    mElements.reserve(items.size());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
InternalBackpack::InternalBackpack(PlayerUnit *pu, BackpackElementBuilder& builder) :
    mPlayer(pu)
,   mBuilder(builder)
{
    ASSERT(pu);
}

////////////////////////////////////////////////////////////////////////////////
InternalBackpack::~InternalBackpack()
{
    // TODO Auto-generated destructor stub
}


////////////////////////////////////////////////////////////////////////////////
void
InternalBackpack::hideAll(void);
void
InternalBackpack::showAll(void);

////////////////////////////////////////////////////////////////////////////////
void
InternalBackpack::backpackChange(Backpack*, const BackpackItemPtr&, Backpack::Event);


} /* namespace hud */


