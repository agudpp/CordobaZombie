/*
 * GameRunner.h
 *
 *  Created on: Nov 28, 2013
 *      Author: agustin
 */

#ifndef GAMERUNNER_H_
#define GAMERUNNER_H_

#include <Engine.h>
#include <input/InputHelper.h>
#include <cursor/MouseCursor.h>
#include <frontend/FEManager.h>
#include <game_states/MainStateBuilder.h>
#include <game_states/MainStateTT.h>
#include <game_states/MainStateMachine.h>

namespace cz {

class GameRunner
{
public:
    GameRunner(engine::Engine& engine);
    ~GameRunner();

    // @brief This method will run the game and will block until we don't
    //        want to show anything else
    // return true on success | false otherwise
    //
    bool
    run(void);


private:
    // @brief Initialize everything we need to run the engine. This method
    //        will call all the other init methods and will return true on success
    //        or false if some of the init methods fails.
    // @return true on success | false otherwise
    //
    bool
    initAll(void);

    // Init methods
    //
    bool
    initGlobalResources(void);
    bool
    initMouseCursor(void);
    bool
    initFrontEnd(void);
    bool
    initInputHelper(void);
    bool
    initGlobalData(void);
    bool
    initMainStateMachine(void);

private:
    engine::Engine& mEngine;
    // General handlers / helpers / managers
    input::InputHelper mInputHelper;
    ui::MouseCursor mMouseCursor;
    ui::FEManager mFrontEndManager;
    // Main state machine
    MainStateBuilder mStatesBuilder;
    MainStateTT mTransitionTable;
    MainStateMachine mMainStateMachine;
    // local variables
    bool mContinueRunning;
};

} /* namespace cz */
#endif /* GAMERUNNER_H_ */
