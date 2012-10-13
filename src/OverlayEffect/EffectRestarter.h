/* This class is used to automatically restart a effect when it finish it
 *
 * EffectRestarter.h
 *
 *  Created on: 25/09/2012
 *      Author: agustin
 *
 */

#ifndef EFFECTRESTARTER_H_
#define EFFECTRESTARTER_H_

#include <boost/signals.hpp>
#include <boost/bind.hpp>

#include "DebugUtil.h"
#include "OverlayEffect.h"


namespace OvEff {

class EffectRestarter {
public:
    EffectRestarter(OverlayEffect *oe, bool stopRestarting = false) :
        effect(oe),
        stopRestart(stopRestarting)
    {

    }

    // stop restart the effect
    inline void setStopRestart(bool sr) {stopRestart = sr;}


    void operator()(OverlayEffect::EventID id)
    {
        if (effect == 0 || stopRestart) return;
        if(id == OverlayEffect::ENDING) effect->start();
    }

    OverlayEffect *effect;
    bool stopRestart;
};

}

#endif /* EFFECTRESTARTER_H_ */
