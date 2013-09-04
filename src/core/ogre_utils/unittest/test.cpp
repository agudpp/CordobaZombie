/*
 * test.cpp
 *
 *  Created on: Jun 22, 2013
 *      Author: agustin
 */

#include <OgreVector3.h>

#include <algorithm>
#include <vector>
#include <cmath>
#include <set>
#include <UnitTest++/UnitTest++.h>

#include <ogre_utils/OgreUtil.h>
#include <types/basics.h>
#include <types/StackVector.h>
#include <types/StackQueue.h>
#include <types/StackPriorityQueue.h>
#include <types/BoolCountingMask.h>

using namespace core;

typedef StackVector<int, 512> SV512;
typedef StackQueue<int, 512> SQ512;
typedef StackPriorityQueue<int, 512> SPQ512;

// Check for the removeDuplicated method
//
TEST(CheckRemoveDuplicated)
{
    static const unsigned int NumTris = 7;
    static const unsigned int NumVerts = NumTris*3; // duplicate all
    static const float step = M_PI_2 / NumTris;
    Ogre::Vector3 verts[NumVerts];

    // we will create an circle of 7 sides

    // potitionate each of them
    unsigned int numVert = 0;
    for (unsigned int i = 0; i < NumTris; ++i) {
        verts[numVert++] = Ogre::Vector3(0,0,0); // center
        verts[numVert++] = Ogre::Vector3(50.f * std::cos(i * step),
                                         50.f * std::sin(i * step),
                                         0);
        verts[numVert++] = Ogre::Vector3(50.f * std::cos(((i+1)%NumTris) * step),
                                         50.f * std::sin(((i+1)%NumTris) * step),
                                         0);
    }
    CHECK_EQUAL(NumVerts, numVert);
    unsigned long indices[NumVerts];
    for (unsigned int i = 0; i < NumVerts; ++i) {
        indices[i] = i;
    }
    unsigned int vCount = NumVerts, iCount = NumVerts;

    OgreUtil::removeDuplicated(verts, vCount, indices, iCount);
    CHECK_EQUAL(8, vCount);
    CHECK_EQUAL(NumVerts, iCount);

    // check the positions of the vertices that are the same that we expect
    numVert = 0;
    for (unsigned int i = 0; i < NumTris; ++i) {
        Ogre::Vector3 v1(0,0,0); // center
        Ogre::Vector3 v2(50.f * std::cos(i * step),
                         50.f * std::sin(i * step),
                         0);
        Ogre::Vector3 v3(50.f * std::cos(((i+1)%NumTris) * step),
                         50.f * std::sin(((i+1)%NumTris) * step),
                         0);
        CHECK(indices[numVert] < vCount);
        CHECK_EQUAL(v1, verts[indices[numVert++]]);
        CHECK(indices[numVert] < vCount);
        CHECK_EQUAL(v2, verts[indices[numVert++]]);
        CHECK(indices[numVert] < vCount);
        CHECK_EQUAL(v3, verts[indices[numVert++]]);
    }
}


int
main(void)
{
    return UnitTest::RunAllTests();
}



