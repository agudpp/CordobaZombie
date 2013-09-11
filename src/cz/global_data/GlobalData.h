/*
 * GlobalData.h
 *
 *  Created on: Sep 10, 2013
 *      Author: agustin
 */

#ifndef GLOBALDATA_H_
#define GLOBALDATA_H_


// Forward stuff
//
namespace coll {
class CollisionHandler;
}

namespace cz {

struct GlobalData
{
    // Collision handler used
    static coll::CollisionHandler* collHandler;
    // last frame elapsed time
    static float lastTimeFrame;

};

} /* namespace cz */
#endif /* GLOBALDATA_H_ */
