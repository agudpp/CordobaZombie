/*
 * BackpackItem.h
 *
 *  Created on: 14/05/2012
 *      Author: agustin
 */

#ifndef BACKPACKITEM_H_
#define BACKPACKITEM_H_

#include <boost/shared_ptr.hpp>

#include <Common/DebugUtil/DebugUtil.h>

#include "BackpackDefines.h"

struct BackpackItem
{
public:
    BackpackItem(BackpackDef::ItemType type) :
        type(type)
    ,   userDef(0)
    {}

    // helper method
    //
    template<typename T>
    inline void getUserDef(T *&u)
    {
        u = static_cast<T*>(userDef);
    }


    // Data
    //
    void *userDef;
    BackpackDef::ItemType type;
};

// Define the shared ptr
//
typedef boost::shared_ptr<BackpackItem> BackpackItemPtr;



#endif /* BACKPACKITEM_H_ */
