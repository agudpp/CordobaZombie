/*
 * ContiguousCont.h
 *
 *  Created on: Aug 20, 2013
 *      Author: agustin
 */

#ifndef CONTIGUOUSCONT_H_
#define CONTIGUOUSCONT_H_

namespace core {

template <typename T>
struct ContiguousContainer {
    T* data;
    unsigned int size;

    inline const T&
    operator[](unsigned int i) const
    {
        return data[i];
    }
    inline T&
    operator[](unsigned int i)
    {
        return data[i];
    }

    ContiguousContainer() : data(0), size(0){};
};


} /* namespace core */
#endif /* CONTIGUOUSCONT_H_ */
