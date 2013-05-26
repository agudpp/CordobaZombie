/*
 * TriangleAStar.cpp
 *
 *  Created on: 24/01/2012
 *      Author: agustin
 */

#include <queue>
#include <set>
#include <bitset>
#include "MultiplatformTypedefs.h"
#include <algorithm>


#include "DebugUtil.h"
#include "TriangleAStar.h"



#define EUCLIDEAN_DISTANCE(p1, p2)	((p1->x - p2->x) * (p1->x - p2->x) + \
        (p1->y - p2->y) * (p1->y - p2->y))



// Helper functions / data / etc
//
namespace {

// Data
//
// this will be used to the bitset size
static const int MAX_PATH_SIZE  =   150;
static const int CACHE_SIZE     =   50;


// Helper class
template<class T, size_t N = MAX_PATH_SIZE>
class AuxContainer {
public:
    AuxContainer():ptr(0){};

    inline T *
    getFreshObjt(void)
    {
        ASSERT(ptr < N);
        return &objs[ptr++];
    }

    inline T *
    getFreshObjt(size_t &id)
    {
        ASSERT(ptr < N);
        id = ptr;
        return &objs[ptr++];
    }

    inline T *
    operator[](size_t i)
    {
        ASSERT(i < ptr);
        return &objs[i];
    }

    inline bool
    isIDValid(size_t id) const
    {
        return id < ptr;
    }

    inline void
    restart(void)
    {
        ptr = 0;
    }

    inline size_t
    getID(const T *elem) const
    {
        return static_cast<size_t>(elem - &objs[0]);
    }


private:
    size_t ptr;
    T objs[N];
};

// Helper container to put the priority nodes
struct PriorityNode {
    PriorityNode(){};
    inline PriorityNode(TriangleAStar::NodePtr n, float h_value, float g_value) :
        node(n),
        hValue(h_value),
        gValue(g_value)
    {
    }

    inline float getFValue(void) const
    {
        return gValue + hValue;
    }

//      inline bool operator<(const PriorityNode &other) const
//      {
//          return getFValue() > other.getFValue();
//      }

    float gValue;
    float hValue;
    TriangleAStar::NodePtr node;
};
class priorityNodeCompare
{
public:
    bool operator()(const PriorityNode *lpn, const PriorityNode *rpn)
    {
        return lpn->getFValue() > rpn->getFValue();
    }
};



// TODO: mejorar estas funciones, deberiamos obtener el punto mas cerca
// a lo largo de todo el edge, por el momento solo nos fijamos en los 2
// vertices
static inline float
heuristic_function(const GEdge *edge, const sm::Point *goal)
{
    // get the closest sm::Point between the two sm::Points of the edge and the goal sm::Point
    const sm::Vertex *const *Vertexs = edge->getSharedVertex();
    float d1 = EUCLIDEAN_DISTANCE(goal, Vertexs[0]);
    float d2 = EUCLIDEAN_DISTANCE(goal, Vertexs[1]);
    return (d1 < d2) ? d1 : d2;
}

static inline float
g_function(const GEdge *edge, const sm::Point *start)
{
    const sm::Vertex *const *Vertexs = edge->getSharedVertex();
    float d1 = EUCLIDEAN_DISTANCE(start, Vertexs[0]);
    float d2 = EUCLIDEAN_DISTANCE(start, Vertexs[1]);
    return (d1 < d2) ? d1 : d2;
}



inline float
nodeCost(TriangleAStar::NodePtr n1, TriangleAStar::NodePtr n2)
{
    // check the cost of go from n1 to n2
//	if(n1->getEdge1() && n1->e1->getNeighbor(n1) == n2){
//		return n1->e1->distance;
//	} else if(n1->e2 && n1->e2->getNeighbor(n1) == n2){
//		return n1->e2->distance;
//	} else if(n1->e3 && n1->e3->getNeighbor(n1) == n2){
//		return n1->e3->distance;
//	} else{
//		// it is not a neighbor, return the euclidean distance
//		return n1->getSquaredDistance(*n2);
//	}
    return 0.0f;
}

}

//		Here we will define the g, h, and f functions used in the A*




////////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG_CRITICAL
    long long int CacheCount = 1;
    long long int pathQueries = 1;
#endif

