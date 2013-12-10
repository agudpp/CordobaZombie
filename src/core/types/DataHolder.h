/*
 * DataHolder.h
 *
 *  Created on: Oct 22, 2013
 *      Author: agustin
 */

#ifndef DATAHOLDER_H_
#define DATAHOLDER_H_

#include <vector>
#include <functional>

namespace core {

// @brief This class will be used to hold elements and remove them (delete its
//        memory or call the associated destructor method already set)
//        once we remove the holder instance.
//        Note that this class is designed to be used with pointers, and if not
//        you will be not able to compile, or at least you shouldn't!
//

// Helper method defining the interface of the function to be called to destroy
// the elements when this class goes from scope.
//

template<typename T>
class DataHolder
{
public:
    typedef std::vector<T>              ElementVec;
    typedef std::function<void (T)>    DestructorMethod;

    static void
    simpleDeleteMethod(T elem)
    {
        delete elem;
    }

public:
    inline DataHolder() : mDestructorMethod(simpleDeleteMethod) {};
    inline DataHolder(const DestructorMethod& method) :
        mDestructorMethod(method) {};
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

    // @brief Remove all the elements without freeing them (without calling the
    //        destructor method)
    //
    inline void
    clearWithoutFree(void);

    // @brief Remove all the elements deleting (freeing) its memory using the
    //        associated destructor method
    //
    inline void
    freeAll(void);

    // @brief Set the destructor method to be used or clear the current one to
    //        not use anyone
    // @param dm        The destructor method
    //
    inline void
    setDestructorMethod(const DestructorMethod& dm);
    inline void
    clearDestructorMethod(void);


private:
    // avoid copying
    DataHolder(const DataHolder<T>&);
    DataHolder& operator=(const DataHolder<T>&);

private:
    ElementVec mElements;
    DestructorMethod mDestructorMethod;
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
    if (!mDestructorMethod) {
        // nothing to do
        mElements.clear();
        return;
    }
    // else apply the destructor method
    for (T& e : mElements) {
        mDestructorMethod(e);
    }
    mElements.clear();
}

template<typename T>
inline void
DataHolder<T>::setDestructorMethod(const DestructorMethod& dm)
{
    mDestructorMethod = dm;
}
template<typename T>
inline void
DataHolder<T>::clearDestructorMethod(void)
{
    mDestructorMethod = nullptr;
}

} /* namespace core */
#endif /* DATAHOLDER_H_ */
