/*
 * BodyPartQueue.h
 *
 *  Created on: Oct 22, 2013
 *      Author: agustin
 */

#ifndef BODYPARTQUEUE_H_
#define BODYPARTQUEUE_H_

#include <types/StackVector.h>
#include <debug/DebugUtil.h>

#include "BodyPartElement.h"


namespace cz {

class BodyPartQueue
{
public:
    // we will define how many elements we want to have per queue.
    //
    static const unsigned int NUM_INSTANCES_PER_BPE = 10;
public:
    BodyPartQueue(){};
    ~BodyPartQueue(){};

    // @brief Add a new body part element to be tracked
    // @param bpe       The body part element
    // @note this class will not handle any memory.
    //
    inline void
    addBodyPartElement(BodyPartElement* bpe);

    // @brief Get a new one element of a particular type and id
    // @param type      The body part type we want
    // @param id        The id of the body part we want
    // @return the available BodyPartElement | 0 on error or if there are no
    //         more elements free
    // @note Do not free the element memory
    //
    inline BodyPartElement*
    getNewOne(BodyPartElementType type, unsigned short id);

    // @brief Return a free (available) element to the queue again
    // @param bpe       The BodyPartElement
    //
    inline void
    letAvailable(BodyPartElement* bpe);

private:
    typedef core::StackVector<BodyPartElement*, NUM_INSTANCES_PER_BPE> BodyPartElementQueue;
    BodyPartElementQueue mQueues[BodyPartElementType::BPE_COUNT];
};





////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
BodyPartQueue::addBodyPartElement(BodyPartElement* bpe)
{
    ASSERT(bpe);
    ASSERT(bpe->type < BodyPartElementType::BPE_COUNT);
    BodyPartElementQueue& queue = mQueues[bpe->type];
    queue.push_back(bpe);
}

inline BodyPartElement*
BodyPartQueue::getNewOne(BodyPartElementType type, unsigned short id)
{
    ASSERT(type < BodyPartElementType::BPE_COUNT);
    BodyPartElementQueue& queue = mQueues[type];

    // find for some element that matches with the id
    for (unsigned int i = 0; i < queue.size(); ++i) {
        BodyPartElement* elem = queue[i];
        if (elem->id == id) {
            // we have one
            queue.disorder_remove(i);
            return elem;
        }
    }
    // no luck, no  elements of that ID found.
    return 0;
}

inline void
BodyPartQueue::letAvailable(BodyPartElement* bpe)
{
    ASSERT(bpe);
    ASSERT(bpe->type < BodyPartElementType::BPE_COUNT);
    BodyPartElementQueue& queue = mQueues[bpe->type];
    queue.push_back(bpe);
}

} /* namespace cz */
#endif /* BODYPARTQUEUE_H_ */
