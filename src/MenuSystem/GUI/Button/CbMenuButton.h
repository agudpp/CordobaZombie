/* Implementation of the MenuButton but using callbacks
 * CbMenuButton.h
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#ifndef CBMENUBUTTON_H_
#define CBMENUBUTTON_H_

#include "MenuButton.h"

class CbMenuButton : public MenuButton {
public:

	enum ButtonID {
		RIGHT_BUTTON = 0,
		LEFT_BUTTON,
	};

	class Cb {
	public:
		/**
		 * Callback that receives the CbMenuButton and the Button id
		 */
		virtual void operator()(CbMenuButton *, ButtonID) = 0;
	};
public:
	CbMenuButton();
	virtual ~CbMenuButton();

	/**
	 * Set the callback to be used
	 */
	inline void setCallback(Cb *cb);
	inline Cb *getCallback(void);

protected:
	////////////////////////////////////////////////////////////////////////////
	////						CALLBACKS TO MPLEMENT						////
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
	Cb		*mCb;

};

inline void CbMenuButton::setCallback(Cb *cb)
{
	mCb = cb;
}
inline CbMenuButton::Cb *CbMenuButton::getCallback(void)
{
	return mCb;
}

#endif /* CBMENUBUTTON_H_ */
