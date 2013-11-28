/*
 * FEElement.h
 *
 *  Created on: Oct 28, 2013
 *      Author: agustin
 */

#ifndef FEELEMENT_H_
#define FEELEMENT_H_

#include <math/AABB.h>

#include "FEInfo.h"

namespace ui {

// forward
//
class FEManager;

class FEElement
{
public:
    FEElement(FEManager* manager = 0, const core::AABB& aabb = core::AABB()) :
        mManager(manager)
    ,   mAABB(aabb)
    ,   mIsActive(true)
    {}
    virtual
    ~FEElement(){};

    // @brief Set / get the manager instance of this element
    // @param manager   The manager instance
    //
    inline void
    setManager(FEManager* manager);
    inline FEManager*
    manager(void);
    inline const FEManager*
    manager(void) const;

    // @brief Set the size of the element container
    // @param size      The size
    // @note The window is taking like: (top,left) = (1,0), (bottom,right) = (0,1)
    //       respectively.
    //
    void
    setDimensions(const core::AABB& size);
    inline const core::AABB&
    dimensions(void) const;

    // @brief Check if a point is inside this element (mouse position).
    // @param point     The point we want to check
    // @returns true if it is | false otherwise
    //
    inline bool
    checkPointInside(const core::Vector2& point) const;

    // @brief Set this element to be active or inactive.
    // @param active    If it is active
    //
    void
    setActive(bool active);
    inline bool
    isActive(void) const;

    // @brief This event is called whenever the mouse get inside over this element.
    // @param info      The information of the mouse and other stuff.
    //
    virtual void
    mouseIn(const FEInfo& info) = 0;

    // @brief This event is called when the mouse gets out from the element (in
    //        the past call the element was over this element).
    // @param info      The information of the mouse and other stuff.
    //
    virtual void
    mouseOut(const FEInfo& info) = 0;

    // @brief Event called all the frames that the mouse is over the element.
    // @param info      The information about the mouse buttons and other stuff.
    //
    virtual void
    mouseOver(const FEInfo& info) = 0;

    // @brief Method called to catch general events like key pressing and other
    //        stuff.
    // @param info      The associated information
    //
    virtual void
    onEvent(const FEInfo& info) = 0;

private:
    FEManager* mManager;
    core::AABB mAABB;
    bool mIsActive;

    // member used only by the manager to improve stuff
    friend class FEManager;
    unsigned short _elemIndex;
};



////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
FEElement::setManager(FEManager* manager)
{
    mManager = manager;
}
inline FEManager*
FEElement::manager(void)
{
    return mManager;
}
inline const FEManager*
FEElement::manager(void) const
{
    return mManager;
}


inline const core::AABB&
FEElement::dimensions(void) const
{
    return mAABB;
}

inline bool
FEElement::checkPointInside(const core::Vector2& point) const
{
    return mAABB.checkPointInside(point);
}

inline bool
FEElement::isActive(void) const
{
    return mIsActive;
}

} /* namespace ui */
#endif /* FEELEMENT_H_ */
