/*
 * BulletQueue.h
 *
 *  Created on: Sep 27, 2013
 *      Author: agustin
 */

#ifndef BULLETQUEUE_H_
#define BULLETQUEUE_H_

#include <types/StackQueue.h>
#include <debug/DebugUtil.h>

#include "Bullet.h"

namespace cz {

template <unsigned int NUM_BULLETS>
class BulletQueue
{
public:
    // The constructor will initializate the bullets setting this queue instance
    // as the parent queue
    //
    inline BulletQueue();
    inline ~BulletQueue();

    // @brief Let a bullet to be used again.
    // @param bullet        The bullet not used anymore.
    // @note THE BULLET SHOULD BELONG TO THIS CONTAINER
    //
    inline void
    letAvailable(Bullet* bullet);

    // @brief Get an available bullet to be used.
    // @return the available bullet, or 0 if no more bullets.
    // @note THIS CLASS IS THE OWNER OF THE MEMORY.
    //
    inline Bullet*
    getAvailable(void) const;

    // @brief Check if we have more bullets here
    //
    inline bool
    hasBullets(void) const;

    // @brief Count the number of allocated bullets we have.
    //
    inline unsigned int
    numAllocatedBullets(void) const;

    // @brief Return the array of bullets of size numAllocatedBullets();
    //
    inline Bullet*
    getBullets(void) const;

private:
    Bullet mBullets[NUM_BULLETS];
    core::StackQueue<Bullet*, NUM_BULLETS> mAvailableBullets;
};



////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

template <unsigned int NUM_BULLETS>
inline BulletQueue<NUM_BULLETS>::BulletQueue()
{
    for (unsigned int i = 0; i < NUM_BULLETS; ++i) {
        mBullets[i].setQueue(this);
        mAvailableBullets.push_back(mBullets[i]);
    }
}
template <unsigned int NUM_BULLETS>
inline BulletQueue<NUM_BULLETS>::~BulletQueue()
{

}

template <unsigned int NUM_BULLETS>
inline void
BulletQueue<NUM_BULLETS>::letAvailable(Bullet* bullet)
{
    ASSERT(!mAvailableBullets.full());
    ASSERT(bullet);
    debugERROR("TODO check here that bullet is one of the mBullet positions and"
        " is not in mAvailableBullets already\n");
    mAvailableBullets.push_back(bullet);
}

template <unsigned int NUM_BULLETS>
inline Bullet*
BulletQueue<NUM_BULLETS>::getAvailable(void) const
{
    if (mAvailableBullets.empty()) {
        return 0;
    }
    Bullet* b = mAvailableBullets.back();
    mAvailableBullets.pop_back();
    return b;
}

template <unsigned int NUM_BULLETS>
inline bool
BulletQueue<NUM_BULLETS>::hasBullets(void) const
{
    return !mAvailableBullets.empty();
}

template <unsigned int NUM_BULLETS>
inline unsigned int
BulletQueue<NUM_BULLETS>::numAllocatedBullets(void) const
{
    return NUM_BULLETS;
}

template <unsigned int NUM_BULLETS>
inline Bullet*
BulletQueue<NUM_BULLETS>::getBullets(void) const
{
    return mBullets;
}

} /* namespace cz */
#endif /* BULLETQUEUE_H_ */
