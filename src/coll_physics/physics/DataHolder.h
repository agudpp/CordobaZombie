/*
 * DataHolder.h
 *
 *  Created on: Oct 22, 2013
 *      Author: agustin
 */

#ifndef DATAHOLDER_H_
#define DATAHOLDER_H_

#include <vector>

namespace physics {

// @brief This class will be used to hold elements and remove them (delete its
//        memory) once we remove the holder instance
//


template<typename T>
class DataHolder
{
public:
    typedef std::vector<T> ElementVec;
public:
    inline DataHolder() {};
    inline ~DataHolder() {freeAll();};

    // @brief Add a new element
    //
    inline void
    add(T element);

    // @brief Remove an element from the container without freeing its memory
    //
    inline void
    remove(T element);

    // @brief Reserve memory
    //
    inline void
    reserveMem(unsigned int size);

    // @brief Return the list of the elements
    //
    inline ElementVec&
    elements(void);
    inline const ElementVec&
    elements(void) const;

    // @brief Remove all the elements without freeing them
    //
    inline void
    clearWithoutFree(void);

    // @brief Remove all the elements deleting (freeing) its memory
    //
    inline void
    freeAll(void);

private:
    ElementVec mElements;
};





////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

template<typename T>
inline void
DataHolder<T>::add(T element)
{
    mElements.push_back(element);
}

template<typename T>
inline void
DataHolder<T>::remove(T element)
{
    for (unsigned int i = 0, size = mElements.size(); i < size; ++i) {
        if (mElements[i] == element) {
            mElements[i] = mElements.back();
            mElements.back();
            return;
        }
    }
    return;
}

template<typename T>
inline void
DataHolder<T>::reserveMem(unsigned int size)
{
    mElements.reserve(size);
}

template<typename T>
inline typename DataHolder<T>::ElementVec&
DataHolder<T>::elements(void)
{
    return mElements;
}
template<typename T>
inline const typename DataHolder<T>::ElementVec&
DataHolder<T>::elements(void) const
{
    return mElements;
}

template<typename T>
inline void
DataHolder<T>::clearWithoutFree(void)
{
    mElements.clear();
}

template<typename T>
inline void
DataHolder<T>::freeAll(void)
{
    for (T& e : mElements) {
        delete e;
    }
    mElements.clear();
}

} /* namespace physics */
#endif /* DATAHOLDER_H_ */
