/*
 * WayPointAStar.h
 *
 *  Created on: Aug 24, 2013
 *      Author: agustin
 */

#ifndef WAYPOINTASTAR_H_
#define WAYPOINTASTAR_H_

#include <types/BoolCountingMask.h>

namespace gps {

// Forward
//
class WayPointGraph;
class WayPointNode;


// Useful classes
//

struct WayPointPath {
    // some useful defines
    //
    static const unsigned int MAX_PATH_SIZE = 25;

    enum Type {
        SAME_POINT = 0, // startPoint = endPoint
        IMPOSSIBLE,     // there is no possible path from start -> end
    };

    core::uint32_t size;
    WayPointNode* node[MAX_PATH_SIZE];
};

class WayPointAStar
{
public:
    WayPointAStar();
    ~WayPointAStar();

    // @brief Set the graph pointer we will use.
    // @param graph the WayPointGraph to be used.
    // @note The graph shouldn't be removed (freed its memory) until you
    //       stop using this class.
    //
    void
    setGraph(const WayPointGraph* graph);

    // @brief Perform the A* algorithm given 2 nodes (start, end).
    // @param start     The first node from where we start.
    // @param end       The last node where we want to go (end).
    // @param path      The resulting path to be filled
    // @return WayPointPath::Type
    //
    WayPointPath::Type
    getPath(const WayPointNode& start, const WayPointNode& end, WayPointPath& path);

private:
    const WayPointNode* mNodes;
    core::BoolCountingMask mMask;
};

} /* namespace gps */
#endif /* WAYPOINTASTAR_H_ */
