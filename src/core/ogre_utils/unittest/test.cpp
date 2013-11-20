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
#include <math/FloatComp.h>
#include <types/basics.h>
#include <types/StackVector.h>
#include <types/StackQueue.h>
#include <types/StackPriorityQueue.h>
#include <types/BoolCountingMask.h>

using namespace core;

typedef StackVector<int, 512> SV512;
typedef StackQueue<int, 512> SQ512;
typedef StackPriorityQueue<int, 512> SPQ512;


inline bool
areVerticesEqual(const Ogre::Vector3& v1,
                 const Ogre::Vector3& v2,
                 float epsilon = FLOAT_COMP_THRESHOLD)
{
    return (v1.squaredDistance(v2) < epsilon);
}

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

// Check for the removeDuplicated method
//
TEST(CheckGetContourVertices)
{
    static const unsigned int NumTris = 7;
    static const unsigned int NumVerts = NumTris*3; // duplicate all
    static const float step = M_PI_2 / NumTris;
    Ogre::Vector3 verts[NumVerts];
    Ogre::Vector3 sortedVerts[NumVerts];

    // we will create an circle of 7 sides

    // potitionate each of them
    unsigned int numVert = 0;
    unsigned int numRealVert = 0;
    for (unsigned int i = 0; i < NumTris; ++i) {
        verts[numVert++] = Ogre::Vector3(0,0,0); // center
        verts[numVert++] = Ogre::Vector3(50.f * std::cos(i * step),
                                         50.f * std::sin(i * step),
                                         0);
        verts[numVert++] = Ogre::Vector3(50.f * std::cos(((i+1)%NumTris) * step),
                                         50.f * std::sin(((i+1)%NumTris) * step),
                                         0);

        sortedVerts[numRealVert] = Ogre::Vector3(50.f * std::cos(i * step),
                                                 50.f * std::sin(i * step),
                                                 0);
        ++numRealVert;
    }

    // reverse them and check that we are getting the same
    Ogre::Vector3 rverts[NumVerts];
    for (int i = NumVerts-1, k = 0; i >= 0; --i, ++k) {
        rverts[k] = verts[i];
    }

    CHECK_EQUAL(NumVerts, numVert);
    unsigned long indices[NumVerts];
    unsigned long rindices[NumVerts];
    for (unsigned int i = 0; i < NumVerts; ++i) {
        indices[i] = i;
        rindices[i] = i;
    }
    unsigned int vCount = NumVerts, iCount = NumVerts;
    unsigned int rvCount = NumVerts, riCount = NumVerts;

    CHECK(OgreUtil::getContourVertices(verts, vCount, indices, iCount));
    CHECK(OgreUtil::getContourVertices(rverts, rvCount, rindices, riCount));
    for (unsigned int i = 0; i < numRealVert; ++i) {
        std::cout << "sortedVerts: " << sortedVerts[i] << std::endl;
    }

    CHECK_EQUAL(7, vCount);
    CHECK_EQUAL(7, rvCount);
    CHECK_EQUAL(NumVerts, iCount);
    CHECK_EQUAL(NumVerts, riCount);

    // check that we maintain the order
    int start = -1;
    int rstart = -1;
    for (int i = 0; i < vCount; ++i) {
        if (areVerticesEqual(verts[0], sortedVerts[i])) {
            start = i;
            break;
        }
    }
    for (int i = 0; i < vCount; ++i) {
        if (areVerticesEqual(rverts[0], sortedVerts[i])) {
            rstart = i;
            break;
        }
    }
    CHECK(start >= 0);
    CHECK(rstart >= 0);
    for (unsigned int i = 1; i < vCount; ++i) {
        CHECK_EQUAL(verts[i], sortedVerts[(i+start) % numRealVert]);
    }
    for (unsigned int i = 1; i < rvCount; ++i) {
        CHECK_EQUAL(rverts[i], sortedVerts[(i+rstart) % numRealVert]);
    }

}


int
main(void)
{
    return UnitTest::RunAllTests();
}



