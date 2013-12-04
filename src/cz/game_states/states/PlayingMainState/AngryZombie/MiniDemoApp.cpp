/*
 * MiniDemoApp.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#include "MiniDemoApp.h"

#include <debug/DebugUtil.h>



// define useful macro precondition
#define PRECONDITION_CHECK \
    ASSERT(mData.sceneMngr);\
    ASSERT(mData.camera);\
    ASSERT(mData.frontEndManager);\
    ASSERT(mData.mouseCursor);\
    ASSERT(mData.inputHelper);\
    ASSERT(mData.effectHandler);\
    ASSERT(mData.rscHandler);\
    ASSERT(mData.soundHandler);\
    ASSERT(mData.informer);


namespace demo_app {



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
MiniDemoApp::MiniDemoApp()
{

}

////////////////////////////////////////////////////////////////////////////////
MiniDemoApp::~MiniDemoApp()
{

}

////////////////////////////////////////////////////////////////////////////////
void
MiniDemoApp::setData(const DemoData& data)
{
    // check that all demo data is right
    mData = data;

    PRECONDITION_CHECK;
}

////////////////////////////////////////////////////////////////////////////////
bool
MiniDemoApp::getResourceToLoad(rrh::ResourceGroup& resource)
{
    PRECONDITION_CHECK;

    ASSERT(false && "TODO");
    return true;
}
////////////////////////////////////////////////////////////////////////////////
bool
MiniDemoApp::getResourceToUnload(rrh::ResourceGroup& resource)
{
    // we remove the same that we load
    return getResourceToLoad(resource);
}

////////////////////////////////////////////////////////////////////////////////
bool
MiniDemoApp::load(void)
{
    PRECONDITION_CHECK;

    // we will load everything here. Parse the SceneHandler and everything else

    // reset the informer
    mData.informer->reset();

    return true;
}
////////////////////////////////////////////////////////////////////////////////
bool
MiniDemoApp::unload(void);

////////////////////////////////////////////////////////////////////////////////
bool
MiniDemoApp::update(float timeFrame);

} /* namespace demo_app */
