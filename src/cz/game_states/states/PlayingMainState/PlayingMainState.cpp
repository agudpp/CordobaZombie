/*
 * PlayingMainState.cpp
 *
 *  Created on: Oct 31, 2013
 *      Author: agustin
 */

#include "PlayingMainState.h"

namespace cz {

PlayingMainState::PlayingMainState()
{
    // TODO Auto-generated constructor stub

}

PlayingMainState::~PlayingMainState()
{
    // TODO Auto-generated destructor stub
}


////////////////////////////////////////////////////////////////////////////
//                           Inherited methods
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
const MainStateID&
PlayingMainState::ID(void) const
{
    static MainStateID id = MainStateID::PlayingState;
    return id;
}

////////////////////////////////////////////////////////////////////////////////
bool
PlayingMainState::configureState(const MainStateInformation& info)
{
    // configure the mDemoApp
    demo_app::DemoData data;
    data.sceneMngr = sOgreInfo.sceneMngr;
    data.renderWindow = sOgreInfo.renderWindow;
    data.camera = sOgreInfo.camera;
    data.frontEndManager = sCommonHandlers.frontEndManager;
    data.mouseCursor = sCommonHandlers.mouseCursor;
    data.inputHelper = sCommonHandlers.inputHelper;
    data.effectHandler = sCommonHandlers.effectHandler;
    data.rscHandler = sRcHandler;
    data.soundHandler = sSoundHandler;
    data.informer = &mInformer;
    mDemoApp.setData(data);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
PlayingMainState::getResourcesToLoad(ResourceGroupList& resourceList)
{
    // get the resources needed by the demo app
    rrh::ResourceGroup rg;
    if (!mDemoApp.getResourceToLoad(rg)) {
        debugERROR("Error trying to laod resources for the demo app\n");
        return false;
    }
    resourceList.push_back(rg);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
PlayingMainState::readyToGo(void)
{
    // set the resulting event that will always be the same
    mEventInfo = MainStateEvent::EVENT_DONE;

    // load demo app
    return mDemoApp.load();
}

////////////////////////////////////////////////////////////////////////////////
bool
PlayingMainState::update(float timeFrame)
{
    return mDemoApp.update(timeFrame);
}

////////////////////////////////////////////////////////////////////////////////
bool
PlayingMainState::unload(void)
{
    // save the statistics into a file
    mInformer.dumpInform("performance_stats.log");

    // unload demo app
    return mDemoApp.unload();
}

////////////////////////////////////////////////////////////////////////////////
bool
PlayingMainState::getResourcesToUnload(ResourceGroupList& resourceList)
{
    // get the resources to unload by the demo app
    rrh::ResourceGroup rg;
    if (!mDemoApp.getResourceToUnload(rg)) {
        debugERROR("Error trying to unload resources for the demo app\n");
        return false;
    }
    resourceList.push_back(rg);
    return true;
}

} /* namespace cz */
