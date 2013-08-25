/*
 * StackPriorityQueue.h
 *
 *  Created on: Aug 24, 2013
 *      Author: agustin
 */

#ifndef STACKPRIORITYQUEUE_H_
#define STACKPRIORITYQUEUE_H_

#include <algorithm>
#include <functional>

#include "StackVector.h"

namespace core {

// @brief A class that will try to maintain all the elements of the queue
//        sorted using the stack as allocator.
//        We will use the StackVector as the base and also the heap_algorithms
//        from the stl. We will change this later if we need to improve
//        performance.
//        Also, note that the constructor / destructor are not called when
//        inserting / removing an element, so try to use this class with
//        built in types.
//


template <typename T, unsigned int MAX_SIZE, typename Comp = std::less<T> >
class StackPriorityQueue
{
public:
    StackPriorityQueue(){};
    ~StackPriorityQueue(){};

    // @brief Get the size / max_size of the queue
    // @return the size
    //
    inline unsigned int
    size(void) const;
    inline unsigned int
    max_size(void) const;

    // @brief Check if the queue is empty/full
    //
    inline bool
    empty(void) const;
    inline bool
    full(void) const;

    // @brief Push an element into the queue
    // @param element   The element to be added in the queue
    // @requires !full()
    //
    inline void
    push(const T& element);

    // @brief Get the top element (with the high priority)
    // @requires !empty()
    //
    inline T&
    top(void);
    inline const T&
    top(void) const;

    // @brief Remove the top element
    // @requires !empty()
    //
    inline void
    pop(void);

    // @brief Clear the queue
    //
    inline void
    clear(void);

private:
    StackVector<T, MAX_SIZE> mImpl;
};





////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE, typename Comp>
inline unsigned int
StackPriorityQueue<T, MAX_SIZE, Comp>::size(void) const
{
    return mImpl.size();
}
template <typename T, unsigned int MAX_SIZE, typename Comp>
inline unsigned int
StackPriorityQueue<T, MAX_SIZE, Comp>::max_size(void) const
{
    return mImpl.max_size();
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE, typename Comp>
inline bool
StackPriorityQueue<T, MAX_SIZE, Comp>::empty(void) const
{
    return mImpl.empty();
}
template <typename T, unsigned int MAX_SIZE, typename Comp>
inline bool
StackPriorityQueue<T, MAX_SIZE, Comp>::full(void) const
{
    return mImpl.full();
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE, typename Comp>
inline void
StackPriorityQueue<T, MAX_SIZE, Comp>::push(const T& element)
{
    ASSERT(!full());
    mImpl.push_back(element);
    std::push_heap(mImpl.begin(), mImpl.end(), Comp());
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE, typename Comp>
inline T&
StackPriorityQueue<T, MAX_SIZE, Comp>::top(void)
{
    ASSERT(!empty());
    return mImpl.front();
}
template <typename T, unsigned int MAX_SIZE, typename Comp>
inline const T&
StackPriorityQueue<T, MAX_SIZE, Comp>::top(void) const
{
    ASSERT(!empty());
    return mImpl.front();
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE, typename Comp>
inline void
StackPriorityQueue<T, MAX_SIZE, Comp>::pop(void)
{
    ASSERT(!empty());
    std::pop_heap(mImpl.begin(), mImpl.end(), Comp());
    mImpl.pop_back();
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE, typename Comp>
inline void
StackPriorityQueue<T, MAX_SIZE, Comp>::clear(void)
{
    mImpl.clear();
}

} /* namespace core */
#endif /* STACKPRIORITYQUEUE_H_ */
