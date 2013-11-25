/*
 * IMainState.h
 *
 *  Created on: Oct 29, 2013
 *      Author: agustin
 */

#ifndef IMAINSTATE_H_
#define IMAINSTATE_H_



#include "MainStateInformation.h"
#include "MainStateEvent.h"


// Forward declaration
//
namespace rrh {
class ResourceHandler;
}

namespace mm {
class VideoPlayer;
class SoundManager;
}

namespace cz {

// @brief This class defines the interface for all the MainStates.
//        This states will be encapsulated to have no interaction with the
//        others states directly, instead they can use shared data structures
//        to fill in / read (like for example, load the game in one state, read
//        then in the game state).
//        Each state can contain different resources to be used, so, you can
//        specify which are the resources that you need in the getResources()
//        method.
//        Do not load any resource manually since this could bring future problems.
//
//        Each state will be called to the update() method on every frame, so
//        you don't need to worry about how to render the things.
//        You will also have access to the main Ogre classes like the RenderWindow
//        SceneManager, Root, Camera, etc.
//
//        The flow of the method called to the states will be:
//          configureState(info) -> getResourcesToLoad(r) -> readyToGo() ->
//          update() (until finish) -> getEventInformation() -> unload() ->
//          getResourcesToUnload(r).
//


// Forward
//
enum class MainStateID;

class IMainState
{
public:
    IMainState();
    virtual
    ~IMainState() {};

    // @brief Set the OgreCommon information to be used by all the States
    // @param ogreData      The ogre Data.
    //
    static void
    setOgreData(const OgreCommon& ogreData);

    // @brief Set the VideoPlayer global instance
    // @param videoPlayer   The Video Player.
    //
    static void
    setVideoPlayer(mm::VideoPlayer* videoPlayer);

    // @brief Set the SoundManager global instance
    // @param soundManager  The SoundManager.
    //
    static void
    setSoundManager(mm::SoundManager* soundManager);

    // @brief Set the ResourceHandler for all main states.
    // @param rch      The resources handler.
    //
    static void
    setRcHandler(const rrh::ResourceHandler* rch);


    // @brief This will return the State ID.
    //
    virtual const MainStateID&
    ID(void) const = 0;

    // @brief This method will configure the state with all the information we
    //        currently have. The state should configure itself in this call
    //        (for example: create the full path where the resources will be loaded).
    // @param info      The global information that we currently have.
    // @return true on success | false otherwise.
    //
    virtual bool
    configureState(const MainStateInformation& info) = 0;

    // @brief Return the list of resources used by this state.
    //        This list should contain all the resources that we need to use
    //        this state.
    // @param resourceList      The resulting resource list we will need to load
    //                          to be able to execute the state correctly.
    // @return true on success | false otherwise.
    //
    virtual bool
    getResourcesToLoad(ResourceGroupList& resourceList) = 0;

    // @brief This method will be called after all the resources were loaded
    //        correctly. This is the method called before starting the update()
    //        loop.
    // @return true on success | false otherwise
    //
    virtual bool
    readyToGo(void) = 0;

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
    update(float timeFrame) = 0;

    // @brief Return the last event information. This means: which was the
    //        event that produce the "stop" in the update() loop.
    // @returns the last MainStateEvent produced in this state.
    //
    inline const MainStateEvent&
    getEventInformation(void) const;

    // @brief This method will be called to unload the current state to destroy
    //        all the instances we use and will not be used anymore, but here
    //        we shouldn't uninitialize any resource for example, that will be
    //        done automatically in getResourcesToUnload().
    // @returns true on success | false otherwise
    //
    virtual bool
    unload(void) = 0;

    // @brief This method should return the list of all the resources that should
    //        be unloaded. This is not necessary the same list obtained from
    //        getResourcesToLoad() since probably we want to let some resources
    //        loaded after we do not use any more this state (for example, all
    //        those resources loaded into a shared data structure).
    // @param resourceList  The list of resources to be unloaded.
    // @return true on success | false otherwise
    //
    virtual bool
    getResourcesToUnload(ResourceGroupList& resourceList) = 0;

protected:
    MainStateEvent mEventInfo;

    // the global Ogre information
    static OgreCommon sOgreInfo;
    // the global VideoPlayer instance
    static mm::VideoPlayer* sVideoPlayer;
    // the global SoundManager instance
    static mm::SoundManager* sSoundManager;
    // static global resources handler for all main states
    static const rrh::ResourceHandler *sRcHandler;
};




////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//
inline const MainStateEvent&
IMainState::getEventInformation(void) const
{
    return mEventInfo;
}

} /* namespace cz */
#endif /* IMAINSTATE_H_ */
