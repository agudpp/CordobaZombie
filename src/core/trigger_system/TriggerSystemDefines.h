/*
 * TriggerSystemDefines.h
 *
 *  Created on: Jun 20, 2013
 *      Author: agustin
 */

#ifndef TRIGGERSYSTEMDEFINES_H_
#define TRIGGERSYSTEMDEFINES_H_

#include <boost/signals.hpp>

namespace core {

// forward
//
class TriggerAgent;

// The number of colors we will use
#define TS_NUM_COLORS       32

typedef core::uint32_t TriggerColor_t;



// Define the Possible error codes for this module
//
enum TriggerCode {
    Ok = 0,                 // means no error
    UnreachablePosition,    // for when the agent is outside of any possible
                            // zone
    AlreadyInitialized,     // error, we are trying to init twice the same agent
    SystemNotBuilt,         // the system wasn't built already
};

// callback stuff
namespace TriggerCallback {


// Define the callbacks interface
enum EventType {
    Entering,       // when we are entering the zone
    Leaving,        // when we are leaving the zone
};

struct EventInfo {
    EventInfo(EventType t, core::uint16_t z, const TriggerAgent& a) :
        type(t), zoneID(z), agent(a)
    {}

    EventType type;
    core::uint16_t zoneID;
    const TriggerAgent& agent;
};


// TODO: remove boost and use stl instead
// Each signal will receive a TriggerZone and an EventType
typedef boost::signal<void (const EventInfo&)> Signal;
typedef boost::signals::connection Connection;
}

}

#endif /* TRIGGERSYSTEMDEFINES_H_ */
