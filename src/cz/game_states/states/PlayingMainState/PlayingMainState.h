/*
 * PlayingMainState.h
 *
 *  Created on: Oct 31, 2013
 *      Author: agustin
 */

#ifndef PLAYINGMAINSTATE_H_
#define PLAYINGMAINSTATE_H_


#include <game_states/IMainState.h>

// TODO: this will be only for the demo stuff
#include <game_states/states/PlayingMainState/AngryZombie/MiniDemoApp.h>
#include <game_states/states/PlayingMainState/AngryZombie/StatisticsInformer.h>


namespace cz {

class PlayingMainState : public IMainState
{
public:
    PlayingMainState();
    virtual
    ~PlayingMainState();


    ////////////////////////////////////////////////////////////////////////////
    //                           Inherited methods
    ////////////////////////////////////////////////////////////////////////////

    // @brief This will return the State ID.
    //
    virtual const MainStateID&
    ID(void) const;

    // @brief This method will configure the state with all the information we
    //        currently have. The state should configure itself in this call
    //        (for example: create the full path where the resources will be loaded).
    // @param info      The global information that we currently have.
    // @return true on success | false otherwise.
    //
    virtual bool
    configureState(const MainStateInformation& info);

    // @brief Return the list of resources used by this state.
    //        This list should contain all the resources that we need to use
    //        this state.
    // @param resourceList      The resulting resource list we will need to load
    //                          to be able to execute the state correctly.
    // @return true on success | false otherwise.
    //
    virtual bool
    getResourcesToLoad(ResourceGroupList& resourceList);

    // @brief This method will be called after all the resources were loaded
    //        correctly. This is the method called before starting the update()
    //        loop.
    // @return true on success | false otherwise
    //
    virtual bool
    readyToGo(void);

    // @brief This is the main method that will be called each frame with the
    //        last time frame as parameter. Here you should do whatever you need
    //        to update the logic of the state, and after this call the scene
    //        will be rendered (OgreScene).
    //        This method will also return true if we need to continue calling
    //        this state or false otherwise (for example if we finish).
    // @param timeFrame     The last time frame.
    // @return true if we need to continue calling this method | false otherwise
    //
    virtual bool
    update(float timeFrame);

    // @brief This method will be called to unload the current state to destroy
    //        all the instances we use and will not be used anymore, but here
    //        we shouldn't uninitialize any resource for example, that will be
    //        done automatically in getResourcesToUnload().
    // @returns true on success | false otherwise
    //
    virtual bool
    unload(void);

    // @brief This method should return the list of all the resources that should
    //        be unloaded. This is not necessary the same list obtained from
    //        getResourcesToLoad() since probably we want to let some resources
    //        loaded after we do not use any more this state (for example, all
    //        those resources loaded into a shared data structure).
    // @param resourceList  The list of resources to be unloaded.
    // @return true on success | false otherwise
    //
    virtual bool
    getResourcesToUnload(ResourceGroupList& resourceList);

private:
    demo_app::MiniDemoApp mDemoApp;
    demo_app::StatisticsInformer mInformer;
};

} /* namespace cz */
#endif /* PLAYINGMAINSTATE_H_ */
