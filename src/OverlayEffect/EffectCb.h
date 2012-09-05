/*
 * EffectCb.h
 *
 *  Created on: 23/08/2012
 *      Author: agustin
 *
 */

#ifndef EFFECTCB_H_
#define EFFECTCB_H_

namespace OvEff {

struct EffectCb {
	enum EventID {
		STARTING,		// when the effect will start
		ENDING			// when the effect is ending
	};

	/**
	 * Function to be implemented
	 * @param	id	The event ID
	 */
	virtual void operator()(EventID id) = 0;
};

}

#endif /* EFFECTCB_H_ */
