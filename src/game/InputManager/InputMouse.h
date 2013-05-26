/*
 * InputMouse.h
 *
 *  Created on: 15/09/2012
 *      Author: agustin
 *
 */

#ifndef INPUTMOUSE_H_
#define INPUTMOUSE_H_

#include "GlobalObjects.h"

namespace input {

enum MouseButtonID
{
	MB_Left = 0, MB_Right, MB_Middle,
	MB_Button3, MB_Button4,	MB_Button5, MB_Button6,	MB_Button7
};

class InputMouse {
public:
	static inline void capture(void);
	static inline bool isMouseDown(MouseButtonID btn);

	// positions
	static inline int absX(void);
	static inline int relX(void);
	static inline int absY(void);
	static inline int relY(void);
	static inline int absZ(void);
	static inline int relZ(void);
};


inline void InputMouse::capture(void)
{
	GLOBAL_MOUSE->capture();
}
inline bool InputMouse::isMouseDown(MouseButtonID btn)
{
	return GLOBAL_MOUSE->getMouseState().buttonDown(OIS::MouseButtonID(btn));
}

// positions
inline int InputMouse::absX(void)
{
	return GLOBAL_MOUSE->getMouseState().X.abs;
}

inline int InputMouse::relX(void)
{
	return GLOBAL_MOUSE->getMouseState().X.rel;
}
inline int InputMouse::absY(void)
{
	return GLOBAL_MOUSE->getMouseState().Y.abs;
}
inline int InputMouse::relY(void)
{
	return GLOBAL_MOUSE->getMouseState().Y.rel;
}
inline int InputMouse::absZ(void)
{
	return GLOBAL_MOUSE->getMouseState().Z.abs;
}
inline int InputMouse::relZ(void)
{
	return GLOBAL_MOUSE->getMouseState().Z.rel;
}

}

#endif /* INPUTMOUSE_H_ */
