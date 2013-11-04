/*
 * ZombieEvent.h
 *
 *  Created on: Sep 10, 2013
 *      Author: agustin
 */

#ifndef ZOMBIEEVENT_H_
#define ZOMBIEEVENT_H_

namespace cz {

// define the possible Events
//
enum ZombieEvent {
    ZE_DONE = 0,
    ZE_RUN,         // run to the player
    ZE_DIE,         // when we die
    ZE_BEING_HIT,   // when we are being hit
    // no more events for now
};


}


#endif /* ZOMBIEEVENT_H_ */
