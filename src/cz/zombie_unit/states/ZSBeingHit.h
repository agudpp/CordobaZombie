/*
 * ZSBeingHit.h
 *
 *  Created on: Nov 5, 2013
 *      Author: agustin
 */

#ifndef ZSBEINGHIT_H_
#define ZSBEINGHIT_H_


#include "ZombieState.h"


namespace cz {

class ZSBeingHit : public ZombieState
{
    // Hit value power (bullet power) needed to extirpate a body part
    //
    static const unsigned int EXTIRPATION_POWER_THRESHOLD = 333;

public:
    ZSBeingHit();
    virtual
    ~ZSBeingHit();

    // @brief Method called when when enter to this state
    //
    void
    enter(ZombieUnit*);

    // @brief Method called when when update this state
    //
    void
    update(ZombieUnit*);

    // @brief Method called when when exit this state
    //
    void
    exit(ZombieUnit*);

private:
    unsigned int mRunningAnim;
};

} /* namespace cz */
#endif /* ZSBEINGHIT_H_ */
