/*
 * OgreUtil.cpp
 *
 *  Created on: Aug 7, 2013
 *      Author: agustin
 */

#include "OgreUtil.h"

#include <OgreSubMesh.h>
#include <OgreMesh.h>

#include <set>
#include <bitset>
#include <cmath>

#include <debug/DebugUtil.h>
#include <types/StackVector.h>
#include <math/FloatComp.h>


// Helper methods
//
namespace {

// @brief Check if two vertices are equal or not using an epsilon as the
//        maximum squared distance between these two vertices to be considered
//        them as equals
//
inline bool
areVerticesEqual(const Ogre::Vector3& v1,
                 const Ogre::Vector3& v2,
                 float epsilon = FLOAT_COMP_THRESHOLD)
{
    return (v1.squaredDistance(v2) < epsilon);
}

}

namespace core {
namespace OgreUtil{

// @brief Get the mesh information (triangles and vertices from a mesh).
// @param mesh      The mesh that we want to extract the information
// @param vCount    [in|out] The max number of vertices we can handle
//                           And the resulting number of vertices read.
// @param vertices  The vertex buffer of size vCount where we will read all the
//                  vertices.
// @param iCount    [in|out] The size of the indices buffer.
//                           And the resulting number of indices read.
// @param indices   The buffer of size iCount where we will put the indices of
//                  the triangles.
// @param transform The transform we want to apply to each vertex, if
//                  transform == Ogre::Matrix4::IDENTITY no transformation will be
//                  applied
// @return true on success | false otherwise
//
bool
getMeshInformation(const Ogre::Mesh* const mesh,
                   core::size_t &vCount,
                   Ogre::Vector3* vertices,
                   core::size_t &iCount,
                   unsigned long* indices,
                   const Ogre::Matrix4& transform)
{
    const core::size_t maxVertices = vCount;
    const core::size_t maxIndices = iCount;
    bool added_shared = false;
    core::size_t current_offset = 0;
    core::size_t shared_offset = 0;
    core::size_t next_offset = 0;
    core::size_t index_offset = 0;

    vCount = iCount = 0;

    // Calculate how many vertices and indices we're going to need
    for ( unsigned short i = 0, size = mesh->getNumSubMeshes(); i < size; ++i) {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);
        // We only need to add the shared vertices once
        if(submesh->useSharedVertices) {
            if( !added_shared ) {
                vCount += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        } else {
            vCount += submesh->vertexData->vertexCount;
        }
        // Add the indices
        iCount += submesh->indexData->indexCount;
    }

    // check if we have enough space
    if (vCount > maxVertices || iCount > maxIndices) {
        debugERROR("We cannot read all the vertices or indices on the current"
            " buffers: iCount: %d | %d,\t vCount: %d | %d\n", iCount, maxIndices,
            vCount, maxVertices);
        return false;
    }

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for (unsigned short i = 0, size = mesh->getNumSubMeshes(); i < size; ++i) {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data = submesh->useSharedVertices ?
            mesh->sharedVertexData : submesh->vertexData;

        if ((!submesh->useSharedVertices) ||
            (submesh->useSharedVertices && !added_shared)) {
            if(submesh->useSharedVertices) {
                added_shared = true;
                shared_offset = current_offset;
            }

            const Ogre::VertexElement* posElem =
                vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf =
                vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char* vertex =
                static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Real will
            //  be comiled/typedefed as double:
            //Ogre::Real* pReal;
            float* pReal;

            for( core::size_t j = 0, vertCount = vertex_data->vertexCount;
                j < vertCount; ++j, vertex += vbuf->getVertexSize()) {
                posElem->baseVertexPointerToElement(vertex, &pReal);
                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);
                vertices[current_offset + j] = transform * pt;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }

        Ogre::IndexData* index_data = submesh->indexData;
        core::size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        unsigned long* pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

        core::size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

        if ( use32bitindexes ) {
            for ( size_t k = 0; k < numTris*3; ++k) {
                indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
            }
        } else {
            for ( size_t k = 0; k < numTris*3; ++k) {
                indices[index_offset++] = static_cast<unsigned long>(pShort[k]) +
                                          static_cast<unsigned long>(offset);
            }
        }

        ibuf->unlock();
        current_offset = next_offset;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
removeDuplicated(Ogre::Vector3* vertices,
                 unsigned int& vCount,
                 unsigned long* indices,
                 unsigned int& iCount)
{
    ASSERT(vertices);
    ASSERT(indices);
    if (vCount == 0) {
        debugWARNING("You are trying to remove duplicated from a empty list?\n");
        return;
    }

    // We will do an slow algorithm here but easy to implement.
    //
    ASSERT(vCount < 1024 && "To much vertices?\n");
    core::StackVector<unsigned int, 1024> modifiedIndices;
    core::StackVector<Ogre::Vector3, 1024> verts;
    std::set<unsigned int> toRemove;
    modifiedIndices.resize(vCount);
    for (unsigned int i = 0; i < vCount; ++i) {
        modifiedIndices[i] = i;
    }

    // copy the verts
    for (unsigned int i = 0; i < vCount; ++i) {
        verts.push_back(vertices[i]);
    }

    // now we will remove all the vertices we consider equal
    for (unsigned int i = 0; i < vCount; ++i) {
        for (unsigned int j = i+1; j < vCount; ++j) {
            if (areVerticesEqual(verts[i], verts[j])) {
                // we will remove this position and let the index to be pointing
                // to the ith element since is the same than this one (jth).
                modifiedIndices[j] = std::min(modifiedIndices[i], modifiedIndices[j]);
                if (modifiedIndices[j] < modifiedIndices[i]) {
                    modifiedIndices[i] = modifiedIndices[j];
                    toRemove.insert(i);
                }
                toRemove.insert(j);
            }
        }
    }

    // now we will remove each vertex from the array and update for each case
    // the indices
    unsigned int offset = 0;
    for (std::set<unsigned int>::iterator it = toRemove.begin(), endIt = toRemove.end();
            it != endIt; ++it) {
        const unsigned int currentIndex = *it - offset;
        verts.erase(currentIndex);

        // now update the modified indices for each case
        unsigned int begin = *it+1;
        for (; begin < modifiedIndices.size(); ++begin) {
            if (modifiedIndices[begin] >= currentIndex) {
                modifiedIndices[begin] -= 1;
            }
        }
        ++offset;
    }

    // now we will remove all the vertices that are duplicated, to do this we
    unsigned int newVCount = vCount - toRemove.size();
#ifdef DEBUG
    // we will ensure that all the vertices are pointing to the first newVCount
    // vertices
    //
    for (unsigned int i = 0; i < vCount; ++i) {
        if (modifiedIndices[i] >= newVCount) {
            debugRED("modifiedIndices[%d]: %d, newVCount: %d\n", i, modifiedIndices[i], newVCount);
        }
        ASSERT(modifiedIndices[i] < newVCount);
    }
#endif
    // we will update the triangles also, to point to the unique indices
    for (unsigned int i = 0; i < iCount; ++i) {
        ASSERT(indices[i] < vCount);
        ASSERT(modifiedIndices[indices[i]] < newVCount);
        indices[i] = modifiedIndices[indices[i]];
    }

    // now we update the indices
    vCount = newVCount;
    // copy again the memory into vertices
    std::memcpy(vertices, verts.begin(), sizeof(Ogre::Vector3) * vCount);
}



} /* namespace OgreUtil */
} /* namespace core */
