/*
 * ZSRunning.h
 *
 *  Created on: Sep 9, 2013
 *      Author: agustin
 */

#ifndef ZSRUNNING_H_
#define ZSRUNNING_H_

#include "ZombieState.h"

namespace cz {

class ZombieUnit;

class ZSRunning : public ZombieState
{
public:
    ZSRunning(){};
    virtual
    ~ZSRunning(){};

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
};

} /* namespace cz */
#endif /* ZSRUNNING_H_ */