const TriangleAStar::Path
TriangleAStar::getCachePath(NodePtr n1, NodePtr n2, size_t &size) const
{
    // look for it in the cache
    bool fwd = true;
    int len = 0;
    const TriangleAStar::Path cp = mCache.getPath( (const GNode *)n1,
            (const GNode *)n2, fwd, size);

    // TODO los Path terminan en Null. Cambiar GNode** por vector<GNode *>

#ifdef DEBUG_CRITICAL
    pathQueries++;
    if(cp)
        debugRAUL("Cache hit: Cache hits count: %ld from %ld queries in "
                "total.\n", CacheCount++, pathQueries);
#endif

    if(cp and !fwd){
        std::reverse(cp, cp+len);
    }
    return cp;
}

////////////////////////////////////////////////////////////////////////////////
void
TriangleAStar::addToCache(Path p, size_t size,
                          NodePtr n1, NodePtr n2,
                          float radius) const
{
    ASSERT(n1);
    ASSERT(n2);
    ASSERT(size > 0);

    mCache.addPath(n1, n2,p);

    return;
}


////////////////////////////////////////////////////////////////////////////////
void
TriangleAStar::findPath(size_t &size,
                        NodePtr start, NodePtr end,
                        const sm::Point &startP, const sm::Point &endP,
                        float radius,
                        float &min)
{
    ASSERT(start);
    ASSERT(end);

    static AuxContainer<PriorityNode> priorityNodes;
    static std::priority_queue<PriorityNode*,
        std::vector<PriorityNode*>, priorityNodeCompare> openset;
    // hash from NodePtr -> id of the AuxContainer
    static _HashTable<NodePtr, size_t>::HashTable			nodePtrHashMap;
    static _HashTable<NodePtr, size_t>::HashTable::iterator	hashIt;
    static std::bitset<MAX_PATH_SIZE>		 				closedset;
    static std::bitset<MAX_PATH_SIZE> 						opensetChecker;
    static NodePtr neighbors[4];
    // the map where we will be putting the path
    static _HashTable<NodePtr, NodePtr>::HashTable			cameFromMap;


    // empty the openset if not empty
    while(!openset.empty()) openset.pop();

    cameFromMap.clear();
    ASSERT(openset.empty());
    closedset.reset();
    opensetChecker.reset();
    priorityNodes.restart();
    nodePtrHashMap.clear();



    // TODO: ver aca como hacer para hacer lo mas rapido posible el hecho de
    // agregar Priority nodes (capaz que creando un arreglo de maxPathLenght
    // y luego usar puntero (osea, allocar los PriorityNodes y luego usar
    // el openset con punteros solamente

    // set the first node to the openset
    size_t id;
    PriorityNode *pn = priorityNodes.getFreshObjt(id);

    pn->node = start;
    pn->hValue = EUCLIDEAN_DISTANCE((&startP), (&endP));
    pn->gValue = 0.0f;
    openset.push(pn);
    nodePtrHashMap[pn->node] = id;
    opensetChecker.set(id);

    static int i;
    static float tentative_g_score = 0.0f;
    static NodePtr neighbor;
    static bool tentative_is_better = false;
    static bool haveToAdd = false;
    static const GEdge *sharedEdge;

    PriorityNode *neighborPNode;
    size_t neighborID;

    min = 99999999.9f;

    // main algorithm
    while(!openset.empty()){
        // TODO: probablemente muy lento esto, hacemos copia por valor de todo
        // el nodo FIXME
        pn = openset.top();
        id = nodePtrHashMap[pn->node];
        openset.pop();
        opensetChecker.reset(id);
//		debug("Tomando nodo con valor: %f\n", pn->getFValue());

        // add the top value to the closed set (only the id we will use)
        closedset.set(id);

        if(pn->node == end){
            // we finish, we have to build the path
            break;
        }

        // get the neighbors
        pn->node->getNeighbors(neighbors);

        // iterate over all the neighbors
        for(i = 0; neighbors[i] != 0; ++i){
            neighbor = neighbors[i];

            // check if we can use this neighbor in base of the radius
            sharedEdge = pn->node->getSharedEdge(neighbor);
            if(sharedEdge->getLenght() < radius){
                // we cannot use this edge
                continue;
            }
            // get the min stretch in the path
            if(min > sharedEdge->getLenght()) min=sharedEdge->getLenght();

            // check if we have the neighbor in the map
            hashIt = nodePtrHashMap.find(neighbor);
            if(hashIt == nodePtrHashMap.end()){
                // add new PriorityNode to the hash
                neighborPNode = priorityNodes.getFreshObjt(neighborID);
                neighborPNode->node = neighbor;
                nodePtrHashMap[neighbor] = neighborID;
            } else {
                neighborID = hashIt->second;
                neighborPNode = priorityNodes[neighborID];
            }

            if(closedset.test(neighborID)){
                // nothing to do
                continue;
            }

            tentative_g_score = pn->gValue +
                    pn->node->getSquaredDistance(*neighbor);

            haveToAdd = false;
            if(!opensetChecker.test(neighborID)){
                // add this node to the openset
                neighborPNode->hValue = heuristic_function(sharedEdge,&endP);
                neighborPNode->gValue = g_function(sharedEdge, &startP);
                haveToAdd = true;
                tentative_is_better = true;
            } else if(tentative_g_score < neighborPNode->gValue){
                tentative_is_better = true;
            } else {
                tentative_is_better = false;
            }

            if(tentative_is_better){
                // TODO: agregar al path
                cameFromMap[neighbor] = pn->node;
                neighborPNode->gValue = tentative_g_score;
                // automatically f_value is updated
            }
            if(haveToAdd){
                openset.push(neighborPNode);
//				debug("Agregando nodo con valor: %f\n", neighborPNode->getFValue());
                opensetChecker.set(neighborID);
            }
        }
    }

    // now we have to build the path from the map cameFromMap
    _HashTable<NodePtr, NodePtr>::HashTable::iterator cfIt = cameFromMap.find(end),
            cfEnd = cameFromMap.end();
    int count = 0;
    mPath[count] = end;
    while(cfIt != cfEnd){
        ++count;
        mPath[count] = cfIt->second;
        cfIt = cameFromMap.find(cfIt->second);
    }
    size = ++count;
    mPath[count] = 0;
    debug("count: %d\t size:%zd\n", count, cameFromMap.size());
    //ASSERT(count == 0);

    // here we apply the funnel algorithm

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
TriangleAStar::TriangleAStar(const TriangleNavMesh *tnm, int maxPathSize) :
    mNavMesh(tnm)
,   mPath(0)
,   mMaxPathSize(maxPathSize)
,   mCache(CACHE_SIZE, maxPathSize)
,   mCurrentCall(0)
{
    ASSERT(tnm);
    ASSERT(maxPathSize < MAX_PATH_SIZE);
    mPath = new NodePtr[maxPathSize + 1];

    // resize the auxiliary vectors used in A* to the size of the max path size
    mClosedSetChecker.resize(MAX_PATH_SIZE, mCurrentCall);
    mOpenSetChecker.resize(MAX_PATH_SIZE, mCurrentCall);
    mMapCameFrom.reserve(MAX_PATH_SIZE);

}

////////////////////////////////////////////////////////////////////////////////
TriangleAStar::~TriangleAStar()
{
    delete []mPath;
}

////////////////////////////////////////////////////////////////////////////////
const TriangleAStar::Path
TriangleAStar::getshortestPath(const sm::Point &p1,
                               const sm::Point &p2,
                               size_t &size,
                               float radius,
                               float &min)
{

    // get the 2 Nodes
    NodePtr n1 = mNavMesh->getNodeFromPoint(p1);
    if(!n1){
        // no node found
        size = 0;
        return 0;
    }
    NodePtr n2 = mNavMesh->getNodeFromPoint(p2);
    if(!n2){
        // no node found
        size = 0;
        return 0;
    }

    // check if they are in the same node
    if(n1 == n2){
        mPath[0] = n1;
        size = 1;
        return mPath;
    }

    // There are not in the same node, now check the cache
    const Path cachedPath = getCachePath(n1,n2,size);
    if(cachedPath){
        // :)
        return cachedPath;
    }

    // There are nothing in the cache, start to search the shortest path
    findPath(size, n1, n2, p1, p2, radius, (min));
    if(size){
        // we find a path, add to the cache
        addToCache(mPath, size, n1, n2, radius);
    } else {
        // no path
        return 0;
    }

    return mPath;
}
