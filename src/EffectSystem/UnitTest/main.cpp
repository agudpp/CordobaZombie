/*
 * Blood.h
 *
 *  Created on: Apr 1, 2012
 *      Author: Emi
 */


#include "Test.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif



#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
    	AppTester *appTester = new /* TODO: elejir aca que tipo sobrecargadoWeaponAppTester();*/ Test();

		appTester->loadAditionalData();
		if(appTester->run() != 0) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, "La app no termino bien", ":(", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "La app no termino bien :(" << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
