/*
 * StatisticsInformer.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#include "StatisticsInformer.h"

#include <time.h>
#include <stdio.h>

#include <fstream>
#include <sstream>
#include <limits>

#include <debug/DebugUtil.h>

namespace demo_app {

////////////////////////////////////////////////////////////////////////////////
void
StatisticsInformer::initContexts(void)
{
    for (Context& c : mContexts) {
        c.reset();
    }

    mContexts[Contexts::CTX_BEST].fps = std::numeric_limits<float>::min();
    mContexts[Contexts::CTX_WORST].fps = std::numeric_limits<float>::max();
}

////////////////////////////////////////////////////////////////////////////////
std::string
StatisticsInformer::convertToString(const Context& c)
{
    /*float fps;
        unsigned int numTris;
        unsigned int numBatches;
        unsigned int numPhysicsObj;
        unsigned int numRagdolls;
        */
    std::stringstream ss;
    ss << "\tfps: " << c.fps << "\n"
        "\tnumber of triangles: " << c.numTris << "\n"
        "\tnumber of batches: " << c.numBatches << "\n"
        "\tnumber of physics objects: " << c.numPhysicsObj << "\n"
        "\tnumber of ragdolls: " << c.numRagdolls << "\n";
    return ss.str();
}

////////////////////////////////////////////////////////////////////////////////
StatisticsInformer::StatisticsInformer() :
    mRenderWindow(0)
,   mCallsCount(0)
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
    initContexts();
    mCallsCount = 0;
    ASSERT(mRenderWindow);
    mRenderWindow->resetStatistics();
}

////////////////////////////////////////////////////////////////////////////////
bool
StatisticsInformer::dumpInform(const std::string& filename)
{
    ASSERT(mRenderWindow);

    // open the file to export the data using the current date and time
    std::ofstream file(filename.c_str(), std::ios::app);
    if (!file.is_open()) {
        debugERROR("Error opening the file %s when trying to dump the statistics "
            "information\n", filename.c_str());
        return false;
    }

    time_t rawtime;
    time (&rawtime);
    char str[512];
    sprintf(str, "%s", ctime(&rawtime));

    // print it into the stream
    file <<
        "\n\n-----------------------------------------------------------------------------\n"
        "            New statistics information " << str << "\n"
        "-----------------------------------------------------------------------------\n\n";

    // now put the data
    file <<
        "Ogre Render window configuration:\n"
        "\tWidth: " << mRenderWindow->getWidth() << "\n"
        "\tHeight: " << mRenderWindow->getHeight() << "\n"
        "\tColourDepth: " << mRenderWindow->getColourDepth() << "\n\n"

        "Ogre data statistics:\n"
        "\tBestFPS: " << mRenderWindow->getBestFPS() << "\n"
        "\tWorstFPS: " << mRenderWindow->getWorstFPS() << "\n"
        "\tAverageFPS: " << mRenderWindow->getAverageFPS() << "\n"
        "\tBestTimeFrame: " << mRenderWindow->getBestFrameTime() << "\n"
        "\tWorstTimeFrame: " << mRenderWindow->getWorstFrameTime() << "\n\n"

        "Local information:\n"
        "Best case:\n" << convertToString(mContexts[Contexts::CTX_BEST]) <<
        "Avg case:\n" << convertToString(mContexts[Contexts::CTX_AVG]) <<
        "Worst case:\n" << convertToString(mContexts[Contexts::CTX_WORST]) <<
        "\n\n";

    // close the file and return that everything is fine
    file.close();

    return true;
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
    ASSERT(mRenderWindow);


    const float fps = mRenderWindow->getLastFPS();

    if (fps == 0.f) {
        return; // do nothing
    }
    ++mCallsCount;
    const unsigned int triCount = mRenderWindow->getTriangleCount();
    const unsigned int batchCount = mRenderWindow->getBatchCount();

    // update the assocated context
    bool shouldUpdate = false;
    Context& ctx = getContextFromFPS(fps, shouldUpdate);
    if (shouldUpdate) {
        ctx.fps = fps;
        ctx.numBatches = mRenderWindow->getBatchCount();
        ctx.numTris = mRenderWindow->getTriangleCount();
        ctx.numPhysicsObj = data.totalPhysicsObjects;
        ctx.numRagdolls = data.numRagdolls;
    }

    // Update the average case
    // get the real values for each field
    const float invCallsCount = 1.f/static_cast<float>(mCallsCount);
    const float callsCount = static_cast<float>(mCallsCount);
    Context& avgCtx = mContexts[Contexts::CTX_AVG];
    avgCtx.fps = (avgCtx.fps * callsCount + fps) * invCallsCount;
    avgCtx.numTris = (avgCtx.numTris * callsCount + triCount) * invCallsCount;
    avgCtx.numBatches = (avgCtx.numBatches * callsCount + batchCount) * invCallsCount;
    avgCtx.numPhysicsObj = (avgCtx.numPhysicsObj * callsCount +
        data.totalPhysicsObjects) * invCallsCount;
    avgCtx.numRagdolls = (avgCtx.numRagdolls * callsCount + data.numRagdolls) *
        invCallsCount;

}


} /* namespace demo_app */
