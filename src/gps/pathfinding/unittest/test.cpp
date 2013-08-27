/*
 * test.cpp
 *
 *  Created on: Jun 22, 2013
 *      Author: agustin
 */

#include <algorithm>
#include <vector>
#include <set>

#include <UnitTest++/UnitTest++.h>

#include <debug/DebugUtil.h>
#include <types/StackVector.h>
#include <pathfinding/WayPointAStar.h>
#include <pathfinding/WayPointGraph.h>
#include <pathfinding/WayPointDefines.h>
#include <pathfinding/WayPointGraphBuilder.h>
#include <pathfinding/WPPathfinder.h>

using namespace core;

typedef gps::WayPointNode WPN;
typedef gps::WayPointPath WPP;
typedef gps::WayPointGraph WPG;
typedef gps::WayPointAStar WPAS;
typedef gps::WayPointGraphBuilder WPGB;
typedef gps::WPPathfinder WPPF;
typedef gps::Vertex V;
typedef gps::WPNodeStackVec SVN512;
typedef gps::VertexStackVec SVV512;


// Simple graph checking with one agent
//
TEST(GraphCreation)
{
    WPG g;
    SVN512 nodes;
    SVV512 verts;
    verts.push_back(V(0,0));
    verts.push_back(V(0,100));
    verts.push_back(V(50,80));
    verts.push_back(V(250,150));

    nodes.resize(verts.size());
    // set the position
    //
    for (unsigned int i = 0; i < nodes.size(); ++i) {
        nodes[i].position = verts[i];
    }

    nodes[0].addNeighbor(2);
    nodes[1].addNeighbor(2);
    nodes[2].addNeighbor(0);
    nodes[2].addNeighbor(1);
    nodes[2].addNeighbor(3);
    nodes[3].addNeighbor(2);

    CHECK(g.build(nodes));


    // create invalid graph
    nodes[0].addNeighbor(544);
    CHECK(!g.build(nodes));
}

TEST(GraphImportExport)
{
    // create a graph using the builder
    WPGB b;
    WPG g;
    SVV512 verts;
    verts.push_back(V(0,0));
    verts.push_back(V(0,100));
    verts.push_back(V(50,80));
    verts.push_back(V(250,150));

    b.setWaypointPosition(verts);
    b.linkWaypoints(0,2);
    b.linkWaypoints(1,2);
    b.linkWaypoints(2,3);
    CHECK(b.buildGraph(g));

    // save and load
    CHECK(g.save("g.wpg"));
    WPG g2;
    CHECK(g2.build("g.wpg"));
    CHECK(g == g2);
}

TEST(AStarSimpleCheck)
{
    WPGB b;
    WPG g;
    SVV512 verts;
    verts.push_back(V(0,0));
    verts.push_back(V(0,100));
    verts.push_back(V(50,80));
    verts.push_back(V(250,150));
    verts.push_back(V(880,150));

    b.setWaypointPosition(verts);
    b.linkWaypoints(0,2);
    b.linkWaypoints(1,2);
    b.linkWaypoints(2,3);
    b.linkWaypoints(1,3);
    CHECK(b.buildGraph(g));

    // check for for a simple path
    WPAS as;
    WPP p;
    as.setGraph(&g);
    CHECK_EQUAL(WPP::Type::SAME_POINT,as.getPath(0, 0, p));
    CHECK_EQUAL(WPP::Type::IMPOSSIBLE, as.getPath(0, 4, p));

    CHECK_EQUAL(WPP::Type::NORMAL, as.getPath(0, 1, p));
    CHECK_EQUAL(3, p.size);
    CHECK_EQUAL(verts[1], p.node[0]);
    CHECK_EQUAL(verts[2], p.node[1]);
    CHECK_EQUAL(verts[0], p.node[2]);

    CHECK_EQUAL(WPP::Type::NORMAL, as.getPath(0, 3, p));
    CHECK_EQUAL(3, p.size);
    CHECK_EQUAL(verts[3], p.node[0]);
    CHECK_EQUAL(verts[2], p.node[1]);
    CHECK_EQUAL(verts[0], p.node[2]);

    // always return the same path?
    for (int i = 0; i < 10; ++i) {
        CHECK_EQUAL(WPP::Type::NORMAL, as.getPath(0, 3, p));
        CHECK_EQUAL(3, p.size);
        CHECK_EQUAL(verts[3], p.node[0]);
        CHECK_EQUAL(verts[2], p.node[1]);
        CHECK_EQUAL(verts[0], p.node[2]);
    }

    CHECK_EQUAL(WPP::Type::NORMAL, as.getPath(1, 3, p));
    CHECK_EQUAL(2, p.size);
    CHECK_EQUAL(verts[3], p.node[0]);
    CHECK_EQUAL(verts[1], p.node[1]);

}

TEST(SimplePathfinderTest)
{
    debugRED("TODO\n");
}



int
main(void)
{
    return UnitTest::RunAllTests();
}



