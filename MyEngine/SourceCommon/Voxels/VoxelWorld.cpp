//
// Copyright (c) 2016 Jimmy Lord http://www.flatheadgames.com
//
// This software is provided 'as-is', without any express or implied warranty.  In no event will the authors be held liable for any damages arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "EngineCommonHeader.h"
#include "VoxelBlock.h"
#include "VoxelChunk.h"
#include "VoxelWorld.h"

VoxelWorld::VoxelWorld()
{
    m_NumChunkPointersAllocated = 0;
    m_pWorldChunkPtrs = 0;

    m_WorldSize.Set( 0, 0, 0 );
    m_ChunkSize.Set( 16, 16, 16 );
    m_BlockSize.Set( 1, 1, 1 );
}

VoxelWorld::~VoxelWorld()
{
    delete[] m_pWorldChunkPtrs;

    for( CPPListNode* pNode = m_pChunksFree.GetHead(); pNode; )
    {
        VoxelChunk* pChunk = (VoxelChunk*)pNode;
        pNode = pNode->GetNext();

        delete pChunk;
    }

    for( CPPListNode* pNode = m_pChunksVisible.GetHead(); pNode; )
    {
        VoxelChunk* pChunk = (VoxelChunk*)pNode;
        pNode = pNode->GetNext();

        delete pChunk;
    }
}

void VoxelWorld::Initialize(Vector3Int worldsize)
{
    // Make sure init is only called once.
    MyAssert( m_NumChunkPointersAllocated == 0 );
    
    SetWorldSize( worldsize );

    Vector3 chunkoffset = m_ChunkSize.MultiplyComponents( m_BlockSize );

    for( int z=0; z<m_WorldSize.z; z++ )
    {
        for( int y=0; y<m_WorldSize.y; y++ )
        {
            for( int x=0; x<m_WorldSize.x; x++ )
            {
                VoxelChunk* pChunk = MyNew VoxelChunk;
                m_pChunksFree.AddHead( pChunk );

                int chunkindex = z * m_WorldSize.y * m_WorldSize.x + y * m_WorldSize.x + x;
                m_pWorldChunkPtrs[chunkindex] = pChunk;

                Vector3Int chunkoffsetinblocks = m_ChunkSize.MultiplyComponents( Vector3Int(x,y,z) );
                PrepareChunk( Vector3( chunkoffset.x * x, chunkoffset.y * y, chunkoffset.z * z ), m_ChunkSize, chunkoffsetinblocks );
            }
        }
    }
    
    for( int z=0; z<m_WorldSize.z; z++ )
    {
        for( int y=0; y<m_WorldSize.y; y++ )
        {
            for( int x=0; x<m_WorldSize.x; x++ )
            {
                int chunkindex = z * m_WorldSize.y * m_WorldSize.x + y * m_WorldSize.x + x;
                VoxelChunk* pChunk = m_pWorldChunkPtrs[chunkindex];

                pChunk->RebuildMesh();
            }
        }
    }
}

void VoxelWorld::SetWorldSize(Vector3Int worldsize)
{
    // TODO: remove this once function is properly implemented
    MyAssert( m_NumChunkPointersAllocated == 0 );
    
    unsigned int pointersneeded = worldsize.x * worldsize.y * worldsize.z;

    if( pointersneeded > m_NumChunkPointersAllocated )
    {
        // TODO: allocate an array with enough pointers for the new size if it's bigger than the old,
        //       copy the existing chunk ptrs over
        //       delete the old array

        m_NumChunkPointersAllocated = pointersneeded;
        m_pWorldChunkPtrs = MyNew VoxelChunk*[pointersneeded];
    }

    m_WorldSize = worldsize;
}

void VoxelWorld::PrepareChunk(Vector3 pos, Vector3Int size, Vector3Int offset)
{
    VoxelChunk* pChunk = (VoxelChunk*)m_pChunksFree.GetHead();
    if( pChunk == 0 )
    {
        LOGInfo( "VoxelWorld", "Attempting to prepare chunk, but none available\n" );
        return;
    }

    pChunk->Initialize( this, pos, size, offset );

    m_pChunksVisible.MoveTail( pChunk );
}

void VoxelWorld::UpdateVisibility(void* pUserData)
{
    // TODO: Update list of visible chunks

    // Add all visible chunks to scene graph
    for( CPPListNode* pNode = m_pChunksVisible.GetHead(); pNode; pNode = pNode->GetNext() )
    {
        VoxelChunk* pChunk = (VoxelChunk*)pNode;

        pChunk->AddToSceneGraph( pUserData );
    }
}

bool VoxelWorld::IsBlockEnabled(Vector3Int pos)
{
    return IsBlockEnabled( pos.x, pos.y, pos.z );
}

bool VoxelWorld::IsBlockEnabled(int x, int y, int z)
{
    if( x < 0 || y < 0 || z < 0 || x >= m_WorldSize.x*m_ChunkSize.x || y >= m_WorldSize.y*m_ChunkSize.y || z >= m_WorldSize.z*m_ChunkSize.z )
        return false;

    Vector3Int chunkpos( x/m_ChunkSize.x, y/m_ChunkSize.y, z/m_ChunkSize.z );

    VoxelChunk* pChunk = m_pWorldChunkPtrs[chunkpos.z * m_WorldSize.y * m_WorldSize.x + chunkpos.y * m_WorldSize.x + chunkpos.x];

    return pChunk->IsBlockEnabled( x, y, z );
}
