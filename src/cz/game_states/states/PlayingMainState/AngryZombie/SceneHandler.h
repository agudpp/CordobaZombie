/*
 * SceneHandler.h
 *
 *  Created on: Dec 3, 2013
 *      Author: agustin
 */

#ifndef SCENEHANDLER_H_
#define SCENEHANDLER_H_


#include <string>
#include <string.h>

#include "Player.h"


namespace demo_app {

class SceneHandler
{
public:

    struct Data {
        Player* player;


        Data()
        {
            memset(this, 0, sizeof(*this));
        }
    };
public:
    SceneHandler();
    ~SceneHandler();

    // @brief Set the data we need to be able to work correctly here.
    //
    void
    setData(const Data& data);

    // @brief Parse the scenes information.
    // @param scenes        File path to the scenes information
    //
    bool
    parseSceneInformation(const std::string& scenes);

    // @brief Configure / reset the current scene.
    //
    void
    configureCurrentScene(void);

    // @brief Go to next scene. This method will not configure the scene.
    // @return true if we could go to next scene or not
    //
    bool
    goToNextScene(void);


private:
    Data mData;
};

} /* namespace demo_app */
#endif /* SCENEHANDLER_H_ */
