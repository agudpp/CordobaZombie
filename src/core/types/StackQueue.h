/*
 * StackQueue.h
 *
 *  Created on: Aug 24, 2013
 *      Author: agustin
 */

#ifndef STACKQUEUE_H_
#define STACKQUEUE_H_


#include <debug/DebugUtil.h>


namespace core {

// @brief Note that this queue will not call the destructor of the elements
//        once we remove them from the queue. This queue should be used
//        for basic (built in) types for example.
//

template <typename T, unsigned int MAX_SIZE>
class StackQueue
{
public:
    StackQueue() : mBegin(0), mEnd(1), mSize(0) {};
    ~StackQueue() {};

    // @brief Get the size / max_size of the queue
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

    // @brief Get the front element of the queue
    // @returns the front element
    // @requires size > 0
    //
    inline T&
    front(void);
    inline const T&
    front(void) const;

    // @brief Return the last (back) element of the queue
    // @returns the last element of the queue.
    // @requires size > 0
    //
    inline T&
    back(void);
    inline const T&
    back(void) const;

    // @brief Push an element in the front of the queue
    // @param element   The element we want to push in the front of the queue
    // @requires !full()
    //
    inline void
    push_front(const T& element);

    // @brief Push an element in the back of the queue
    // @param element   The element we want to push in the back of the queue
    // @requires !full()
    //
    inline void
    push_back(const T& element);

    // @brief Remove an element front the front of the queue
    // @requires !empty()
    //
    inline void
    pop_front(void);

    // @brief Remove an element back the front of the queue
    // @requires !empty()
    //
    inline void
    pop_back(void);

    // @brief Clear the queue
    //
    inline void
    clear(void);

    // @brief Get an element in a specific position of the queue
    // @param i     The index of the position (i < size)
    // @returns the element in the i-th position
    //
    inline T&
    operator[](unsigned int i);
    inline const T&
    operator[](unsigned int i) const;

private:

    // @brief Helper functions to get the next index in the circular buffer or
    //        the previous one from a given index.
    // @param ci    The current index
    // @return the next/prev index
    //
    inline unsigned int
    next(unsigned int ci) const;
    inline unsigned int
    prev(unsigned int ci) const;

private:
    T mData[MAX_SIZE+1];
    unsigned int mBegin;
    unsigned int mEnd;
    unsigned int mSize;
};


////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline unsigned int
StackQueue<T, MAX_SIZE>::next(unsigned int ci) const
{
    ASSERT(ci <= MAX_SIZE); // real max size = MAX_SIZE + 1
    if (ci == MAX_SIZE) {
        return 0;
    } else {
        return ci + 1;
    }
}
template <typename T, unsigned int MAX_SIZE>
inline unsigned int
StackQueue<T, MAX_SIZE>::prev(unsigned int ci) const
{
    ASSERT(ci <= MAX_SIZE); // real max size = MAX_SIZE + 1
    if (ci == 0) {
        return MAX_SIZE;
    } else {
        return ci - 1;
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline unsigned int
StackQueue<T, MAX_SIZE>::size(void) const
{
    return mSize;
}
template <typename T, unsigned int MAX_SIZE>
inline unsigned int
StackQueue<T, MAX_SIZE>::max_size(void) const
{
    return MAX_SIZE;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline bool
StackQueue<T, MAX_SIZE>::empty(void) const
{
    return mSize == 0;
}
template <typename T, unsigned int MAX_SIZE>
inline bool
StackQueue<T, MAX_SIZE>::full(void) const
{
    return size() == MAX_SIZE;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline T&
StackQueue<T, MAX_SIZE>::front(void)
{
    ASSERT(!empty());
    return mData[next(mBegin)];
}
template <typename T, unsigned int MAX_SIZE>
inline const T&
StackQueue<T, MAX_SIZE>::front(void) const
{
    ASSERT(!empty());
    return mData[next(mBegin)];
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline T&
StackQueue<T, MAX_SIZE>::back(void)
{
    ASSERT(!empty());
    return mData[prev(mEnd)];
}
template <typename T, unsigned int MAX_SIZE>
inline const T&
StackQueue<T, MAX_SIZE>::back(void) const
{
    ASSERT(!empty());
    return mData[prev(mEnd)];
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline void
StackQueue<T, MAX_SIZE>::push_front(const T& element)
{
    ASSERT(!full());
    // set the element in the current begin pointer element
    mData[mBegin] = element;
    // decrement the mBegin pointer
    mBegin = prev(mBegin);
    ++mSize;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline void
StackQueue<T, MAX_SIZE>::push_back(const T& element)
{
    ASSERT(!full());
    mData[mEnd] = element;
    mEnd = next(mEnd);
    ++mSize;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline void
StackQueue<T, MAX_SIZE>::pop_front(void)
{
    ASSERT(!empty());
    mBegin = next(mBegin);
    --mSize;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline void
StackQueue<T, MAX_SIZE>::pop_back(void)
{
    ASSERT(!empty());
    mEnd = prev(mEnd);
    --mSize;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline void
StackQueue<T, MAX_SIZE>::clear(void)
{
    mBegin = 0;
    mEnd = 1;
    mSize = 0;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline T&
StackQueue<T, MAX_SIZE>::operator[](unsigned int i)
{
    ASSERT(i < mSize);
    return mData[(next(mBegin) + i) % (MAX_SIZE+1)];
}
template <typename T, unsigned int MAX_SIZE>
inline const T&
StackQueue<T, MAX_SIZE>::operator[](unsigned int i) const
{
    ASSERT(i < mSize);
    return mData[(next(mBegin) + i) % (MAX_SIZE+1)];
}

} /* namespace core */
#endif /* STACKQUEUE_H_ */
