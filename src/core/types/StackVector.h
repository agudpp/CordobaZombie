/*
 * StackVector.h
 *
 *  Created on: Aug 24, 2013
 *      Author: agustin
 */

#ifndef STACKVECTOR_H_
#define STACKVECTOR_H_

#include <cstring> // memcpy

#include <debug/DebugUtil.h>


namespace core {

// @brief Note that this vector will not call the destructor of the elements
//        once we remove them from the vector. This vector should be used
//        for basic (built in) types for example.
//        We will not call the constructor neither once we resize the vector.
//

template <typename T, unsigned int MAX_SIZE>
class StackVector
{
public:
    StackVector() : mSize(0) {};
    ~StackVector() {};

    // @brief Get the front element of the vector.
    // @returns the first element
    // @requires size > 0
    //
    inline T&
    front(void);
    inline const T&
    front(void) const;

    // @brief Get the back element (last) of the vector.
    // @return the last element of the vector
    // @requires size > 0
    //
    inline T&
    back(void);
    inline const T&
    back(void) const;

    // @brief Remove all the elements of the vector
    //
    inline void
    clear(void);

    // @brief Return the number of elements that this vector contain
    //
    inline unsigned int
    size(void) const;
    inline unsigned int
    max_size(void) const;

    // @brief Check if the vector is empty / full
    //
    inline bool
    empty(void) const;
    inline bool
    full(void) const;

    // @brief Resize the number of elements of the vector
    // @param size  The new size of the vector (size < MAX_SIZE).
    //
    inline void
    resize(unsigned int size);

    // @brief Get an element of the vector.
    // @param i     The index of the element we want (i < size)
    //
    inline T&
    operator[](unsigned int i);
    inline const T&
    operator[](unsigned int i) const;

    // @brief Remove the last element of the vector
    // @requires size > 0
    //
    inline void
    pop_back(void);

    // @brief Insert a new element in the back of this vector
    // @param element   The element we want to add in the last position of the
    //                  vector
    //
    inline void
    push_back(const T& element);

    // @brief Insert a element in a specific position BUT TAKE INTO ACCOUNT
    //        that this insertion will not maintain the order of the elements..
    //        Basically we will do: vec.push_back(vec[i]); vec[i] = element;
    // @param i         The position where we want to add the element (i < size).
    // @param element   The element we want to add at the i-th position.
    //
    inline void
    disorder_insert(unsigned int i, const T& element);

    // @brief Remove an element from an specific position BUT TAKE INTO ACCOUNT
    //        that the order will be modified. We will remove the i-th element
    //        but putting in that position the last one:
    //        vec[i] = vec.back(); vec.pop_back();
    // @param i         The position of the element we want to remove (i < size)
    //
    inline void
    disorder_remove(unsigned int i);

    // @brief Erase an element from the array maintaining the order
    // @param i         The i-th element we wanto to remove (i < size).
    //
    inline void
    erase(unsigned int i);

    // @brief Returns the pointers for the begin position and the end position
    //
    inline T*
    begin(void);
    inline const T*
    begin(void) const;
    inline T*
    end(void);
    inline const T*
    end(void) const;


