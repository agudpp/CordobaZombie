/*
 * BillboardWrapper.h
 *
 *  Created on: Apr 7, 2013
 *      Author: agustin
 */

#ifndef BILLBOARDWRAPPER_H_
#define BILLBOARDWRAPPER_H_

#include <OgreBillboard.h>

#include <Common/DebugUtil/DebugUtil.h>

namespace billboard {

// Wrapper class needed to do a faster check when we have to let the billboard
// available
//
class BillboardWrapper {
public:
    BillboardWrapper(Ogre::Billboard *b = 0, unsigned int id = 0) :
        mBillboard(b)
    ,   mID(id)
    {}

    ~BillboardWrapper(){};

    // get the billboard
    //
    inline Ogre::Billboard* billboard(void) {return mBillboard;}
    inline const Ogre::Billboard* billboard(void) const {return mBillboard;}

    // overloaded methods to fast access the billboard itself
    //
    inline Ogre::Billboard *operator->() {return mBillboard;}
    inline const Ogre::Billboard *operator->() const {return mBillboard;}

    inline Ogre::Billboard &operator*() {return *mBillboard;}
    inline const Ogre::Billboard &operator*() const {return *mBillboard;}

    // automatically cast operator
    //
    inline operator Ogre::Billboard *() {return mBillboard;}
    inline operator const Ogre::Billboard *() const {return mBillboard;}
    inline operator bool() const {return mBillboard != 0;}

    // reset the pointer of the billboard
    //
    inline void reset(void) {mBillboard = 0;}

    // get/set the atlas ID
    //
    inline unsigned int atlasID(void) const {return mID;}
    inline void setAtlasID(unsigned int id) {mID = id;}

private:
    Ogre::Billboard *mBillboard;
    unsigned int mID;
};


} /* namespace billboard */
#endif /* BILLBOARDWRAPPER_H_ */
