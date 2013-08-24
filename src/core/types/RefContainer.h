/*
 * RefContainer.h
 *
 *  Created on: Jun 21, 2013
 *      Author: agustin
 */

#ifndef REFCONTAINER_H_
#define REFCONTAINER_H_

#include <vector>

#include "basics.h"

namespace core {

template <typename T, typename CounterType = core::uint8_t>
class RefContainer
{
    struct Pair {
        Pair(T& f, CounterType s = 0) : first(f), second(s) {}
        bool operator==(const Pair& other) const
        {
            return first == other.first;
        }

        T first;
        CounterType second;

    };
    typedef Pair Element;

public:

    // @brief Add an element to the container (if already exists it will
    //        increment the reference counter).
    // @param element   The element to be added
    //
    inline void
    addElement(T& element)
    {
        for (core::size_t i = 0, size = mElements.size(); i < size; ++i) {
            if (mElements[i] == element) {
                ++mElements[i].second;
                return;
            }
        }
        mElements.push_back(Element(element));
    }

    // @brief Remove an element (or decrement the counter if there are more than
    //        one reference to it). We will use swap with the last to remove the
    //        element.
    // @param element   The element to be removed
    //
    inline void
    removeElement(T& element)
    {
        for (core::size_t i = 0, size = mElements.size(); i < size; ++i) {
            if (mElements[i] == element) {
                if (mElements[i].second == 1) {
                    // we must remove the element
                    mElements[i] = mElements.back();
                    mElements.pop_back();
                    return;
                } else {
                    // decrement the reference only
                    --mElements[i].second;
                }
            }
        }
    }

    // @brief Clear the container removing all the elements
    //
    inline void
    clear(void)
    {
        mElements.clear();
    }

    // @brief Overloaded method to get the i-th element.
    //
    inline T&
    operator[](core::size_t i)
    {
        return mElements[i].first;
    }

    // @brief Check if the container is emtpy
    //
    inline bool
    empty(void) const
    {
        return mElements.empty();
    }

    // @brief Check the size of the container
    //
    inline core::size_t
    size(void) const
    {
        return mElements.size();
    }

private:
    // for now we will use a vector
    std::vector<Element> mElements;

};

} /* namespace core */
#endif /* REFCONTAINER_H_ */
