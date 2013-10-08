/*
 * Effect.h
 *
 *  Created on: Oct 2, 2013
 *      Author: agustin
 */

#ifndef EFFECT_H_
#define EFFECT_H_


// @brief Here we will define the interface for the effects
//


namespace effect {

// Forward
//
class EffectHandler;

class Effect
{
public:
    Effect(){};
    virtual
    ~Effect(){};

    // @brief Function called before the effect will be reproduced
    //
    virtual void
    beforeStart(void) = 0;

    // @brief Function called each frame to update the effect.
    // @param timeFrame     The time of the frame
    // @returns true if the effect is still active and should be reproduced
    //          next frame, or false if the effect finished.
    //
    virtual bool
    update(float timeFrame) = 0;

    // @brief Function called after the effect stops reproducing it.
    //
    virtual void
    afterFinish(void) = 0;

private:
    friend class EffectHandler;

    unsigned int id;
};

} /* namespace effect */
#endif /* EFFECT_H_ */
