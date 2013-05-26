/*
 * MainState.h
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#ifndef MAINSTATE_H_
#define MAINSTATE_H_

#include "IState.h"
#include "MenuButtonEffect.h"

namespace mm_states {

class MainState : public IState {

    // Enum the buttons
    enum {
        NewGameButtonIndex = 0,
        HistoryButtonIndex,
        CreditsButtonIndex,
        ConfigButtonIndex,
        ExitButtonIndex,
        NUMBER_BUTTONS, // only to know the number of buttons
    };

    static const char *BUTTONS_NAMES[NUMBER_BUTTONS];

    enum State {
        Looping,
        Exiting,
    };

public:
	MainState();
	virtual ~MainState();


    /**
     * Callback that receives the CbMenuButton and the Button id
     */
    void operator()(CbMenuButton *, CbMenuButton::ButtonID id);

    /**
     * Function called to load all the resources used by the state.
     * This function is called before we start the entering transition range
     */
    void load(void);

    /**
     * Function called right before we start the main loop (update()).
     * This function is called once and after that we start calling the update
     * function.
     */
    void beforeUpdate(void);

    /**
     * This function is called every frame, here we have to implement all the
     * state logic.
     */
    void update(void);

    /**
     * Function called once the state will be closed, so we have to unload all
     * the resources used by this class.
     */
    void unload(void);

    /**
     * This function have to be implemented to receive the input events from the
     * keyboard (for example, if the user press Escape or other key).
     * The Logic have to be implemented in the state itself and not in the
     * MainMenuState
     * @param   key     The keyboard pressed
     *
     * @note    We only will call this function with KeyPress event (only once
     *          if the user still pressing the key, this function will be called
     *          once).
     */
    void keyPressed(input::KeyCode key);

private:
    /**
     * Function called when we have to exit this state
     */
    void exitState(void);

    /**
     * Check the input events
     */
    void checkInput(void);

private:
    typedef std::vector<f_e::MenuButtonEffect>    MenuButtonEffVec;

    MenuButtonEffVec    mMenuButtonsEff;
    State   mState;
    Event   mReturnEvent;

};

}

#endif /* MAINSTATE_H_ */
