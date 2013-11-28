/*
 * levelEditor.cpp
 *
 *  Created on: 23/12/2010
 *      Author: agustin
 */

#include <string>
#include <vector>

#include "CLICollisionExporter.h"

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
    	core::CommandLine *cmdLineTool = new tool::CLICollisionExporter;

    	// parse arguments
    	std::vector<std::string> arguments;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    	debugERROR("TODO this for windows\n");
    	ASSERT(false && "NOT IMPLEMENTED YET!");
#else
    	arguments.reserve(argc);
    	for (int i = 0; i < argc; ++i) {
    	    arguments.push_back(argv[i]);
    	}
#endif

    	// execute the tool
    	return cmdLineTool->execute(arguments);
    }

#ifdef __cplusplus
}
#endif
