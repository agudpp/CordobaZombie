/*
 * PhysicsHandler.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#include "PhysicsHandler.h"

#include <debug/DebugUtil.h>

namespace demo_app {



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
PhysicsHandler::PhysicsHandler()
{
}
////////////////////////////////////////////////////////////////////////////////
PhysicsHandler::~PhysicsHandler()
{

}

////////////////////////////////////////////////////////////////////////////////
void
PhysicsHandler::clear(void)
{
    mObjects.clear();
}

////////////////////////////////////////////////////////////////////////////////
void
PhysicsHandler::build(void)
{
    // do nothing?
}

////////////////////////////////////////////////////////////////////////////////
void
PhysicsHandler::performCollisions(float timeFrame)
{
    // we will iterate over all the elements and will update them and remove
    // all the elements that have nothing to do
    for (unsigned int i = 0; i < mObjects.size(); ++i) {
        if (!mObjects[i]->update(timeFrame)) {
            //remove this one
            mObjects.disorder_remove(i);
            --i;
        }
    }
}

} /* namespace demo_app */
