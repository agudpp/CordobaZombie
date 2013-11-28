/*
 * FESimpleButton.h
 *
 *  Created on: Oct 28, 2013
 *      Author: agustin
 */

#ifndef FESIMPLEBUTTON_H_
#define FESIMPLEBUTTON_H_

#include <functional>

#include <frontend/FEOgreElement.h>

namespace ui {

// Forward
//
class FEManager;

class FESimpleButton : public FEOgreElement<4>
{
public:
    // Define the possible states of the button. Note that when you set the
    // atlas coords you have to set it in the next order (State order):
    //
    enum State {
        S_NORMAL = 0,
        S_MOUSE_OVER,
        S_CLICKED,
        S_DISABLED,
    };

    // Define the possible events we will handle for the callback
    //
    enum Event {
        E_RIGHT_PRESSED = 0,
    };

    // Define the type of callback supported by this button
    //
    typedef std::function<void (FESimpleButton*, Event)> Callback;
public:
    FESimpleButton(FEManager* manager = 0, const core::AABB& aabb = core::AABB()) :
        FEOgreElement(manager, aabb)
    ,   mCurrentState(State::S_NORMAL)
    ,   mEnabled(true)
    {
    }
    virtual
    ~FESimpleButton()
    {}

    // @brief Set the callback to be used when the button is pressed (after it is
    //        released, not when the button is just clicked).
    // @param buttonPressedCb   The callback.
    //
    inline void
    setButtonPressedCallback(const Callback& cb);
    inline const Callback&
    buttonPressedCallback(void) const;

    // @brief Set (force) / Get current state of the button.
    //
    inline void
    setButtonState(State state);
    inline State
    buttonState(void) const;

    // @brief Enable / disable the button.
    // @param enabled
    //
    inline void
    setEnabled(bool enabled);
    inline bool
    isEnabled(void) const;


    ////////////////////////////////////////////////////////////////////////////
    // Events
    //

    // @brief This event is called whenever the mouse get inside over this element.
    // @param info      The information of the mouse and other stuff.
    //
    virtual void
    mouseIn(const FEInfo& info);

    // @brief This event is called when the mouse gets out from the element (in
    //        the past call the element was over this element).
    // @param info      The information of the mouse and other stuff.
    //
    virtual void
    mouseOut(const FEInfo& info);

    // @brief Event called all the frames that the mouse is over the element.
    // @param info      The information about the mouse buttons and other stuff.
    //
    virtual void
    mouseOver(const FEInfo& info);

    // @brief Method called to catch general events like key pressing and other
    //        stuff.
    // @param info      The associated information
    //
    virtual void
    onEvent(const FEInfo& info);

private:
    Callback mButtonPressedCallback;
    State mCurrentState;
    bool mEnabled;
};






////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
FESimpleButton::setButtonPressedCallback(const Callback& cb)
{
    mButtonPressedCallback = cb;
}
inline const FESimpleButton::Callback&
FESimpleButton::buttonPressedCallback(void) const
{
    return mButtonPressedCallback;
}

inline void
FESimpleButton::setButtonState(State state)
{
    // configure the uv coords
    setAtlasCoords(state);
    mCurrentState = state;
}
inline FESimpleButton::State
FESimpleButton::buttonState(void) const
{
    return mCurrentState;
}

inline void
FESimpleButton::setEnabled(bool enabled)
{
    if (mEnabled == enabled) {
        return;
    }
    mEnabled = enabled;
    if (enabled) {
        setAtlasCoords(State::S_NORMAL);
    } else {
        setAtlasCoords(State::S_DISABLED);
    }
}
inline bool
FESimpleButton::isEnabled(void) const
{
    return mEnabled;
}

} /* namespace ui */
#endif /* FESIMPLEBUTTON_H_ */
