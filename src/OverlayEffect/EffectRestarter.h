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
    EffectRestarter(OverlayEffect *oe) :
        effect(oe)
    {
    }
    ~EffectRestarter()
    {
        unlink();
    }


    // link to the effect
    inline void link(void)
    {
        if (effect == 0) return;
        connection = effect->addCallback(boost::bind(
                &EffectRestarter::operator(),
                this,
                _1));
    }
    inline void unlink(void)
    {
        if (connection.connected()) connection.disconnect();
    }


    void operator()(OverlayEffect::EventID id)
    {
        if (effect == 0) return;
        if(id == OverlayEffect::ENDING) effect->start();
    }

    OverlayEffect *effect;

    boost::signals::connection connection;
};

}

#endif /* EFFECTRESTARTER_H_ */