    ////////////////////////////////////////////////////////////////////////////
    // Operators
    //
    inline StackVector&
    operator=(const StackVector<T, MAX_SIZE>& other);
    inline bool
    operator==(const StackVector<T, MAX_SIZE>& other) const;


private:
    T mData[MAX_SIZE];
    unsigned int mSize;
};




////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline T&
StackVector<T,MAX_SIZE>::front(void)
{
    ASSERT(mSize > 0);
    return mData[0];
}
template <typename T, unsigned int MAX_SIZE>
inline const T&
StackVector<T,MAX_SIZE>::front(void) const
{
    ASSERT(mSize > 0);
    return mData[0];
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline T&
StackVector<T,MAX_SIZE>::back(void)
{
    ASSERT(mSize > 0);
    return mData[mSize - 1];
}
template <typename T, unsigned int MAX_SIZE>
inline const T&
StackVector<T,MAX_SIZE>::back(void) const
{
    ASSERT(mSize > 0);
    return mData[mSize - 1];
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline void
StackVector<T,MAX_SIZE>::clear(void)
{
    mSize = 0;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline unsigned int
StackVector<T,MAX_SIZE>::size(void) const
{
    return mSize;
}
template <typename T, unsigned int MAX_SIZE>
inline unsigned int
StackVector<T,MAX_SIZE>::max_size(void) const
{
    return MAX_SIZE;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline bool
StackVector<T,MAX_SIZE>::empty(void) const
{
    return mSize == 0;
}
template <typename T, unsigned int MAX_SIZE>
inline bool
StackVector<T,MAX_SIZE>::full(void) const
{
    return mSize == MAX_SIZE;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline void
StackVector<T,MAX_SIZE>::resize(unsigned int size)
{
    ASSERT(size <= MAX_SIZE);
    mSize = size;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline T&
StackVector<T,MAX_SIZE>::operator[](unsigned int i)
{
    ASSERT(i < mSize);
    return mData[i];
}
template <typename T, unsigned int MAX_SIZE>
inline const T&
StackVector<T,MAX_SIZE>::operator[](unsigned int i) const
{
    ASSERT(i < mSize);
    return mData[i];
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline void
StackVector<T,MAX_SIZE>::pop_back(void)
{
    ASSERT(mSize > 0);
    --mSize;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline void
StackVector<T,MAX_SIZE>::push_back(const T& element)
{
    ASSERT(mSize < MAX_SIZE);
    mData[mSize++] = element;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline void
StackVector<T,MAX_SIZE>::disorder_insert(unsigned int i, const T& element)
{
    ASSERT(i < mSize);
    ASSERT(mSize < MAX_SIZE);
    push_back(mData[i]);
    mData[i] = element;
    ++mSize;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline void
StackVector<T,MAX_SIZE>::disorder_remove(unsigned int i)
{
    ASSERT(i < mSize);
    mData[i] = back();
    pop_back();
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline void
StackVector<T,MAX_SIZE>::erase(unsigned int i)
{
    ASSERT(i < mSize);
    //std::memcpy(mData + i, mData + i + 1, sizeof(T) * (mSize - i));
    T* beg = begin() + i, *last = end() - 1;
    while (beg != last) {
        *beg = *(beg+1);
        ++beg;
    }
    --mSize;
}

////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline T*
StackVector<T,MAX_SIZE>::begin(void)
{
    return &(mData[0]);
}
template <typename T, unsigned int MAX_SIZE>
inline const T*
StackVector<T,MAX_SIZE>::begin(void) const
{
    return &(mData[0]);
}

template <typename T, unsigned int MAX_SIZE>
inline T*
StackVector<T,MAX_SIZE>::end(void)
{
    return &mData[mSize]; // end + 1
}
template <typename T, unsigned int MAX_SIZE>
inline const T*
StackVector<T,MAX_SIZE>::end(void) const
{
    return &mData[mSize]; // end + 1
}


////////////////////////////////////////////////////////////////////////////////
template <typename T, unsigned int MAX_SIZE>
inline StackVector<T,MAX_SIZE>&
StackVector<T,MAX_SIZE>::operator=(const StackVector<T, MAX_SIZE>& other)
{
    clear();
    for (const T* b = other.begin(), *end = other.end(); b != end; ++b) {
        push_back(*b);
    }
}

template <typename T, unsigned int MAX_SIZE>
inline bool
StackVector<T,MAX_SIZE>::operator==(const StackVector<T, MAX_SIZE>& other) const
{
    if (size() != other.size()) {
        return false;
    }
    T* b1 = begin(), *b2 = other.begin(), *last = end();
    while (b1 != last) {
        if (*b1 != *b2) {
            return false;
        }
        ++b1; ++b2;
    }
    return true;
}

} /* namespace core */
#endif /* STACKVECTOR_H_ */
