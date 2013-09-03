/*
 * test.cpp
 *
 *  Created on: Jun 22, 2013
 *      Author: agustin
 */

#include <algorithm>
#include <vector>
#include <set>
#include <cmath>

#include <UnitTest++/UnitTest++.h>

#include <math/FloatComp.h>
#include <types/basics.h>
#include <types/StackVector.h>
#include <types/StackQueue.h>
#include <types/StackPriorityQueue.h>
#include <types/BoolCountingMask.h>
#include <collisions/CollisionHandler.h>
#include <collisions/CollDefines.h>
#include <collisions/CollObject.h>
#include <collisions/CollPreciseInfo.h>
#include <collisions/CollPreciseInfoBuilder.h>

using namespace core;

typedef StackVector<int, 512> SV512;
typedef StackQueue<int, 512> SQ512;
typedef StackPriorityQueue<int, 512> SPQ512;
typedef coll::CollisionHandler CH;
typedef coll::CollObject CO;
typedef coll::CollPreciseInfo CPI;
typedef coll::CollPreciseInfoBuilder CPIB;
typedef coll::QueryArgs QA;
typedef coll::QueryResult QR;
typedef core::AABB BB;


// TODO: REMOVE THIS SHIT
//
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Collision/b2Collision.h>


TEST(TestBox2D)
{
    b2Manifold manifold;
    b2Vec2 vertices[3];
    vertices[0].Set(0.0f, 0.0f);
    vertices[1].Set(15.0f, 3.0f);
    vertices[2].Set(10.0f, 10.0f);
    int32 count = 3;
    b2PolygonShape polygon;
    polygon.Set(vertices, count);

    vertices[0].Set(-2.0f, 5.0f);
    vertices[1].Set(17.0f, 6.5f);
    vertices[2].Set(0.0f, 12.0f);
    b2PolygonShape polygon2;
    polygon2.Set(vertices, count);

    b2Transform t;
    t.SetIdentity();
    b2CollidePolygons(&manifold, &polygon, t, &polygon2, t);
    CHECK(manifold.pointCount > 0);
    CHECK(b2TestOverlap(&polygon, 0, &polygon2, 0, t, t));


    // get two collObjects and make them collide
    CH ch;
    BB bb(20,0,0,20);
    QA args;
    QR r;

    CO* o1 = ch.getNewCollObject();
    CO* o2 = ch.getNewCollObject();
    CO* o3 = ch.getNewCollObject();

    core::Vector2 verts[3];
    verts[0] = core::Vector2(1.0f, 0.0f);
    verts[1] = core::Vector2(15.0f, 3.0f);
    verts[2] = core::Vector2(10.0f, 10.0f);
    CPI* cp1 = CPI::createPolygonPrecise(verts, 3);
    o1->setPreciseInfo(cp1);

    verts[0] = core::Vector2(-2.0f, 5.0f);
    verts[1] = core::Vector2(17.0f, 6.5f);
    verts[2] = core::Vector2(0.0f, 12.0f);
    CPI* cp2 = CPI::createPolygonPrecise(verts, 3);
    o2->setPreciseInfo(cp2);

    CHECK(o1->collideBB(*o2));
    CHECK(o1->collidePrecise(*o2));
    CHECK(o2->collideBB(*o1));
    CHECK(o2->collidePrecise(*o1));

    // create a empty collision object (without precise info)
    o3->setBoundingBox(BB(12,-4,0,-0.5));
    CHECK(o1->collideBB(*o3) == false);
    CHECK(o1->collidePrecise(*o3) == false);
    CHECK(o2->collideBB(*o3));
    CHECK(o2->collidePrecise(*o3));
    CHECK(o3->collideBB(*o1) == false);
    CHECK(o3->collidePrecise(*o1) == false);
    CHECK(o3->collideBB(*o2));
    CHECK(o3->collidePrecise(*o2));

    // move one polygon outside
    b2Transform t2;
    t2.Set(b2Vec2(100,100), 0);
    b2CollidePolygons(&manifold, &polygon, t, &polygon2, t2);
    CHECK(manifold.pointCount == 0);
    CHECK(!b2TestOverlap(&polygon, 0, &polygon2, 0, t, t2));
}



