/*
 * BoolCountingMask.h
 *
 *  Created on: Aug 24, 2013
 *      Author: agustin
 */

#ifndef BOOLCOUNTINGMASK_H_
#define BOOLCOUNTINGMASK_H_

#include <vector>

#include <debug/DebugUtil.h>

#include "basics.h"


namespace core {

// @brief This class will be used as a bitset or boolean mask but with the
//        feature that we will not need to reinitialize everytime, we only
//        need to call newRound and that way we will be initializing the
//        boolean mask in O(1).
//        It is more memory expensive but fast in runtime (we probably can
//        do some performance tests
//

template <typename T = core::uint16_t>
class BoolCountingMask
{
public:
    BoolCountingMask() : mCurrentRound(0) {};
    ~BoolCountingMask() {};

    // @brief Set the size of the mask
    // @param size  The new size of the mask
    //
    inline void
    setSize(unsigned int size);

    // @brief Set a new round. This method will reinitialize the mask.
    //
    inline void
    newRound(void);

    // @brief Check if particular position is marked or not
    // @param i     The index position (i < size);
    // @return true if it is marked | false otherwise
    //
    inline bool
    isMarked(unsigned int i) const;
    inline bool
    operator[](unsigned int i) const;

    // @brief Mark a specific position (as true).
    // @param i     The index position we want to mark (i < size).
    //
    inline void
    mark(unsigned int i);

    // @brief Unmark a specific position (as false)
    // @param i     The index position we want to "un-mark" (i < size)
    //
    inline void
    unmark(unsigned int i);

private:
    std::vector<T> mMask;
    T mCurrentRound;
};



////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

template <typename T>
inline void
BoolCountingMask<T>::setSize(unsigned int size)
{
    mCurrentRound = 0;
    mMask.resize(size);
    for (unsigned int i = 0; i < size; ++i) {
        unmark(i); // unmark all the values
    }
}

template <typename T>
inline void
BoolCountingMask<T>::newRound(void)
{
    ++mCurrentRound;
}

template <typename T>
inline bool
BoolCountingMask<T>::isMarked(unsigned int i) const
{
    ASSERT(i < mMask.size());
    return mMask[i] == mCurrentRound;
}
template <typename T>
inline bool
BoolCountingMask<T>::operator[](unsigned int i) const
{
    ASSERT(i < mMask.size());
    return mMask[i] == mCurrentRound;
}

template <typename T>
inline void
BoolCountingMask<T>::mark(unsigned int i)
{
    ASSERT(i < mMask.size());
    mMask[i] = mCurrentRound;
}

template <typename T>
inline void
BoolCountingMask<T>::unmark(unsigned int i)
{
    ASSERT(i < mMask.size());
    mMask[i] = mCurrentRound - 1;
}

} /* namespace core */
#endif /* BOOLCOUNTINGMASK_H_ */
