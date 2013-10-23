/*
 * BodyPartQueue.h
 *
 *  Created on: Oct 22, 2013
 *      Author: agustin
 */

#ifndef BODYPARTQUEUE_H_
#define BODYPARTQUEUE_H_

#include "ResourceQueue.h"

namespace cz {

class BodyPartQueue
{
public:
    // define the number of queues we will handle, each one with different
    // objects
    //
    enum BodyPartObject {
        BPO_HEAD = 0,
        BPO_FORE_ARM,
        BPO_UPPER_ARM,
        BPO_UPPER_FORE_ARM,

        BPO_COUNT,
    };

    // we will define how many elements we want to have per queue.
    //
    static const unsigned int NUM_INSTANCES_PER_BPO = 10;
public:
    BodyPartQueue(){};
    ~BodyPartQueue(){};

private:
//    ResourceQueue<>
};

} /* namespace cz */
#endif /* BODYPARTQUEUE_H_ */
