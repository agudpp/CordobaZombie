/*
 * TriggerAgent.h
 *
 *  Created on: Jun 20, 2013
 *      Author: agustin
 */

#ifndef TRIGGERAGENT_H_
#define TRIGGERAGENT_H_

#include <math/Vec2.h>
#include <types/basics.h>
#include <types/RefContainer.h>

#include "TriggerSystemDefines.h"

namespace core {


// Forward declarations
//
class TriggerSystem;

class TriggerAgent
{
public:
    ~TriggerAgent(){}

    // @brief Get the current position of the agent
    // @returns current position of the agent
    //
    inline const Vector2&
    position(void) const;

    // @brief Set the position of the agent (update its position)
    // @param position  The new position of the agent
    //
    TriggerCode
    setPosition(const Vector2& position);

    // @brief Set/get the ID of the agent
    //
    inline core::uint16_t
    id(void) const;

    // @brief Set/get the mask to be used as filter (to skip all the zones that
    //        doesn't match with the agent mask)
    // @param mask  The mask to be used for this agent
    //
    inline core::uint16_t
    mask(void) const;
    inline void
    setMask(core::uint16_t mask);

private:
    // friend class
    //
    friend class TriggerSystem;

    // constructor only available for the TriggerSystem
    //
    TriggerAgent(TriggerSystem& ts);

    // @brief Set/get the ID of the agent
    //
    inline void
    setID(core::uint16_t id);

private:
    TriggerSystem& mTriggerSystem;
    Vector2 mPosition;
    core::uint16_t mID;
    core::uint16_t mMask;

private:
    // This members are used only by the TriggerSystem
    //
    core::uint16_t lastCellID;
    TriggerColor_t currentColors;

};


////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//


inline const Vector2&
TriggerAgent::position(void) const
{
    return mPosition;
}

inline core::uint16_t
TriggerAgent::id(void) const
{
    return mID;
}
inline void
TriggerAgent::setID(core::uint16_t id)
{
    mID = id;
}

inline core::uint16_t
TriggerAgent::mask(void) const
{
    return mMask;
}
inline void
TriggerAgent::setMask(core::uint16_t mask)
{
    mMask = mask;
}

} /* namespace core */
#endif /* TRIGGERAGENT_H_ */
