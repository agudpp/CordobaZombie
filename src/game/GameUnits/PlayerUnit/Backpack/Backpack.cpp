/*
 * Backpack.cpp
 *
 *  Created on: 14/05/2012
 *      Author: agustin
 */

#include <OgreOverlayManager.h>
#include <OgreStringConverter.h>

#include <auto_ptr.h>

#include "GUIHelper.h"
#include "GlobalObjects.h"
#include "DebugUtil.h"
#include "Backpack.h"
#include "Util.h"


// Auxiliary methods
//
namespace {

// swap and remove an element
//
template<typename VecType, typename T>
inline T
swapAndRemove(VecType& vec, const T elem)
{
    for (size_t i = 0, size = vec.size(); i < size; ++i) {
        if (vec[i] == elem) {
            T tmp = vec[i];
            vec[i] = vec.back();
            vec.pop_back();
            return tmp;
        }
    }

    return 0;
}

}


////////////////////////////////////////////////////////////////////////////////
void
Backpack::addBackpackItem(BackpackItem* bi)
{
    ASSERT(!hasBackpackItem(bi));
    ASSERT(bi->type < BackpackDef::ItemType::COUNT);
    mItems[bi->type].push_back(bi);

    // trigger the signal
    mSignal(this, bi, Event::ITEM_ADDED);
}

////////////////////////////////////////////////////////////////////////////////
void
Backpack::removeBackpackItem(const BackpackItem* bi)
{
    ASSERT(hasBackpackItem(bi));
    ASSERT(bi->type < BackpackDef::ItemType::COUNT);

    const BackpackItem* tmp = swapAndRemove(mItems[bi->type], bi);

    // trigger the signal
    mSignal(this, tmp, Event::ITEM_REMOVED);
}

void
Backpack::removeBackpackItemUserDef(void *bi)
{
    // we need to search for that element
    for (size_t i = 0; i < BackpackDef::ItemType::COUNT; ++i) {
        ItemVec& vec = mItems[i];
        for (size_t j = 0, size = vec.size(); j < size; ++j) {
            ASSERT(vec[j]);
            if (vec[j]->userDef == bi) {
                // we need to remove this element
                const BackpackItem* tmp = vec[j];
                vec[j] = vec.back();
                vec.pop_back();

                // emmit the event
                mSignal(this, tmp, Event::ITEM_REMOVED);
                return;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
Backpack::hasBackpackItem(const BackpackItem* bi) const
{
    if (bi == 0 || bi->type >= BackpackDef::ItemType::COUNT) {
        return false;
    }

    // check if the element is in the corresponding vector
    const ItemVec& vec = mItems[bi->type];
    for (size_t i = 0, size = vec.size(); i < size; ++i) {
        if (vec[i] == bi) {
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
BackpackItem*
Backpack::hasBackpackItemUserDef(void *ud) const
{
    // we need to search for that element
    for (size_t i = 0; i < BackpackDef::ItemType::COUNT; ++i) {
        const ItemVec& vec = mItems[i];
        for (size_t j = 0, size = vec.size(); j < size; ++j) {
            ASSERT(vec[j]);
            if (vec[j]->userDef == ud) {
                return vec[j];
            }
        }
    }

    // nothing found
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
Backpack::Connection
Backpack::addCallback(const Signal::slot_type& subscriber)
{
    return mSignal.connect(subscriber);
}

////////////////////////////////////////////////////////////////////////////////
void
Backpack::getAllItems(std::vector<BackpackItem*>& items)
{
    size_t accum = 1;
    for (size_t i = 0; i < BackpackDef::ItemType::COUNT; ++i) {
        accum += mItems[i].size();
    }
    items.clear();
    items.reserve(accum);
    for (size_t i = 0; i < BackpackDef::ItemType::COUNT; ++i) {
        items.insert(items.end(), mItems[i].begin(), mItems[i].end());
    }
}



