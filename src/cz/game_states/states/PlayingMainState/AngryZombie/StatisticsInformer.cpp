/*
 * StatisticsInformer.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#include "StatisticsInformer.h"

#include <debug/DebugUtil.h>

namespace demo_app {


////////////////////////////////////////////////////////////////////////////////
StatisticsInformer::StatisticsInformer()
{

}
////////////////////////////////////////////////////////////////////////////////
StatisticsInformer::~StatisticsInformer()
{

}

////////////////////////////////////////////////////////////////////////////////
void
StatisticsInformer::reset(void)
{
    debugERROR("TODO\n");
}

////////////////////////////////////////////////////////////////////////////////
bool
StatisticsInformer::dumpInform(const std::string& filename)
{
    debugERROR("TODO\n");
}

////////////////////////////////////////////////////////////////////////////////
void
StatisticsInformer::update(float frameTime, const Data& data)
{
    // TODO: complete here:
    // Calculate the current fps.
    // If we have a new count of fps save if it is the maximum / minimum / avg
    // Also save the current information (if new event occur), save the current
    // number of ragdolls and number of elements in the scene, etc. Check if
    // we can get the triangles count from the rendering engine also to have a
    // more accurate data.
    //

}


} /* namespace demo_app */
