/*
 * InternalBackpack.cpp
 *
 *  Created on: May 4, 2013
 *      Author: agustin
 */

#include "InternalBackpack.h"

#include <boost/bind.hpp>

#include <debug/DebugUtil.h>
#include <GameUnits/PlayerUnit/Backpack/Backpack.h>
#include <GameUnits/PlayerUnit/PlayerUnit.h>

#include "BackpackElementBuilder.h"


namespace hud {


////////////////////////////////////////////////////////////////////////////////
void
InternalBackpack::loadElements(void)
{
    // get all the elements of the backpack and construct them
    std::vector<BackpackItem*> items;
    mPlayerBackpack.getAllItems(items);

    // now using the backpackElementBuilder we will create all the backpack
    // elements we need
    mElements.reserve(items.size() + 5); // just in case

    for (size_t i = 0, size = items.size(); i < size; ++i) {
        BackpackElementPtr element = mBuilder.buildElement(items[i], mPlayer);
        ASSERT(element.get() != 0);
        mElements.push_back(element);
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
InternalBackpack::getElementIndexFromItem(const BackpackItem* item, size_t& index) const
{
    for (size_t i = 0, size = mElements.size(); i < size; ++i) {
        if (mElements[i]->getBackpackItemPtr() == item) {
            index = i;
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
InternalBackpack::InternalBackpack(PlayerUnit *pu, BackpackElementBuilder& builder) :
    mPlayer(pu)
,   mBuilder(builder)
,   mPlayerBackpack(*mPlayer->getBackpack())
{
    ASSERT(pu);

    // load all the elements
    loadElements();

    // set the callback
    mBackpackUpdates = mPlayerBackpack.addCallback(
        boost::bind(&InternalBackpack::backpackChange, this, _1, _2));
}

////////////////////////////////////////////////////////////////////////////////
InternalBackpack::~InternalBackpack()
{
}


////////////////////////////////////////////////////////////////////////////////
void
InternalBackpack::hideAll(void)
{
    // hide all the elements
    for (size_t i = 0, size = mElements.size(); i < size; ++i) {
        mElements[i]->hide();
    }
}
void
InternalBackpack::showAll(void)
{
    for (size_t i = 0, size = mElements.size(); i < size; ++i) {
        BackpackElementPtr &element = mElements[i];
        element->show();

        // update the element if it is dirty
        if (element->isDirty()) {
            element->update();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void
InternalBackpack::backpackChange(Backpack* backpack,
                                 const BackpackItem* item,
                                 Backpack::Event event)
{
    // preconditions
    ASSERT(backpack == &mPlayerBackpack);

    switch (event) {
    case Backpack::Event::ITEM_ADDED:
    {
#ifdef DEBUG
        // ensure the element is not already in the backpack
        size_t dummy;
        ASSERT(!getElementIndexFromItem(item, dummy));
        ASSERT(mPlayerBackpack.hasBackpackItem(item));
#endif
        // we will add this element to the backpack
        BackpackElementPtr element = mBuilder.buildElement(item, mPlayer);
        if (!element.get()) {
            ASSERT(false); // ??? what happens here
            return;
        }
        mElements.push_back(element);

        // show the element and update it if necessary
        element->update();
        element->show();
    }
    break;
    case Backpack::Event::ITEM_REMOVED:
    {
#ifdef DEBUG
        // ensure the element is already in the backpack
        size_t dummy;
        ASSERT(getElementIndexFromItem(item, dummy));
        ASSERT(!mPlayerBackpack.hasBackpackItem(item));
#endif

        size_t currentIndex;
        if (!getElementIndexFromItem(item, currentIndex)) {
            ASSERT(false); // WTF!
            return;
        }

        // swap with the last
        ASSERT(currentIndex < mElements.size());
        mElements[currentIndex]->hide();
        mElements[currentIndex] = mElements.back();
        mElements.pop_back();
    }
    break;

    default:
        debugERROR("Case not implemented? %d \n", event);
        ASSERT(false);
    }
}


} /* namespace hud */


