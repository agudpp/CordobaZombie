/*
 * TriggerZone.h
 *
 *  Created on: Jun 20, 2013
 *      Author: agustin
 */

#ifndef TRIGGERZONE_H_
#define TRIGGERZONE_H_

#include <math/AABB.h>
#include <math/Vec2.h>
#include <types/basics.h>


namespace core {

// we will define the zone as an AABB only for now
//
typedef core::AABB TZType;

class TriggerZone
{
public:

    // @brief For now we will accept only AABB as zones
    //
    inline TriggerZone(const TZType& zone);

    // Empty constructor / destructors
    //
    TriggerZone() : mMask(~0) {};
    ~TriggerZone(){};

    // @brief Check if a point is inside of this zone
    // @param point The point to be checked
    // @return true if it is inside | false otherwise
    //
    inline bool
    isPointInside(const Vector2& point) const;

    // @brief Check if another TriggerZone intersects with this one
    // @param tz The TriggerZone to be checked
    // @return true if intersects | false otherwise
    //
    inline bool
    intersects(const TriggerZone& other) const;

    // @brief Configure the current zone of this object
    // @param zone  The zone to be used for this current instance
    //
    inline void
    setZone(const TZType& zone);

    // @brief Get the current zone
    //
    inline const TZType&
    zone(void) const;

    // @brief Set / get the trigger mask. This trigger mask is used as a filter
    //        when checking for different agents (some triggers should be activated
    //        only for some agents, but not for others).
    //
    inline core::uint16_t
    mask(void) const;
    inline void
    setMask(core::uint16_t mask);

private:
    TZType mZone;
    core::uint16_t mMask;
};


////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//


inline TriggerZone::TriggerZone(const TZType& zone) :
    mZone(zone)
,   mMask(~0)
{
}

inline bool
TriggerZone::isPointInside(const Vector2& point) const
{
    return mZone.checkPointInside(point);
}

inline bool
TriggerZone::intersects(const TriggerZone& other) const
{
    return mZone.collide(other.mZone);
}

inline void
TriggerZone::setZone(const TZType& zone)
{
    mZone = zone;
}

inline const TZType&
TriggerZone::zone(void) const
{
    return mZone;
}

inline core::uint16_t
TriggerZone::mask(void) const
{
    return mMask;
}

inline void
TriggerZone::setMask(core::uint16_t mask)
{
    mMask = mask;
}

} /* namespace core */
#endif /* TRIGGERZONE_H_ */
