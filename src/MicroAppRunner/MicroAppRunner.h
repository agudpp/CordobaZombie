/*
 * MicroAppRunner.h
 *
 *  Created on: 10/11/2011
 *      Author: agustin
 *
 */

#ifndef MICROAPPRUNNER_H_
#define MICROAPPRUNNER_H_


#include "DebugUtil.h"
#include "GlobalObjects.h"

class MicroApp;

#define MICROAR_FADER_TIME			1.0f
class MicroAppRunner {
public:

	/**
	 * Enqueue the MicroApp to be executed
	 */
    static inline void enqueueMicroApp(MicroApp *app);
    static inline bool hasMicroAppToRun(void);
    static inline void clearMicroAppsQueue(void);

	/* Run an micro app. This function blocks until the micro app finish.
	 * The MicroApp to run is the first that was enqueued.
	 * 1) app->load();
	 * 2) while(app->update(frametime) != DONE);
	 * 3) app->unload();
	 */
	static void runMicroApp(void);
private:
	static void showFade(bool begin);

private:
	// the last micro app enqueued
	static MicroApp *sMicroApp;
};




inline void MicroAppRunner::enqueueMicroApp(MicroApp *app)
{
	ASSERT(app);
	sMicroApp = app;
}
inline bool MicroAppRunner::hasMicroAppToRun(void)
{
	return sMicroApp != 0;
}
inline void MicroAppRunner::clearMicroAppsQueue(void)
{
    sMicroApp = 0;
}

#endif /* MICROAPPRUNNER_H_ */
