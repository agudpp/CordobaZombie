/*
 * FiringSystemHandler.h
 *
 *  Created on: Oct 21, 2013
 *      Author: agustin
 */

#ifndef FIRINGSYSTEMHANDLER_H_
#define FIRINGSYSTEMHANDLER_H_

#include <vector>

#include <debug/DebugUtil.h>

#include "Bullet.h"

// forward
//
namespace physics {
class DynamicWorld;
}

namespace cz {

class FiringSystemHandler
{
public:
    FiringSystemHandler();
    ~FiringSystemHandler();

    // @brief Set the Dynamic world instance to be used to perform the raycasts
    // @param dw        The DynamicWorld instance
    //
    inline void
    setInfo(physics::DynamicWorld* dw);


    // @brief Add a new bullet to be updated / tracked.
    // @param bullet        The bullet
    //
    inline void
    add(Bullet* bullet);

    // @brief Remove a bullet from the list of active bullets, and disable it.
    // @param bullet        The bullet we want to remove.
    // @note This method is slow, we will iterate over all the instances for
    //       now.
    //
    inline void
    remove(const Bullet* bullet);

    // @brief If we already contain a bullet in the list (slow).
    // @param bullet        The bullet we want to check
    //
    inline bool
    contains(const Bullet* bullet) const;

    // @brief Update all the logic of all the bullets. This implies move the
    //        bullets, check for collision detection, remove bullets from scene,
    //        etc.
    //
    void
    update(void);

private:

    // @brief Return the index of a particular bullet
    // @param bullet    Bullet to get its index
    // @return index | -1 if not exists;
    //
    inline int
    getIndex(const Bullet* bullet) const;

private:
    physics::DynamicWorld* mDynamicWorld;
    std::vector<Bullet*> mBullets;
};




////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline int
FiringSystemHandler::getIndex(const Bullet* bullet) const
{
    for (unsigned int i = 0, size = mBullets.size(); i < size; ++i) {
        if (mBullets[i] == bullet) {
            return i;
        }
    }
    return -1;
}

inline void
FiringSystemHandler::setInfo(physics::DynamicWorld* dw)
{
    ASSERT(dw);
    mDynamicWorld = dw;
}

inline void
FiringSystemHandler::add(Bullet* bullet)
{
    ASSERT(getIndex(bullet) < 0);
    mBullets.push_back(bullet);
}

inline void
FiringSystemHandler::remove(const Bullet* bullet)
{
    int index = getIndex(bullet);
    if (index < 0) {
        debugWARNING("Trying to remove a bullet that doesn't exists?\n");
        return;
    }
    mBullets[index]->disable();
    mBullets[index] = mBullets.back();
    mBullets.pop_back();
}

inline bool
FiringSystemHandler::contains(const Bullet* bullet) const
{
    return getIndex(bullet) >= 0;
}

} /* namespace cz */
#endif /* FIRINGSYSTEMHANDLER_H_ */
