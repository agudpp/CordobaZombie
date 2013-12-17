/*
 * StatisticsInformer.h
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#ifndef STATISTICSINFORMER_H_
#define STATISTICSINFORMER_H_


#include <string.h>

#include <string>

#include <OgreRenderWindow.h>

namespace demo_app {

class StatisticsInformer
{
public:

    struct Data {
        unsigned int numRagdolls;
        unsigned int totalPhysicsObjects;

        Data() {reset();}

        void reset(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

public:
    StatisticsInformer();
    ~StatisticsInformer();

    // @brief Set the render window to retrieve the information
    //
    inline void
    setOgreRenderWindow(Ogre::RenderWindow* renderWindow);


    // @brief Clear / reset the data.
    //
    void
    reset(void);

    // @brief Save the data to a file (append to the last of the file if already
    //        exists).
    // @param fileName      The filename where we will save the info
    // @return true on success | false otherwise
    //
    bool
    dumpInform(const std::string& filename);

    // @brief Call this method each frame with the associated data
    //
    void
    update(float frameTime, const Data& data);

private:

    enum Contexts {
        CTX_BEST,
        CTX_WORST,
        CTX_AVG,

        CTX_COUNT,
    };

    struct Context {
        float fps;
        unsigned int numTris;
        unsigned int numBatches;
        unsigned int numPhysicsObj;
        unsigned int numRagdolls;


        Context(){reset();}
        void reset(void)
        {
            memset(this, 0, sizeof(*this));
        }
    };

    // @brief Init contexts
    //
    void
    initContexts(void);

    // @brief Get the context associated to current number of fps.
    //
    inline Context&
    getContextFromFPS(float fps, bool& shouldUpdate);

    // @brief context to string convertion
    //
    std::string
    convertToString(const Context& c);

private:
    Context mContexts[Contexts::CTX_COUNT];
    Ogre::RenderWindow* mRenderWindow;
    unsigned int mCallsCount;
};








////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline StatisticsInformer::Context&
StatisticsInformer::getContextFromFPS(float fps, bool& shouldUpdate)
{
    shouldUpdate = false;
    if (fps < mContexts[Contexts::CTX_WORST].fps) {
        shouldUpdate = true;
        return mContexts[Contexts::CTX_WORST];
    } else if (fps > mContexts[Contexts::CTX_BEST].fps) {
        shouldUpdate = true;
        return mContexts[Contexts::CTX_BEST];
    }

    mContexts[Contexts::CTX_AVG];
}

inline void
StatisticsInformer::setOgreRenderWindow(Ogre::RenderWindow* renderWindow)
{
    mRenderWindow = renderWindow;
}

} /* namespace demo_app */
#endif /* STATISTICSINFORMER_H_ */
