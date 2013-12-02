/*
 * MainMenuSubState.h
 *
 *  Created on: Nov 22, 2013
 *      Author: agustin
 */

#ifndef MAINMENUSUBSTATE_H_
#define MAINMENUSUBSTATE_H_

#include <OgreString.h>

#include <game_states/MainStateInformation.h>



// @brief Define the interface of this class (for the MainMenu SubStates).
//        Each sub state will be able to load / unload its resources like any
//        other MainState. The calling method flow will be:
//        getResourcesToLoad(r) -> load() ->
//          multiple times: show() -> update() (until finish) -> hide() ->
//        unload() -> getResourcesToUnload(r).
//


// forward
//
namespace mm {
class SoundHandler;
};

namespace cz {

// @brief Define the SubState possible events
//
enum MainMenuSubStateEvent {
    MMSSE_CONTINUE = 0,     // This means that the current state should continue
                            // being executed
    MMSSE_DONE,
    MMSSE_CREDITS,
    MMSSE_HELP,
    MMSSE_HISTORY,
    MMSSE_PLAY_GAME,
    MMSSE_EXIT_GAME,

    MMSSE_COUNT,
};

class MainMenuSubState
{
public:
    MainMenuSubState();
    virtual
    ~MainMenuSubState();

    // @brief Set the OgreCommon information to be used by all the States
    // @param ogreData      The ogre Data.
    //
    static void
    setOgreData(const OgreCommon& ogreData);

    // @brief Set the SoundHandler global instance
    // @param soundHandler  The SoundManager.
    //
    static void
    setSoundHandler(mm::SoundHandler* soundHandler);

    // @brief Set the CommonHandlers structure for all main states.
    // @param ch      The common handlers structure with all the handlers
    //
    static void
    setCommonHandlers(const CommonHandlers& ch);

    // @brief Return the list of resources used by this sub-state.
    //        This list should contain all the resources that we need to use
    //        in this sub-state.
    // @param resourceList      The resulting resource list we will need to load
    //                          to be able to execute the sub-state correctly.
    // @return true on success | false otherwise.
    //
    virtual bool
    getResourcesToLoad(ResourceGroupList& resourceList) = 0;

    // @brief This method will be called after all the resources were loaded
    //        correctly. This is the method called before starting the update()
    //        loop and here we should load all we need for the sub-state.
    // @return true on success | false otherwise
    //
    virtual bool
    load(void) = 0;

    // @brief This method will show all the data already loaded for this sub-state
    // @return true on success | false otherwise
    //
    virtual bool
    show(void) = 0;

    // @brief This is the main method that will be called each frame with the
    //        last time frame as parameter. Here you should do whatever you need
    //        to update the logic of the sub-state, and after this call the scene
    //        will be rendered (OgreScene).
    //        This method will also return the associated MainMenuSubStateEvent.
    // @param timeFrame     The last time frame.
    // @return MainMenuSubStateEvent
    //
    virtual MainMenuSubStateEvent
    update(float timeFrame) = 0;

    // @brief This method will be called to hide the current sub-state information
    //        but not to destroy the data.
    // @returns true on success | false otherwise
    //
    virtual bool
    hide(void) = 0;

    // @brief This method will be called to unload the current sub-state to destroy
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

private:
    // avoid copying
    MainMenuSubState(const MainMenuSubState&);
    MainMenuSubState& operator=(const MainMenuSubState&);

protected:

    // @brief Helper method used to get the absolute path from a relative path.
    // @param relPath       The relative path to be transformed in absolute path
    // @returns true on success and in the relPath arg the absolute path for
    //          the given relPath.
    //
    bool
    convertToAbsolute(Ogre::String& relPath) const;

protected:
    // The OgreStuff information and the SoundSystem associated
    //
    // the global Ogre information
    static OgreCommon sOgreInfo;
    // the global SoundHandler instance
    static mm::SoundHandler* sSoundHandler;
    // static global general shared handlers
    static CommonHandlers sCommonHandlers;
};

} /* namespace cz */
#endif /* MAINMENUSUBSTATE_H_ */
