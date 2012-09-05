/*
 * HudElement.h
 *
 *  Created on: 16/06/2012
 *      Author: agustin
 */

#ifndef HUDELEMENT_H_
#define HUDELEMENT_H_

class HudElement
{
public:
	HudElement();
	virtual ~HudElement();

	/**
	 * Load/Unload all the resources of the HudElement
	 */
	virtual void load(void) = 0;
	virtual void unload(void) = 0;

	/**
	 * Hide/Show the element
	 */
	virtual void show(void) = 0;
	virtual void hide(void) = 0;

};

#endif /* HUDELEMENT_H_ */
