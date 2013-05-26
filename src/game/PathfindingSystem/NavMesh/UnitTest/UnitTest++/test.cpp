/*
<<<<<<< HEAD
 * test.cpp
 *
 *  Created on: 09/01/2013
 *      Author: agustin
 */

// global
#include <UnitTest++/UnitTest++.h>
#include <string>
#include <vector>

#include <OgreRenderWindow.h>
#include <OgreEntity.h>

#include <AppTester.h>
#include <TriNavMeshBuilder.h>
#include <GlobalObjects.h>
#include <Graph.h>
#include <Util.h>

/////////////////////       Auxiliar functions      ///////////////////////////
//

static void
loadOgreSystem(void)
{
    // Leak memory :)
    AppTester *ogreApp = new AppTester(true, false);

    // hide the window
    ogreApp->getRenderWindow()->setVisible(false);
}


template<typename _T>
inline void
freeElements(std::vector<_T> &vec)
{
    for(size_t i = 0, size = vec.size(); i < size; ++i){
        delete vec[i];
    }
    vec.clear();
}

// TESTS

static Ogre::Entity *pathfindingMesh = 0;
static PolyStructsContainer<core::Vertex *> vertexs;
static PolyStructsContainer<Triangle *> triangles;
static TriangleNavMesh triNavMesh;

TEST(LoadResources)
{
    pathfindingMesh = GLOBAL_SCN_MNGR->createEntity("pathfinding.mesh");
    CHECK(pathfindingMesh != 0);
}

TEST(CreateGraphFromMesh)
{
    CHECK(pathfindingMesh != 0);

    vertexs.removeAndFreeAll();
    triangles.removeAndFreeAll();

    // Create graph
    const bool b = Common::Util::getTrianglesFromMesh(vertexs,
                                                      triangles,
                                                      pathfindingMesh->getMesh());
    CHECK(b);

    // build the triangle nav mesh
    std::vector<Triangle *> &verVec = triangles.getObjs();
    for(size_t i = 0, size = verVec.size(); i < size; ++i){
        triNavMesh.addTriangle(verVec[i]);
    }
    CHECK_EQUAL(0, triNavMesh.build());
}

TEST(CheckExporter)
{
    CHECK(pathfindingMesh != 0);
    const Graph &graph = triNavMesh.getGraph();
    CHECK(TriNavMeshBuilder::exportGraph(graph, "fromGraph.pfg"));
}

TEST(CheckImporter)
{
    CHECK(pathfindingMesh != 0);
    std::vector<GNode *> localNodes;
    PolyStructsContainer<core::Vertex *> localVertexs;
    PolyStructsContainer<Triangle *> localTriangles;
    std::vector<GEdge *> localEdges;

    CHECK(TriNavMeshBuilder::importGraph(localNodes, localVertexs, localTriangles,
                                   localEdges, "fromGraph.pfg"));

    // build the local trinavmesh and compare later the graph with the one we
    // have, they should be equal
    std::vector<Triangle *> &verVec = localTriangles.getObjs();
    TriangleNavMesh localTriNavMesh;
    for(size_t i = 0, size = verVec.size(); i < size; ++i){
        localTriNavMesh.addTriangle(verVec[i]);
    }
    CHECK_EQUAL(0, localTriNavMesh.build());
    CHECK(triNavMesh.getGraph() == localTriNavMesh.getGraph());

    localTriNavMesh.destroy();
    localVertexs.removeAndFreeAll();
    localTriangles.removeAndFreeAll();
}
int main()
{
    // Create ogre app
    loadOgreSystem();

    // run the tests
    return UnitTest::RunAllTests();
}
=======
 * test++.cpp
 *
 *  Created on: Apr 11, 2013
 *      Author: agustin
 */


#include <string>
#include <fstream>

#include <UnitTest++/UnitTest++.h>

#include <PathfindingSystem/AbstractGraph/Graph.h>

// Helper methods
//

// Load a graph from a file
static bool
loadGraphFromFile(const std::string &fName,
                  Graph &graph)
{

}

TEST(FailSpectacularly)
{
    CHECK(false);
}

int main()
{
    return UnitTest::RunAllTests();
}

>>>>>>> PlayerMovement
