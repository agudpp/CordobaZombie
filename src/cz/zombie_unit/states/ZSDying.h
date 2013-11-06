/*
 * ZSDying.h
 *
 *  Created on: Nov 5, 2013
 *      Author: agustin
 */

#ifndef ZSDYING_H_
#define ZSDYING_H_

#include "ZombieState.h"

namespace cz {

class ZSDying : public ZombieState
{
public:
    ZSDying();
    virtual
    ~ZSDying();

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
    bool mIsRagdollDead;
};

} /* namespace cz */
#endif /* ZSDYING_H_ */
