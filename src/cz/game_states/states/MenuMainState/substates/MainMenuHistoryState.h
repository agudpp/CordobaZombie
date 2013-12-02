/*
 * MainMenuHistoryState.h
 *
 *  Created on: Nov 22, 2013
 *      Author: agustin
 */

#ifndef MAINMENUHISTORYSTATE_H_
#define MAINMENUHISTORYSTATE_H_

#include <OgreOverlay.h>

#include <frontend/element/button/FESimpleButton.h>
#include <helpers/slide_player/SlidePlayer.h>
#include <types/StackVector.h>

#include "MainMenuSubState.h"




namespace cz {

class MainMenuHistoryState : public MainMenuSubState
{
    // internal enum for buttons
    enum Buttons {
        NEXT_SLIDE = 0,
        PREV_SLIDE,
        BACK,

        COUNT,
    };
public:
    MainMenuHistoryState();
    virtual
    ~MainMenuHistoryState();

    ////////////////////////////////////////////////////////////////////////////
    // Signals
    //
    // @brief Whenever a button is pressed this method will be called. This is
    //        the "callback" for the FESimpleButton.
    //
    void
    buttonPressed(ui::FESimpleButton* button, ui::FESimpleButton::Event event);

    ////////////////////////////////////////////////////////////////////////////
    //                          Inherited methods                             //
    ////////////////////////////////////////////////////////////////////////////


    // @brief Return the list of resources used by this sub-state.
    //        This list should contain all the resources that we need to use
    //        in this sub-state.
    // @param resourceList      The resulting resource list we will need to load
    //                          to be able to execute the sub-state correctly.
    // @return true on success | false otherwise.
    //
    bool
    getResourcesToLoad(ResourceGroupList& resourceList);

    // @brief This method will be called after all the resources were loaded
    //        correctly. This is the method called before starting the update()
    //        loop and here we should load all we need for the sub-state.
    // @return true on success | false otherwise
    //
    bool
    load(void);

    // @brief This method will show all the data already loaded for this sub-state
    // @return true on success | false otherwise
    //
    bool
    show(void);

    // @brief This is the main method that will be called each frame with the
    //        last time frame as parameter. Here you should do whatever you need
    //        to update the logic of the sub-state, and after this call the scene
    //        will be rendered (OgreScene).
    //        This method will also return the associated MainMenuSubStateEvent.
    // @param timeFrame     The last time frame.
    // @return MainMenuSubStateEvent
    //
    MainMenuSubStateEvent
    update(float timeFrame);

    // @brief This method will be called to hide the current sub-state information
    //        but not to destroy the data.
    // @returns true on success | false otherwise
    //
    bool
    hide(void);

    // @brief This method will be called to unload the current sub-state to destroy
    //        all the instances we use and will not be used anymore, but here
    //        we shouldn't uninitialize any resource for example, that will be
    //        done automatically in getResourcesToUnload().
    // @returns true on success | false otherwise
    //
    bool
    unload(void);

    // @brief This method should return the list of all the resources that should
    //        be unloaded. This is not necessary the same list obtained from
    //        getResourcesToLoad() since probably we want to let some resources
    //        loaded after we do not use any more this state (for example, all
    //        those resources loaded into a shared data structure).
    // @param resourceList  The list of resources to be unloaded.
    // @return true on success | false otherwise
    //
    bool
    getResourcesToUnload(ResourceGroupList& resourceList);

private:

    // @brief Get all the materials we can found an put it in the vector of
    //        slides.
    // @param slides        The resulting vector with the slides names to be
    //                      fill.
    //
    void
    getSlides(SlidePlayer::SlidesVec& slides);

private:
    Ogre::Overlay* mOverlay;
    core::StackVector<ui::FESimpleButton, Buttons::COUNT> mButtons;
    SlidePlayer mSlidePlayer;
    MainMenuSubStateEvent mRetVal;

};

} /* namespace cz */
#endif /* MAINMENUHISTORYSTATE_H_ */
