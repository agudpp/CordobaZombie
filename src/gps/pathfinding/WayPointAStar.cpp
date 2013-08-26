/*
 * WayPointAStar.cpp
 *
 *  Created on: Aug 24, 2013
 *      Author: agustin
 */

#include "WayPointAStar.h"

#include <debug/DebugUtil.h>
#include <types/StackPriorityQueue.h>
#include <types/StackVector.h>

#include "WayPointGraph.h"

// Helper stuff
//
namespace {

#define EUCLIDEAN_DISTANCE(p1, p2)  ((p1.x - p2.x) * (p1.x - p2.x) + \
        (p1.y - p2.y) * (p1.y - p2.y))

// Structure used to compare the nodes, this is NOT MULTI-thread!
//
struct PriorityNodeCmp {
    static float* gValue;
    static float* fValue;

    // @brief Compare less operator
    //
    inline bool
    operator()(const gps::index_t& a, const gps::index_t& b) const
    {
        return (gValue[a] + fValue[a]) >= (gValue[b] + fValue[b]);
    }
};
float* PriorityNodeCmp::gValue = 0;
float* PriorityNodeCmp::fValue = 0;

// @brief Define the heuristic_function here for two nodes
//
inline float
heuristic_function(const gps::WayPointNode& n1, const gps::WayPointNode& n2)
{
    return EUCLIDEAN_DISTANCE(n1.position, n2.position);
}

}

namespace gps {


////////////////////////////////////////////////////////////////////////////////
WayPointAStar::WayPointAStar() :
    mNodes(0)
,   mNodesCount(0)
{

}

////////////////////////////////////////////////////////////////////////////////
WayPointAStar::~WayPointAStar()
{

}

////////////////////////////////////////////////////////////////////////////////
void
WayPointAStar::setGraph(const WayPointGraph* graph)
{
    ASSERT(graph);
    ASSERT(graph->nodes().data);
    ASSERT(graph->nodes().size > 0);

    mNodes = graph->nodes().data;
    mNodesCount = graph->nodes().size;
    mClosedSet.setSize(mNodesCount);
}

////////////////////////////////////////////////////////////////////////////////
WayPointPath::Type
WayPointAStar::getPath(const index_t startIndex,
                       const index_t endIndex,
                       WayPointPath& path)
{
    ASSERT(startIndex < mNodesCount);
    ASSERT(endIndex < mNodesCount);

    if (startIndex == endIndex) {
        return WayPointPath::Type::SAME_POINT;
    }

    // get the nodes
    const WayPointNode& startNode = mNodes[startIndex];
    const WayPointNode& endNode = mNodes[endIndex];

    // now we will execute the A* algorithm.
    // Create the data structures needed.
    // the g_score and f_score shouldn't be initialized neither since
    // we will only use values that were already wrote.
    //
    float g_score[MAX_NUM_NODES_TO_VISIT];
    float f_score[MAX_NUM_NODES_TO_VISIT];
    PriorityNodeCmp::gValue = g_score;
    PriorityNodeCmp::fValue = f_score;
    core::StackPriorityQueue<index_t, MAX_NUM_NODES_TO_VISIT, PriorityNodeCmp> openSet;
    // note that the above cameFromMap it is not necessary to be initialized each
    // time since we will only set the values we need
    index_t cameFromMap[MAX_NUM_NODES_TO_VISIT];


    // reset the closedSet and the mOpenSetChecker
    mClosedSet.newRound();
    mOpenSetChecker.newRound();

    // init the main values.
    g_score[startIndex] = 0;
    f_score[startIndex] = heuristic_function(startNode, endNode);
    openSet.push(startIndex);

    // now start to move from start to end
    while (!openSet.empty()) {
        const index_t currentIndex = openSet.top();
        const WayPointNode& currentNode = mNodes[currentIndex];

        if (currentIndex == endIndex) {
            // done
            break;
        }

        // mark this as already visited (this is the same that put it in the
        // closed set) and removed it from the openSet.
        //
        openSet.pop();
        mClosedSet.mark(currentIndex);
        mOpenSetChecker.unmark(currentIndex);

        // now we will iterate for each neighbor of the current node
        for (unsigned int i = 0, size = currentNode.neighborsCount; i < size; ++i) {
            const index_t neighborIndex = currentNode.neighbors[i];
            const WayPointNode& neighborNode = mNodes[neighborIndex];

            // here we should do any checking with the current neighbor, for example
            // if we test the max radius that is able to pass from currentNode
            // to neighbor we should discard the neighbors here.
            //

            // if we are here then it means that the neighbor it is a possible
            // candidate to be analyzed. We will check the distance using the
            // distance and the heuristic function
            const float tentativeGScore = g_score[currentIndex] +
                EUCLIDEAN_DISTANCE(currentNode.position, neighborNode.position);

            // check if we have to skip this node or not
            const bool isNeighborInClosedSet = mClosedSet.isMarked(neighborIndex);
            if (isNeighborInClosedSet &&
                    tentativeGScore >= g_score[neighborIndex]) {
                // skip it, it is already marked and the tentative score is not
                // better
                continue;
            }

            // if the node wasn't already checked and the tentative g score is
            // better, we will add this node to be analized
            //
            if (!isNeighborInClosedSet ||
                    (tentativeGScore < g_score[neighborIndex])) {
                // add to the map and set the values for this neighbor (g and f)
                //
                cameFromMap[neighborIndex] = currentIndex;
                g_score[neighborIndex] = tentativeGScore;
                f_score[neighborIndex] = g_score[neighborIndex] +
                    heuristic_function(neighborNode, endNode);
                if (!mOpenSetChecker.isMarked(neighborIndex)) {
                    mOpenSetChecker.mark(neighborIndex);
                    openSet.push(neighborIndex);
                }
            }
        }
    }

    // clear the resulting path
    path.size = 0;

    // reconstruct the path now in the inverse order
    index_t index = endIndex;
    index_t current = cameFromMap[index];
    while (current != startIndex && path.size < WayPointPath::MAX_PATH_SIZE) {
        ASSERT(cameFromMap[index] < mNodesCount);
        current = cameFromMap[index];
        path.node[path.size] = mNodes[current].position;
        ++path.size;
    }

    if (current != startIndex) {
        // we couldn't get the path?
        return WayPointPath::Type::IMPOSSIBLE;
    }
    return WayPointPath::Type::NORMAL;
}

} /* namespace gps */
