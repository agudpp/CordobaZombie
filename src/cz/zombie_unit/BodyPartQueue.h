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

    // the vector we will use for the elements
    typedef core::StackVector<BodyPartElement,
                                  NUM_INSTANCES_PER_BPE *
                                      BodyPartElementType::BPE_COUNT> BodyPartElementVec;
public:
    BodyPartQueue(){};
    ~BodyPartQueue(){};

    // @brief Return the associated vector of the bodypart elements.
    //
    inline BodyPartElementVec&
    bodyPartElements(void);

    // @brief You must call this method after you set all the bodypart elements
    //        in the bodyPartElementVect of this class. After that, when calling
    //        this method we will build the queue and set all the elements.
    //
    inline void
    build(void);

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

    // @brief Check if the queue contains elements.
    //
    inline bool
    isEmpty(void) const;

    // @brief Clear the queue.
    //
    inline void
    clear(void);

private:
    typedef core::StackVector<BodyPartElement*, NUM_INSTANCES_PER_BPE> BodyPartElementQueue;

    BodyPartElementVec mElements;
    BodyPartElementQueue mQueues[BodyPartElementType::BPE_COUNT];
};





////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline BodyPartQueue::BodyPartElementVec&
BodyPartQueue::bodyPartElements(void)
{
    return mElements;
}

inline void
BodyPartQueue::build(void)
{
    for (BodyPartElement& bpe : mElements) {
        ASSERT(bpe.bodyPartElementType() < BodyPartElementType::BPE_COUNT);
        BodyPartElementQueue& queue = mQueues[bpe.bodyPartElementType()];
        queue.push_back(&bpe);
    }
}

inline BodyPartElement*
BodyPartQueue::getNewOne(BodyPartElementType type, unsigned short id)
{
    ASSERT(type < BodyPartElementType::BPE_COUNT);
    BodyPartElementQueue& queue = mQueues[type];

    // find for some element that matches with the id
    for (unsigned int i = 0; i < queue.size(); ++i) {
        BodyPartElement* elem = queue[i];
        if (elem->ID() == id) {
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
    ASSERT(bpe->bodyPartElementType() < BodyPartElementType::BPE_COUNT);
    BodyPartElementQueue& queue = mQueues[bpe->bodyPartElementType()];
    queue.push_back(bpe);
}

inline bool
BodyPartQueue::isEmpty(void) const
{
    return mElements.empty();
}

inline void
BodyPartQueue::clear(void)
{
    for (unsigned int i = 0; i < NUM_INSTANCES_PER_BPE; ++i) {
        mQueues[i].clear();
    }
}

} /* namespace cz */
#endif /* BODYPARTQUEUE_H_ */
