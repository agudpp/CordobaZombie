/*
 * InputMouse.h
 *
 *  Created on: Jul 22, 2013
 *      Author: agustin
 */

#ifndef INPUTMOUSE_H_
#define INPUTMOUSE_H_

#include <OIS/OISMouse.h>

#include <debug/DebugUtil.h>


namespace input {

enum MouseButtonID
{
    MB_Left = 0,
    MB_Right,
    MB_Middle,
    MB_Button3,
    MB_Button4,
    MB_Button5,
    MB_Button6,
    MB_Button7
};

class Mouse {
public:
    static inline void
    capture(void);

    static inline bool
    isMouseDown(MouseButtonID btn);

    // positions
    static inline int
    absX(void);
    static inline int
    relX(void);
    static inline int
    absY(void);
    static inline int
    relY(void);
    static inline int
    absZ(void);
    static inline int
    relZ(void);

    // @brief We should set first the mouse pointer to be used
    // @param mouse     The mouse ptr
    static inline void
    setMouse(OIS::Mouse* mouse);

private:
    static OIS::Mouse* sMouse;
};


////////////////////////////////////////////////////////////////////////////////
// Inline stuff

inline void
Mouse::capture(void)
{
    ASSERT(sMouse);
    sMouse->capture();
}
inline bool
Mouse::isMouseDown(MouseButtonID btn)
{
    ASSERT(sMouse);
    return sMouse->getMouseState().buttonDown(OIS::MouseButtonID(btn));
}

// positions
inline int
Mouse::absX(void)
{
    ASSERT(sMouse);
    return sMouse->getMouseState().X.abs;
}

inline int
Mouse::relX(void)
{
    ASSERT(sMouse);
    return sMouse->getMouseState().X.rel;
}
inline int
Mouse::absY(void)
{
    ASSERT(sMouse);
    return sMouse->getMouseState().Y.abs;
}
inline int
Mouse::relY(void)
{
    ASSERT(sMouse);
    return sMouse->getMouseState().Y.rel;
}
inline int
Mouse::absZ(void)
{
    ASSERT(sMouse);
    return sMouse->getMouseState().Z.abs;
}
inline int
Mouse::relZ(void)
{
    ASSERT(sMouse);
    return sMouse->getMouseState().Z.rel;
}

inline void
Mouse::setMouse(OIS::Mouse* mouse)
{
    ASSERT(mouse);
    sMouse = mouse;
}

}

#endif /* INPUTMOUSE_H_ */

