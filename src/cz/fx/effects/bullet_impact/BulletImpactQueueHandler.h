/*
 * BulletImpactQueueHandler.h
 *
 *  Created on: Nov 20, 2013
 *      Author: agustin
 */

#ifndef BULLETIMPACTQUEUEHANDLER_H_
#define BULLETIMPACTQUEUEHANDLER_H_


#include "BulletImpactEffect.h"

#include <debug/DebugUtil.h>
#include <fx/effects/EffectQueueDefs.h>

namespace cz {

class BulletImpactQueueHandler
{
public:
    // Define the different number of Queues we will handle (queue types)
    //
    enum Type {
        BIQT_DEFAULT = 0,
        BIQT_METAL,
        BIQT_WOOD,
        BIQT_ROCK,

        COUNT,
    };

private:
    // define the list of names of particle systems we need to use for a
    // particular type of queue
    //
    static const char* QUEUE_PARTICLE_NAMES[Type::COUNT];

public:
    BulletImpactQueueHandler();
    // @note when we destroy this class we will destroy all the queues first
    ~BulletImpactQueueHandler();

    // @brief Build all the queues (particle systems and sounds)
    // @return true on success | false otherwise
    //
    bool
    buildQueues(void);

    // @brief Destroy all the queues
    //
    void
    destroyQueues(void);

    // @brief Return a specific queue reference from a given type.
    //        This queue will be valid until this class is destroyed. You should
    //        not destroy nor free the memory of the returned queue.
    // @param type      The type of the queue we want
    // @return the associated queue pointer | 0 on error
    //
    inline BulletImpactEffectQueue*
    queue(Type type);
    inline const BulletImpactEffectQueue*
    queue(Type type) const;

private:
    BulletImpactEffectQueue mQueues[Type::COUNT];
};







////////////////////////////////////////////////////////////////////////////////
// Inline Stuff
//

inline BulletImpactEffectQueue*
BulletImpactQueueHandler::queue(Type type)
{
    ASSERT(type < Type::COUNT);
    return &(mQueues[type]);
}
inline const BulletImpactEffectQueue*
BulletImpactQueueHandler::queue(Type type) const
{
    ASSERT(type < Type::COUNT);
    return &(mQueues[type]);
}

} /* namespace cz */
#endif /* BULLETIMPACTQUEUEHANDLER_H_ */