// Simple graph checking with one agent
//
TEST(CheckSimpleCollision)
{
    CH ch;
    BB bb(20,0,0,20);
    QA args;
    QR r;


    ch.setWorldBoundingBox(bb);
    CO* o = ch.getNewCollObject();
    CHECK(o != 0);
    CHECK(ch.exists(o));
    o->setBoundingBox(BB(10,0,0,10));
    ch.coAddStatic(o);

    // check that we are not colliding with anything
    CHECK_EQUAL(false, ch.performQuery(o, args, r));
    CHECK(r.objects.empty());
    CHECK(r.points.empty());

    // create a new object
    CO* o2 = ch.getNewCollObject();
    CHECK(o2 != o);
    CHECK(ch.exists(o2));
    o2->setBoundingBox(BB(5,-5,0,1));
    CHECK(o2->collideBB(*o));
    CHECK(o->collideBB(*o2));

    // we should not be colliding with anything still
    CHECK_EQUAL(false, ch.performQuery(o, args, r));
    CHECK(r.objects.empty());
    CHECK(r.points.empty());

    // add the element and check that we are colliding
    ch.coAddDynamic(o2);
    CHECK_EQUAL(true, ch.performQuery(o, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK(r.points.empty());
    CHECK_EQUAL(o2, r.objects[0]);
    CHECK_EQUAL(true, ch.performQuery(o2, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK(r.points.empty());
    CHECK_EQUAL(o, r.objects[0]);

}

// Check destroy
TEST(CheckDestroy)
{
    CH ch;
    BB bb(20,0,0,20);
    QA args;
    QR r;

    ch.setWorldBoundingBox(bb);
    CO* o = ch.getNewCollObject();
    CHECK(o != 0);
    CHECK(ch.exists(o));
    o->setBoundingBox(BB(10,0,0,10));
    ch.coAddStatic(o);

    // create a new object
    CO* o2 = ch.getNewCollObject();
    o2->setBoundingBox(BB(5,-5,0,1));
    CHECK(o2->collideBB(*o));
    CHECK(o->collideBB(*o2));
    ch.coAddDynamic(o2);
    CHECK_EQUAL(true, ch.performQuery(o, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK(r.points.empty());
    CHECK_EQUAL(o2, r.objects[0]);
    CHECK_EQUAL(true, ch.performQuery(o2, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK(r.points.empty());
    CHECK_EQUAL(o, r.objects[0]);

    ch.destroy();
    CHECK_EQUAL(false, ch.performQuery(o, args, r));
    CHECK_EQUAL(0, r.objects.size());
    CHECK_EQUAL(false, ch.performQuery(o2, args, r));
    CHECK_EQUAL(0, r.objects.size());
}

// check updating the bounding box
TEST(CheckUpdateBB)
{
    CH ch;
    BB bb(20,0,0,20);
    QA args;
    QR r;

    ch.setWorldBoundingBox(bb);

    // create two dynamic objects and let them collide, then move one of them
    // to not collide, test that, then move them again to collide again and check
    // it again
    //
    CO* o = ch.getNewCollObject();
    CHECK(o != 0);
    o->setBoundingBox(BB(10,0,0,10));
    ch.coAddDynamic(o);
    CO* o2 = ch.getNewCollObject();
    o2->setBoundingBox(BB(5,-5,0,1));
    CHECK(o2->collideBB(*o));
    CHECK(o->collideBB(*o2));
    ch.coAddDynamic(o2);
    CHECK_EQUAL(true, ch.performQuery(o2, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK_EQUAL(o, r.objects.back());

    // move it outside the collision zone
    o2->setBoundingBox(BB(15,0,11,10));
    ch.update(); // simulate one frame
    CHECK_EQUAL(false, ch.performQuery(o2, args, r));
    CHECK_EQUAL(0, r.objects.size());
    CHECK_EQUAL(false, ch.performQuery(o, args, r));
    CHECK_EQUAL(0, r.objects.size());

    // put it again in the collision zone
    o2->setBoundingBox(BB(6,4,5,5));
    ch.update(); // simulate one frame
    CHECK_EQUAL(true, ch.performQuery(o2, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK_EQUAL(o, r.objects.back());
    CHECK_EQUAL(true, ch.performQuery(o, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK_EQUAL(o2, r.objects.back());
}

// check setting enable / disable
TEST(CheckSetEnableDisableColl)
{
    CH ch;
    BB bb(20,0,0,20);
    QA args;
    QR r;

    ch.setWorldBoundingBox(bb);

    // create two dynamic objects and let them collide, disable one of them
    // test, disable the other, test, enable the other and test, enable the second
    // and test..
    //
    CO* o = ch.getNewCollObject();
    CHECK(o != 0);
    o->setBoundingBox(BB(10,0,0,10));
    ch.coAddDynamic(o);
    CO* o2 = ch.getNewCollObject();
    o2->setBoundingBox(BB(5,-5,0,1));
    CHECK(o2->collideBB(*o));
    CHECK(o->collideBB(*o2));
    ch.coAddDynamic(o2);
    CHECK_EQUAL(true, ch.performQuery(o2, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK_EQUAL(o, r.objects.back());

    // move it outside the collision zone
    o2->setCollisionsEnabled(false);
    CHECK_EQUAL(false, ch.performQuery(o2, args, r));
    CHECK_EQUAL(0, r.objects.size());
    CHECK_EQUAL(false, ch.performQuery(o, args, r));
    CHECK_EQUAL(0, r.objects.size());

    o->setCollisionsEnabled(false);
    CHECK_EQUAL(false, ch.performQuery(o2, args, r));
    CHECK_EQUAL(0, r.objects.size());
    CHECK_EQUAL(false, ch.performQuery(o, args, r));
    CHECK_EQUAL(0, r.objects.size());

    o2->setCollisionsEnabled(true);
    CHECK_EQUAL(false, ch.performQuery(o2, args, r));
    CHECK_EQUAL(0, r.objects.size());
    CHECK_EQUAL(false, ch.performQuery(o, args, r));
    CHECK_EQUAL(0, r.objects.size());

    o->setCollisionsEnabled(true);
    CHECK_EQUAL(true, ch.performQuery(o2, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK_EQUAL(o, r.objects.back());
    CHECK_EQUAL(true, ch.performQuery(o, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK_EQUAL(o2, r.objects.back());
}

// check translating / setting a new position
TEST(CheckTranslating_NewPos)
{
    CH ch;
    BB bb(100,0,0,100);
    QA args;
    QR r;

    ch.setWorldBoundingBox(bb);

    // create two dynamic objects and let them collide, then translate one of them
    // to not collide, test that, then set the position of the other to
    // let them collide again and check it
    //
    CO* o = ch.getNewCollObject();
    CHECK(o != 0);
    o->setBoundingBox(BB(10,0,0,10));
    ch.coAddDynamic(o);
    CO* o2 = ch.getNewCollObject();
    o2->setBoundingBox(BB(5,-5,0,1));
    CHECK(o2->collideBB(*o));
    CHECK(o->collideBB(*o2));
    ch.coAddDynamic(o2);
    CHECK_EQUAL(true, ch.performQuery(o2, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK_EQUAL(o, r.objects.back());

    // move it outside the collision zone
    o2->translate(core::Vector2(20,20));
    ch.update(); // simulate one frame
    CHECK_EQUAL(false, ch.performQuery(o2, args, r));
    CHECK_EQUAL(0, r.objects.size());
    CHECK_EQUAL(false, ch.performQuery(o, args, r));
    CHECK_EQUAL(0, r.objects.size());

    // put it again in the collision zone
    o->setPosition(o->position() + core::Vector2(20,20));
    ch.update(); // simulate one frame
    CHECK_EQUAL(true, ch.performQuery(o2, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK_EQUAL(o, r.objects.back());
    CHECK_EQUAL(true, ch.performQuery(o, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK_EQUAL(o2, r.objects.back());
}

// check removing the element
TEST(CheckRemoving)
{
    CH ch;
    BB bb(100,0,0,100);
    QA args;
    QR r;

    ch.setWorldBoundingBox(bb);

    // create two dynamic objects and let them collide, remove one of them and
    // they should be available to collide, add it again and they should collide
    // again
    //
    CO* o = ch.getNewCollObject();
    CHECK(o != 0);
    o->setBoundingBox(BB(10,0,0,10));
    ch.coAddDynamic(o);
    CO* o2 = ch.getNewCollObject();
    o2->setBoundingBox(BB(5,-5,0,1));
    CHECK(o2->collideBB(*o));
    CHECK(o->collideBB(*o2));
    ch.coAddDynamic(o2);
    CHECK_EQUAL(true, ch.performQuery(o2, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK_EQUAL(o, r.objects.back());

    // move it outside the collision zone
    ch.coRemoveDynamic(o2);
    CHECK_EQUAL(false, ch.performQuery(o2, args, r));
    CHECK_EQUAL(0, r.objects.size());
    CHECK_EQUAL(false, ch.performQuery(o, args, r));
    CHECK_EQUAL(0, r.objects.size());

    // put it again in the collision zone
    ch.coAddDynamic(o2);
    CHECK_EQUAL(true, ch.performQuery(o2, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK_EQUAL(o, r.objects.back());
    CHECK_EQUAL(true, ch.performQuery(o, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK_EQUAL(o2, r.objects.back());
}

// check with different masks
TEST(CheckMasks)
{
    CH ch;
    BB bb(100,0,0,100);
    QA args;
    QR r;

    ch.setWorldBoundingBox(bb);

    // create 2 dynamic objects and let them collide, change the mask for one
    // of them and perform a query using another mask. be sure to not get that
    // perform the same query using the same mask than the object (they should be
    // intersected)
    //
    CO* o = ch.getNewCollObject();
    CHECK(o != 0);
    o->setBoundingBox(BB(10,0,0,10));
    ch.coAddDynamic(o);
    CO* o2 = ch.getNewCollObject();
    o2->setBoundingBox(BB(5,-5,0,1));
    CHECK(o2->collideBB(*o));
    CHECK(o->collideBB(*o2));
    ch.coAddDynamic(o2);
    CHECK_EQUAL(true, ch.performQuery(o2, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK_EQUAL(o, r.objects.back());

    // change the mask
    o2->setMask(0x2);
    args.mask = ~0 & !0x2;
    CHECK_EQUAL(false, ch.performQuery(o, args, r));
    CHECK_EQUAL(0, r.objects.size());

    // change the mask again
    args.mask = 2;
    CHECK_EQUAL(true, ch.performQuery(o, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK_EQUAL(o2, r.objects.back());
    args.mask = ~0;
    CHECK_EQUAL(true, ch.performQuery(o, args, r));
    CHECK_EQUAL(1, r.objects.size());
    CHECK_EQUAL(o2, r.objects.back());

}

// Hard collisions tests
TEST(CheckMultipleCollisions)
{
    CH ch;
    BB bb(200,0,0,200);
    QA args;
    QR result;


    ch.setWorldBoundingBox(bb);

    // define some variables
    static const unsigned int numRows = 12;
    static const unsigned int numCols = 12;
    static const float sizeCellY = 12;
    static const float sizeCellX = 12;
    static const float eps = 1;

    core::StackVector<BB, numRows * numCols> bbs;
    core::StackVector<CO*, numRows * numCols> cobs;
    core::StackVector<CO*, 4096> cos, cos2;

    bool dynamic = true;
    for (unsigned int c = 0; c < numCols; ++c) {
        for (unsigned int r = 0; r < numRows; ++r) {
            const float top = sizeCellY * (r+1) + eps;
            const float bottom = sizeCellY * r - eps;
            const float right = sizeCellX * (c+1) + eps;
            const float left = sizeCellX * c - eps;

            bbs.push_back(BB(top, left, bottom, right));
            cobs.push_back(ch.getNewCollObject(~0, bbs.back()));
            // we will add one dynamic one static
            if (dynamic) {
                ch.coAddDynamic(cobs.back());
            } else {
                ch.coAddStatic(cobs.back());
            }
            // the new implmementation does not support update static objects
            // and that is correct
            // dynamic = !dynamic;
        }
    }

    // now for each position that matchs with the matrix, we should get 4 || 6 || 9
    // intersections and should be
    //
    CO* o = ch.getNewCollObject();
    CHECK(o != 0);
    ch.coAddDynamic(o);
    for (unsigned int c = 0; c < numCols; ++c) {
        for (unsigned int r = 0; r < numRows; ++r) {
            const float top = sizeCellY * (r+1);
            const float bottom = sizeCellY * r;
            const float right = sizeCellX * (c+1);
            const float left = sizeCellX * c;
            o->setBoundingBox(BB(top, left, bottom, right));
            ch.update(); // simulate one frame
            CHECK_EQUAL(true, ch.performQuery(o, args, result));

            if (c == 0 || c == (numCols-1)) {
                if (r == 0 || r == (numRows-1)) {
                    // corner cases
                    CHECK_EQUAL(4, result.objects.size());
                } else {
                    CHECK_EQUAL(6, result.objects.size());
                }
            } else if (r == 0 || r == (numRows-1)) {
                CHECK_EQUAL(6, result.objects.size());
            } else {
                CHECK_EQUAL(9, result.objects.size());
            }
            for (unsigned int i = 0; i < result.objects.size(); ++i) {
                cos.push_back(result.objects[i]);
            }
        }
    }

    // now we will move all the dynamic elements with the same translation vector
    // and then check again the collisions
    const core::Vector2 tvec(sizeCellX, sizeCellY);

    for (unsigned int i = 0; i < cobs.size(); ++i) {
        cobs[i]->translate(tvec);
    }

    for (unsigned int c = 0; c < numCols; ++c) {
        for (unsigned int r = 0; r < numRows; ++r) {
            const float top = sizeCellY * (r+1);
            const float bottom = sizeCellY * r;
            const float right = sizeCellX * (c+1);
            const float left = sizeCellX * c;
            o->setBoundingBox(BB(top, left, bottom, right));
            o->translate(tvec);
            ch.update(); // simulate one frame
            CHECK_EQUAL(true, ch.performQuery(o, args, result));

            if (c == 0 || c == (numCols-1)) {
                if (r == 0 || r == (numRows-1)) {
                    // corner cases
                    CHECK_EQUAL(4, result.objects.size());
                } else {
                    CHECK_EQUAL(6, result.objects.size());
                }
            } else if (r == 0 || r == (numRows-1)) {
                CHECK_EQUAL(6, result.objects.size());
            } else {
                CHECK_EQUAL(9, result.objects.size());
            }
            for (unsigned int i = 0; i < result.objects.size(); ++i) {
                cos2.push_back(result.objects[i]);
            }
        }
    }

    // check that the results are the same? NOTE that we cannot ensure the order
    // of the results here.. so we will use a set
    std::set<CO*> checked;
    for (unsigned int i = 0; i < cos.size(); ++i) {
        checked.insert(cos[i]);
    }
    for (unsigned int i = 0; i < cos2.size(); ++i) {
        CHECK(checked.find(cos2[i]) != checked.end());
    }
}

// check different queries
TEST(CheckCollisionObjectQuery)
{
    debugERROR("TODO\n");
}

// TODO: implement the other queries here


// check dynamic / static collisions
TEST(CheckDynamicStaticCollisions)
{
    debugERROR("TODO\n");
}

// check different argument queries (for more precision information)
TEST(CheckArgumentQueries)
{
    debugERROR("TODO\n");
}



////////////////////////////////////////////////////////////////////////////////

// Try to build different circles from a cloud of points
TEST(CheckCollPreciseInfoBuilderCircle)
{
    {
        core::Vector2 verts[6];
        const float step6 = (2*M_PI) / 6;
        for (int i = 0; i < 6; ++i) {
            verts[i].x = 50.f * std::cos(i * step6);
            verts[i].y = 50.f * std::sin(i * step6);
        }

        CPIB b;
        b.setInfo(verts, 6, 0.1f);
        CHECK_EQUAL(CPIB::Type::PIT_CIRCLE, b.getBestType());
        CHECK_EQUAL(true, core::fcomp_equal(b.getRadius(), 50.f));
    }
    {
        core::Vector2 verts[16];
        const float step6 = (2*M_PI) / 16;
        for (int i = 0; i < 16; ++i) {

            verts[i].x = 150.f * std::cos(i * step6) + 0.01;
            verts[i].y = 150.f * std::sin(i * step6) + 0.01;
        }

        CPIB b;
        b.setInfo(verts, 16, 0.1f);
        CHECK_EQUAL(CPIB::Type::PIT_CIRCLE, b.getBestType());
        CHECK_EQUAL(true, core::fcomp_equal(b.getRadius(), 150.f));
    }
}

// Try to build different AABB from a cloud of points
TEST(CheckCollPreciseInfoBuilderAABB)
{
    {
        // check a perfect AABB
        core::Vector2 verts[4] = {
            {0,0},
            {10,0},
            {10,10},
            {0,10},
        };

        CPIB b;
        b.setInfo(verts, 4, 0.1f);
        CHECK_EQUAL(CPIB::Type::PIT_SIMPLE_AABB, b.getBestType());
        const core::AABB& bb = b.getBoundingBox();
        CHECK(core::fcomp_equal(bb.tl.x, 0));
        CHECK(core::fcomp_equal(bb.tl.y, 10));
        CHECK(core::fcomp_equal(bb.br.y, 0));
        CHECK(core::fcomp_equal(bb.br.x, 10));
    }
    {
        // little difference
        core::Vector2 verts[4] = {
            {0.1f, -0.1f},
            {9.9f, 0.1f},
            {10.1f, 10.1f},
            {-0.1f, 9.9f},
        };
        CPIB b;
        b.setInfo(verts, 4, 0.1f);
        CHECK_EQUAL(CPIB::Type::PIT_SIMPLE_AABB, b.getBestType());
        const core::AABB& bb = b.getBoundingBox();
        CHECK(core::fcomp_equal(bb.tl.x, -0.1f));
        CHECK(core::fcomp_equal(bb.tl.y, 10.1f));
        CHECK(core::fcomp_equal(bb.br.y, -0.1f));
        CHECK(core::fcomp_equal(bb.br.x, 10.1f));
    }
    {
        // multiple points but occupying almost all the area
        core::Vector2 verts[8] = {
            {0.f, 0.f},
            {5.f, 0.1f},
            {10.f, 0.f},
            {15.f, 0.3f},
            {15.4f, 18.f},
            {15.f, 25.f},
            {7.f, 24.9f},
            {0.1f, 24.8f},
        };
        CPIB b;
        b.setInfo(verts, 8, 0.1f);
        CHECK_EQUAL(CPIB::Type::PIT_SIMPLE_AABB, b.getBestType());
        const core::AABB& bb = b.getBoundingBox();
        CHECK(core::fcomp_equal(bb.tl.x, 0.f));
        CHECK(core::fcomp_equal(bb.tl.y, 25.f));
        CHECK(core::fcomp_equal(bb.br.y, 0.f));
        CHECK(core::fcomp_equal(bb.br.x, 15.4f));
    }

    {
        // create an invalid BB and check that we are not getting that
        core::Vector2 verts[4] = {
            {0.f, 15.f},
            {20.0f, 0.f},
            {30.0f, 14.0f},
            {13.0f, 30.0f},
        };
        CPIB b;
        b.setInfo(verts, 4, 0.1f);
        CHECK(CPIB::Type::PIT_SIMPLE_AABB != b.getBestType());
    }
}

// Try to build different a polygon structure from a cloud of points
TEST(CheckCollPreciseInfoBuilderPolygon)
{

}


int
main(void)
{
    return UnitTest::RunAllTests();
}



