/*
 * MainStateBuilder.cpp
 *
 *  Created on: Oct 29, 2013
 *      Author: agustin
 */

#include "MainStateBuilder.h"

#include <debug/DebugUtil.h>
#include <game_states/states/ExitMainState/ExitMainState.h>
#include <game_states/states/InfoMainState/InfoMainState.h>
#include <game_states/states/IntroMainState/IntroMainState.h>
#include <game_states/states/MenuMainState/MenuMainState.h>
#include <game_states/states/PlayingMainState/PlayingMainState.h>
#include <game_states/states/PrePlayIntroMainState/PrePlayIntroMainState.h>


#include "IMainState.h"
#include "MainStateID.h"

namespace cz {

MainStateBuilder::MainStateBuilder()
{

}

MainStateBuilder::~MainStateBuilder()
{
}

////////////////////////////////////////////////////////////////////////////////
void
MainStateBuilder::setOgreCommon(const OgreCommon& ogreData)
{
    // set this data to the states
    IMainState::setOgreData(ogreData);
}

////////////////////////////////////////////////////////////////////////////////
IMainState*
MainStateBuilder::buildState(const MainStateID& id) const
{
    IMainState* result = 0;

    switch (id) {
    case MainStateID::InfoState:
        result = new InfoMainState;
        break;
    case MainStateID::IntroState:
        result = new IntroMainState;
        break;
    case MainStateID::MenuState:
        result = new MenuMainState;
        break;
    case MainStateID::PrePlayingState:
        result = new PrePlayIntroMainState;
        break;
    case MainStateID::PlayingState:
        result = new PlayingMainState;
        break;
    case MainStateID::ExitState:
        result = new ExitMainState;
        break;
    }

    if (result == 0) {
        debugERROR("We are not supporting the ID type %d right now\n", id);
    }
    return result;
}

} /* namespace cz */
