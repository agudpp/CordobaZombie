/*
 * CallbackMenuButton.h
 *
 *  Created on: May 4, 2013
 *      Author: agustin
 */

#ifndef CALLBACKMENUBUTTON_H_
#define CALLBACKMENUBUTTON_H_


#include <boost/shared_ptr.hpp>
#include <boost/signal.hpp>

#include "MenuButton.h"

class CallbackMenuButton : public MenuButton
{
public:

    // The event type
    //
    enum ButtonID {
        RIGHT_BUTTON = 0,
        LEFT_BUTTON,
    };

    // Define the connection and signals type
    //
    typedef boost::signal<void (CallbackMenuButton *, ButtonID)> Signal;
    typedef boost::signals::scoped_connection Connection;

public:
    CallbackMenuButton();
    virtual
    ~CallbackMenuButton();

    /**
     * Set the callback to be used
     */
    inline Connection
    addCallback(const Signal::slot_type& subscriber);

protected:
    ////////////////////////////////////////////////////////////////////////////
    ////                        CALLBACKS TO MPLEMENT                       ////
    ////////////////////////////////////////////////////////////////////////////

    /**
     * When the button is pressed with right button (after is released)
     */
    virtual void rightPressed(void);

    /**
     * When the button is left Pressed
     */
    virtual void leftPressed(void);

private:
    Signal mSignal;

};


// Define shared ptr
//
typedef boost::shared_ptr<CallbackMenuButton> CallbackMenuButtonPtr;


// Inline impl
//
inline CallbackMenuButton::Connection
CallbackMenuButton::addCallback(const Signal::slot_type& subscriber)
{
    return mSignal.connect(subscriber);
}
#endif /* CALLBACKMENUBUTTON_H_ */
