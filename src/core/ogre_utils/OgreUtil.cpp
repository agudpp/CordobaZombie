/*
 * OgreUtil.cpp
 *
 *  Created on: Aug 7, 2013
 *      Author: agustin
 */

#include "OgreUtil.h"

#include <OgreSubMesh.h>
#include <OgreMesh.h>

#include <debug/DebugUtil.h>


// Helper methods
//
namespace {


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



} /* namespace OgreUtil */
} /* namespace core */
