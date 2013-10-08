/*
 * BillboardGroupHandler.h
 *
 *  Created on: Oct 8, 2013
 *      Author: agustin
 */

#ifndef BILLBOARDGROUPHANDLER_H_
#define BILLBOARDGROUPHANDLER_H_

#include <OgreBillboard.h>

#include <set>

#include <types/StackVector.h>
#include <debug/DebugUtil.h>

#include "BillboardStack.h"
#include "UVCoordsDefs.h"


namespace cz {

// @brief This class will be like the manager of the billboards, it will group
//        the different kind of billboards into groups. And each we will be
//        able to get an specific billboard from an ID and a pointer to the
//        BillboardGroupHandler.
//

class BillboardGroupHandler
{
public:

    // define the maximum number of groups we can handle
    //
    static const unsigned int MAX_NUM_GROUPS = 20;

    // we will configure each group with different values:
    //
    struct GroupInfo {
        UVCoord coords;
        BillboardStack* stack;
    };

    typedef core::StackVector<GroupInfo, MAX_NUM_GROUPS> GroupVec;

public:
    BillboardGroupHandler() {};
    ~BillboardGroupHandler() {clear();};

    // @brief Add new group to be handled by this Handler
    // @param groupInfo     The group information. Note that once you add
    //                      this group it will be identified by the current index
    // @return index        The associated index (ID) of the grupo.
    // @note All the allocated BillboardStack are now owned by this class. We will
    //       free its memory when we destroy the class or call clear.
    //
    inline unsigned int
    addGroup(const GroupInfo& groupInfo);

    // @brief Remove all the groups.
    //
    inline void
    clear(void);

    // @brief Return the number of groups handled by this instance, and a pointer
    //        to them.
    //
    inline const GroupVec&
    groups(void) const;
    inline GroupVec&
    groups(void);

    ////////////////////////////////////////////////////////////////////////////
    // Helper methods to get the billboards with different configurations
    //
    // Common params:
    // @param gindex        The group index (should be valid).
    // @param pos           The position where we want to put the billboard
    // @param dvec          Direction vector of the billboard
    //
    // Returns:
    // @return billboard | 0 on error.
    //

    inline Ogre::Billboard*
    get(unsigned int gindex);
    inline Ogre::Billboard*
    get(unsigned int gindex, const Ogre::Vector3& pos);
    inline Ogre::Billboard*
    get(unsigned int gindex, const Ogre::Vector3& pos, const Ogre::Vector3& dvec);


private:
    // avoid copying
    BillboardGroupHandler(const BillboardGroupHandler&);
    BillboardGroupHandler& operator=(const BillboardGroupHandler&);

private:
    GroupVec mGroups;
};




////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline unsigned int
BillboardGroupHandler::addGroup(const GroupInfo& groupInfo)
{
    ASSERT(groupInfo.stack);
    mGroups.push_back(groupInfo);
    return mGroups.size() - 1;
}

inline void
BillboardGroupHandler::clear(void)
{
    // we need to check and ensure that we are not releasing twice the same
    // BillboardStack, for that we will use an ugly set
    // TODO: replace with a own set
    std::set<BillboardStack*> stacks;

    for (GroupInfo& gi : mGroups) {
        stacks.insert(gi.stack);
    }
    for (auto it = stacks.begin(), end = stacks.end(); it != end; ++it) {
        delete *it;
    }
    mGroups.clear();
}

inline const BillboardGroupHandler::GroupVec&
BillboardGroupHandler::groups(void) const
{
    return mGroups;
}
inline BillboardGroupHandler::GroupVec&
BillboardGroupHandler::groups(void)
{
    return mGroups;
}


inline Ogre::Billboard*
BillboardGroupHandler::get(unsigned int gindex)
{
    GroupInfo& info = mGroups[gindex];
    Ogre::Billboard* b = info.stack->getBillboard(info.coords);
    return b;
}
inline Ogre::Billboard*
BillboardGroupHandler::get(unsigned int gindex, const Ogre::Vector3& pos)
{
    GroupInfo& info = mGroups[gindex];
    Ogre::Billboard* b = info.stack->getBillboard(info.coords, pos);
    return b;
}
inline Ogre::Billboard*
BillboardGroupHandler::get(unsigned int gindex,
                           const Ogre::Vector3& pos,
                           const Ogre::Vector3& dvec)
{
    Ogre::Billboard* b = get(gindex, pos);
    if (b == 0) {
        return b;
    }
    b->mDirection = dvec;
    return b;
}

} /* namespace cz */
#endif /* BILLBOARDGROUPHANDLER_H_ */
