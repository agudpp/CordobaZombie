/*
 * MainStateInformation.h
 *
 *  Created on: Oct 29, 2013
 *      Author: agustin
 */

#ifndef MAINSTATEINFORMATION_H_
#define MAINSTATEINFORMATION_H_


#include <string>
#include <functional>

#include <types/StackVector.h>
#include <ResourceGroup.h>


// forward
//
namespace Ogre {
class SceneManager;
class RenderWindow;
class Root;
class Camera;
}
namespace ui {
class FEManager;
}


namespace cz {

// Here we will define a common class to be used for all the states to get
// the information about global variables like current resource path, current
// level, etc.
//

struct MainStateInformation {
    std::string currentPath;
};


// Here we will define all the common Ogre classes that the states should have.
//
struct OgreCommon {
    Ogre::SceneManager* sceneMngr;
    Ogre::RenderWindow* renderWindow;
    Ogre::Root* root;
    Ogre::Camera* camera;

    OgreCommon() :
        sceneMngr(0)
    ,   renderWindow(0)
    ,   root(0)
    ,   camera(0)
    {}
};

// Here we will define the global shared handlers we will use
//
struct CommonHandlers {
    ui::FEManager* frontEndManager;

    CommonHandlers() :
        frontEndManager(0)
    {}
};

// We will define the list of resources that a state can handle.
// For now we will be sure than we are not handling more than 128 files at
// once.
//
//typedef std::reference_wrapper<rrh::ResourceGroup> ResourceGroupWrapper;
typedef core::StackVector<rrh::ResourceGroup, 128> ResourceGroupList;

}


#endif /* MAINSTATEINFORMATION_H_ */
