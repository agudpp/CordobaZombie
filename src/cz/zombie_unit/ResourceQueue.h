/*
 * ResourceQueue.h
 *
 *  Created on: Oct 22, 2013
 *      Author: agustin
 */

#ifndef RESOURCEQUEUE_H_
#define RESOURCEQUEUE_H_

#include <types/StackQueue.h>

namespace cz {


template<unsigned int SIZE, typename Type>
class ResourceQueue
{
public:
    ResourceQueue()
    {
        for (unsigned int i = 0; i < SIZE; ++i) {
            mQueue.push_back(&(mElements[i]));
        }
    }
    ~ResourceQueue() {}


    // @brief Check if we have more elements in the queue
    //
    inline bool
    empty(void) const
    {
        return mQueue.empty();
    }

    // @brief Check the last element in the queue without removing it.
    // @returns the associated element or 0 if no more elements.
    //
    inline Type*
    last(void)
    {
        if (empty()) return 0;
        return mQueue.back();
    }

    // @brief Get a new element from the queue and pop it up.
    //        If no more elements then return 0.
    //
    inline Type*
    getNewOne(void)
    {
        if (empty()) return 0;
        Type* e = mQueue.back();
        mQueue.pop_back();
        return e;
    }

    // @brief Let an element instance available again in the queue to be used later.
    // @param elem   The element we want to let available again, the element
    //               should belong to this queue (so elem should be obtained
    //               calling this->getNewOne())
    //
    inline void
    letAvailable(Type* elem)
    {
        ASSERT(elem);
        debugERROR("Check if elem belongs to this queue\n");
        mQueue.push_back(elem);
    }

protected:
    Type mElements[SIZE];
    core::StackQueue<Type*, SIZE> mQueue;
};

} /* namespace cz */
#endif /* RESOURCEQUEUE_H_ */
