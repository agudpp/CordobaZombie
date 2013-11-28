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

class BulletQueue
{
    static const unsigned int NUM_BULLETS = 35;

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
    getAvailable(void);

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
    getBullets(void);
    inline const Bullet*
    getBullets(void) const;

    // @brief Create all the bullets
    //
    inline void
    build(void);

private:
    Bullet mBullets[NUM_BULLETS];
    core::StackQueue<Bullet*, NUM_BULLETS> mAvailableBullets;
};



////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline BulletQueue::BulletQueue()
{
    for (unsigned int i = 0; i < NUM_BULLETS; ++i) {
        mBullets[i].setQueue(this);
        mAvailableBullets.push_back(&(mBullets[i]));
    }
}

inline BulletQueue::~BulletQueue()
{

}

inline void
BulletQueue::letAvailable(Bullet* bullet)
{
    ASSERT(!mAvailableBullets.full());
    ASSERT(bullet);
    debugERROR("TODO check here that bullet is one of the mBullet positions and"
        " is not in mAvailableBullets already\n");
    mAvailableBullets.push_back(bullet);
}

inline Bullet*
BulletQueue::getAvailable(void)
{
    if (mAvailableBullets.empty()) {
        return 0;
    }
    Bullet* b = mAvailableBullets.back();
    mAvailableBullets.pop_back();
    return b;
}

inline bool
BulletQueue::hasBullets(void) const
{
    return !mAvailableBullets.empty();
}

inline unsigned int
BulletQueue::numAllocatedBullets(void) const
{
    return NUM_BULLETS;
}

inline Bullet*
BulletQueue::getBullets(void)
{
    return mBullets;
}
inline const Bullet*
BulletQueue::getBullets(void) const
{
    return mBullets;
}

inline void
BulletQueue::build(void)
{
    for (Bullet& b : mBullets) {
        b.create();
    }
}

} /* namespace cz */
#endif /* BULLETQUEUE_H_ */
