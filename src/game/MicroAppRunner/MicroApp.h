/* Used for apps where the efficiency is not important
 *
 * MicroApp.h
 *
 *  Created on: 10/11/2011
 *      Author: agustin
 *
 *
 * OpenVC License
 *
 * Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
 * Copyright (C) 2009, Willow Garage Inc., all rights reserved.
 * Third party copyrights are property of their respective owners.
 *
 * This software is provided by the copyright holders and contributors "as is" and
 * any express or implied warranties, including, but not limited to, the implied
 * warranties of merchantability and fitness for a particular purpose are disclaimed.
 * In no event shall the Intel Corporation or contributors be liable for any direct,
 * indirect, incidental, special, exemplary, or consequential damages
 * (including, but not limited to, procurement of substitute goods or services;
 * loss of use, data, or profits; or business interruption) however caused
 * and on any theory of liability, whether in contract, strict liability,
 * or tort (including negligence or otherwise) arising in any way out of
 * the use of this software, even if advised of the possibility of such damage.
 */

#ifndef MICROAPP_H_
#define MICROAPP_H_

#include <OgreString.h>

#include "MicroAppRunner.h"

class MicroApp {
public:
	typedef enum {
		DONE	= 0,	// when the microApp finish
		CONTINUE		// when the microApp must continue be updating
	} microAppCode;
public:
	MicroApp() {};
	virtual ~MicroApp() {};

	/**
	 * Execute the microApp
	 */
	inline void execute(void);

	/* Loads the app */
	virtual void load(void) = 0;

	/* The function used to update the MicroApp in every frame */
	virtual microAppCode update(Ogre::Real frameTime) = 0;

	/* Unloads the app */
	virtual void unload(void) = 0;

private:
	MicroApp(const MicroApp &);
};

/**
 * Execute the microApp
 */
inline void MicroApp::execute(void)
{
	MicroAppRunner::enqueueMicroApp(this);
}

#endif /* MICROAPP_H_ */
