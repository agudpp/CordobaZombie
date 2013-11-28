/*
 * ResourcesLoadingTest.h
 *
 *  Created on: 1/11/2013
 *      Author: raul
 */

#ifndef ResourcesLoading_H_
#define ResourcesLoading_H_


#include <vector>
#include <string>

#include <app_tester/AppTester.h>
#include <debug/DebugUtil.h>
#include <ResourceHandler.h>

#include <input/InputHelper.h>


namespace tests {

class ResourcesLoading : public core::AppTester
{
public:
	ResourcesLoading();

	~ResourcesLoading();

    /* Load additional info */
    void
    loadAditionalData(void);

    /* function called every frame. Use GlobalObjects::lastTimeFrame */
    void
    update();



private:

    float mTimeFrame;
    input::InputHelper 		mInputHelper;
    rrh::ResourceHandler 	*mRrh;
    rrh::ResourceGroup   	*mRg;
    Ogre::Overlay			*mOverlay;
};

}

#endif /* ResourcesLoading_H_ */
