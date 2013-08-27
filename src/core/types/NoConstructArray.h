/*
 * NoConstructArray.h
 *
 *  Created on: Aug 26, 2013
 *      Author: agustin
 */

#ifndef NOCONSTRUCTARRAY_H_
#define NOCONSTRUCTARRAY_H_

#include <array>

namespace core {

// @brief This class will be used to create an array of objects of type T
//        without initialize them nor destruct them. This is very dangerous
//        if you use it with objects that should be constructed (or had vtable).
//        So use this with the basic types only.
//

template <typename T, unsigned int SIZE>
class NoConstructArray
{
public:
    // @brief We will only implement the operator[] since is the only thing
    //        we can do to an array.
    // @param i     The index position
    // @return a reference to that element
    //
    inline T&
    operator[](unsigned int i);
    inline const T&
    operator[](unsigned int i) const;

    // @brief Return the pointers to the begin and end of the array.
    //        Use this instead of operator[] for more performance.
    // @note that the end() will return the last + 1 memory addr.
    inline T*
    begin(void);
    inline const T*
    begin(void) const;
    inline T*
    end(void);
    inline const T*
    end(void) const;

private:
    char mData[SIZE * sizeof(T)];
};



////////////////////////////////////////////////////////////////////////////////
// inline stuff
//

template <typename T, unsigned int SIZE>
inline T&
NoConstructArray<T, SIZE>::operator[](unsigned int i)
{
    return (reinterpret_cast<T*>(mData))[i];
}
template <typename T, unsigned int SIZE>
inline const T&
NoConstructArray<T, SIZE>::operator[](unsigned int i) const
{
    return (reinterpret_cast<T*>(mData))[i];
}

template <typename T, unsigned int SIZE>
inline T*
NoConstructArray<T, SIZE>::begin(void)
{
    return (reinterpret_cast<T*>(mData));
}
template <typename T, unsigned int SIZE>
inline const T*
NoConstructArray<T, SIZE>::begin(void) const
{
    return (reinterpret_cast<T*>(mData));
}
template <typename T, unsigned int SIZE>
inline T*
NoConstructArray<T, SIZE>::end(void)
{
    return (reinterpret_cast<T*>(mData + SIZE * sizeof(T)));
}
template <typename T, unsigned int SIZE>
inline const T*
NoConstructArray<T, SIZE>::end(void) const
{
    return (reinterpret_cast<T*>(mData + SIZE * sizeof(T)));
}


} /* namespace core */
#endif /* NOCONSTRUCTARRAY_H_ */
