/*
 * ContiguousCont.h
 *
 *  Created on: Aug 20, 2013
 *      Author: agustin
 */

#ifndef CONTIGUOUSCONT_H_
#define CONTIGUOUSCONT_H_

#include <cstring>
#include <stdlib.h>
#include <cstdlib>

#include <types/basics.h>

namespace core {

template <typename T>
struct ContiguousContainer {
    core::uint32_t size;
    T* data;


    // @brief Access to the i-th element
    // @param i     The position in the container
    //
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

    // @brief build the container from a memory addr
    // @param addr      The memory address
    //
    inline void
    setMemAddr(void* addr)
    {
        std::memcpy(&size, addr, sizeof(core::uint32_t));
        data = addr + sizeof(core::uint32_t);
    }

    // @brief Free the current memory of this container using free() and reset
    //        the size and data to 0.
    //
    inline void
    freeMemory(void)
    {
        if (data != 0) {
            free(data); size = data = 0;
        }
    }

    // empty constructor
    ContiguousContainer() : data(0), size(0){};

    // build the container from a block of memory
    //
    ContiguousContainer(void* addr)
    {
        setMemAddr(addr);
    }
};


} /* namespace core */
#endif /* CONTIGUOUSCONT_H_ */
