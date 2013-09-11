/*
 * StaticDataLoader.h
 *
 *  Created on: Sep 10, 2013
 *      Author: agustin
 */

#ifndef STATICDATALOADER_H_
#define STATICDATALOADER_H_

#include <pathfinding/WPPathfinder.h>
#include <collisions/CollisionHandler.h>

namespace cz {

// @brief This class will put all the static data together and will initialize
//        Everything we need to run the game.
//        It will also set the GlobalData pointers.
//

struct StaticDataLoader
{
    // @brief The constructor will initialize the classes that depends from
    //        the static instance.
    StaticDataLoader();
    ~StaticDataLoader();

    gps::WPPathfinder pathfinder;
    coll::CollisionHandler collHandler;
};

} /* namespace cz */
#endif /* STATICDATALOADER_H_ */
